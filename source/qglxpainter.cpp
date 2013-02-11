#include "qglxpainter.h"

QGLXPainter::QGLXPainter()
    :QPainter()
{
}

QGLXPainter::QGLXPainter(QPaintDevice *)
    :QPainter(){}

void QGLXPainter::begin(QPaintDevice *device)
{
    if (!m_texture.isCreated())
        m_texture.create();

    if (device->width() != m_image.width() || device->height() != m_image.height())
        m_image = QImage(device->width(),device->height(),QImage::Format_ARGB32);
    else
        m_image.fill(QColor(0,0,0,0));

    QPainter::begin(&m_image);
    QVector3D screenVerts[4] = {QVector3D(-1,-1,0.5),QVector3D(1,-1,0.5),QVector3D(1,1,0.5),QVector3D(-1,1,0.5)};
    QVector2D screenCoords[4] = {QVector2D(0,1),QVector2D(1,1),QVector2D(1,0),QVector2D(0,0)};

    if (!m_screenVertices.isCreated())
    {
        m_screenVertices.create();
        m_screenVertices.bind();
        m_screenVertices.setUsagePattern( QGLXBuffer::StaticDraw );
        m_screenVertices.allocate(&screenVerts[0],4 * sizeof(QVector3D),GL_FLOAT);
        m_screenVertices.release();
    }

    if (!m_screenCoords.isCreated())
    {
        m_screenCoords.create();
        m_screenCoords.bind();
        m_screenCoords.setUsagePattern( QGLXBuffer::StaticDraw );
        m_screenCoords.allocate(&screenCoords[0],4 * sizeof(QVector2D),GL_FLOAT);
        m_screenCoords.release();
    }

    if (!m_program.isLinked())
    {
        m_program.addShaderFromSourceFile( QGLShader::Vertex, ":/shaders/blend.vert" );
        m_program.addShaderFromSourceFile( QGLShader::Fragment, ":/shaders/blend.frag" );
        m_program.link();
    }

}
void QGLXPainter::end()
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
    m_screenVertices.bind();
    m_program.setAttributeBuffer( "Vert_Position", GL_FLOAT,  0 ,3,sizeof(GL_FLOAT) * 3);
    m_program.enableAttributeArray("Vert_Position");
    glVertexAttribDivisor( m_program.attributeLocation("Vert_Position"),0);

    m_screenCoords.bind();
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

}
