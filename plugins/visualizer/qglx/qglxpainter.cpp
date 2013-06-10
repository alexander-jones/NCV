#include "qglxpainter.h"

const char * vertex = "#version 330\n\
        in vec3 Vert_Position;\n\
        in vec2 Vert_Coord;\n\
        out vec2 TextureCoordinate;\n\
        void main( void )\n\
        {\n\
            TextureCoordinate = Vert_Coord;\n\
            gl_Position = vec4(Vert_Position,1.0f);\n\
        }\n";


const char * fragment = "#version 330\n\
        uniform usampler2D BlendMap;\n\
        in vec2 TextureCoordinate;\n\
        out vec4 Color;\n\
        vec4 hexToColor(uint hex)\n\
        {\n\
            float alpha = float(hex >> 24U )/255.0;\n\
            float red = float((hex >> 16U) & 255U) /255.0;\n\
            float green = float((hex >> 8U)& 255U)/255.0;\n\
            float blue = float(hex & 255U)/255.0;\n\
            return vec4(red,green,blue,alpha);\n\
        }\n\
        void main(void)\n\
        {\n\
            Color = hexToColor(texture(BlendMap, TextureCoordinate).r);\n\
        }\n";


const QVector3D screenVerts[4] = {QVector3D(-1,-1,0.5),QVector3D(1,-1,0.5),QVector3D(1,1,0.5),QVector3D(-1,1,0.5)};
const QVector2D screenCoords[4] = {QVector2D(0,1),QVector2D(1,1),QVector2D(1,0),QVector2D(0,0)};

QGLXPainter::QGLXPainter()
    :QPainter()
{
}

QGLXPainter::QGLXPainter(QPaintDevice *)
    :QPainter(){}


QGLXPainter::~QGLXPainter()
{
    if (isActive())
        end();
}

bool QGLXPainter::begin(QPaintDevice *device)
{
    if (!m_texture.isCreated())
        m_texture.create();

    if (device->width() != m_image.width() || device->height() != m_image.height())
        m_image = QImage(device->width(),device->height(),QImage::Format_ARGB32);
    else
        m_image.fill(QColor(0,0,0,0));

    QPainter::begin(&m_image);

    if (!m_screenVertices.isCreated())
    {
        m_screenVertices.create();
        m_screenVertices.allocate(&screenVerts[0],4 * sizeof(QVector3D),QGLXBuffer::StaticDraw);
    }

    if (!m_screenCoords.isCreated())
    {
        m_screenCoords.create();
        m_screenCoords.allocate(&screenCoords[0],4 * sizeof(QVector2D),QGLXBuffer::StaticDraw);
    }

    if (!m_program.isLinked())
    {
        m_program.addShaderFromSourceCode( QGLShader::Vertex, vertex);
        m_program.addShaderFromSourceCode( QGLShader::Fragment, fragment);
        m_program.link();
    }

    return true;
}

bool QGLXPainter::end()
{
    QPainter::end();

    m_texture.bind();
    m_texture.allocate(device()->width(),device()->height(),GL_R32UI,1,m_image.bits());
    m_texture.setMinFilter(QGLXTexture2D::Linear);
    m_texture.setMagFilter(QGLXTexture2D::Linear);
    m_texture.setWrapFunction(QGLXTexture2D::Clamp,QGLXTexture2D::Clamp);
    m_texture.release();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    m_program.bind();
    m_screenVertices.bind(QGLXBuffer::ArrayBuffer);
    m_program.setAttributeBuffer( "Vert_Position", GL_FLOAT,  0 ,3,sizeof(GL_FLOAT) * 3);
    m_program.enableAttributeArray("Vert_Position");
    glVertexAttribDivisor( m_program.attributeLocation("Vert_Position"),0);

    m_screenCoords.bind(QGLXBuffer::ArrayBuffer);
    m_program.setAttributeBuffer( "Vert_Coord", GL_FLOAT,  0 ,2,sizeof(GL_FLOAT) * 2);
    m_program.enableAttributeArray("Vert_Coord");
    glVertexAttribDivisor( m_program.attributeLocation("Vert_Coord"), 0);

    glEnable(GL_TEXTURE_2D);
    m_texture.bind(0);
    m_program.setUniformValue("BlendMap",0);

    glDrawArrays(GL_QUADS,0,4);

    glBindTexture(GL_TEXTURE_2D,0);
    m_screenCoords.release();
    m_screenVertices.release();
    m_program.release();
    glDisable(GL_BLEND);

    return true;
}
