#include "skysphere.h"


SkySphere::SkySphere()
{

    m_radius = 0;
    m_rings = 0;
    m_sectors = 0;
}

float SkySphere::radius()
{
    return m_radius;
}

int SkySphere::rings()
{
    return m_rings;
}

int SkySphere::sectors()
{
    return m_sectors;
}

void SkySphere::create(float radius, QImage texture, QVector2D textureRepeat)
{
    if (radius > 0.0f)
    {
        m_radius = radius;
        m_rings = 20;
        m_sectors = 20;

        float const R = 1./(float)(m_rings-1);
        float const S = 1./(float)(m_sectors-1);
        int r, s;
        \
        QVector<QVector3D> vertices;
        QVector<QVector2D> coordinates;
        for(r = 0; r < m_rings; r++)
            for(s = 0; s < m_sectors; s++)
            {
                QVector3D normal;
                normal.setY(sin( -M_PI_2 + M_PI * (float)r * R  ));

                normal.setX(cos(2.0f*M_PI * (float)s * S) * sin( M_PI * (float)r * R ) );
                normal.setZ(sin(2.0f*M_PI * (float)s * S) * sin( M_PI * (float)r * R ));


                coordinates.append(QVector2D((2.0f * s * S),1.0f-(r * R )  ) * textureRepeat);
                vertices.append(normal * radius);

            }

        m_count = (m_rings-1) * (m_sectors-1) * 6;
        QVector<GLuint> indices;
        for(r = 0; r < m_rings-1; r++)
            for(s = 0; s < m_sectors-1; s++)
            {
                indices.append( r * m_sectors + s);
                indices.append(r * m_sectors + (s+1));
                indices.append((r+1) * m_sectors + (s+1));

                indices.append( r * m_sectors + s);
                indices.append((r+1) * m_sectors + (s+1));
                indices.append((r+1) * m_sectors + s);
            }



        m_positionBuffer.create();
        m_positionBuffer.allocate( &vertices[0], vertices.size() * sizeof(QVector3D));

        m_coordinateBuffer.create();
        m_coordinateBuffer.allocate( &coordinates[0], coordinates.size()* sizeof(QVector2D));

        m_indexBuffer.create();
        m_indexBuffer.allocate( &indices[0], indices.size()* sizeof(GLuint));

        m_texture.create();

        QVector<QVector3D> linearData;
        for (int i = 0; i <texture.height() ;i++)
        {

            QRgb * rgbTexData = (QRgb*)texture.scanLine(i);
            for (int j = 0; j <texture.width() ;j++)
            {
                QColor col = QColor::fromRgb(rgbTexData[j]);
                linearData.append(QVector3D(col.redF(),col.greenF(),col.blueF()));
            }
        }

        m_texture.allocate(texture.width(),texture.height(),GL_RGB8,1,&linearData[0]);

        m_texture.bind();
        m_texture.setMinFilter(QGLXTexture2D::Linear);
        m_texture.setMagFilter(QGLXTexture2D::Linear);
        m_texture.setWrapFunction(QGLXTexture2D::Repeat,QGLXTexture2D::Repeat);
        m_texture.release();

        const char * vertex = "#version 330\n\
                uniform mat4 WVP;\n\
                in vec3 Vert_Position;\n\
                in vec2 Vert_Coord;\n\
                out vec2 TextureCoordinate;\n\
                void main( void )\n\
                {\n\
                    TextureCoordinate = Vert_Coord;\n\
                    vec4 pos = WVP *vec4(Vert_Position,1.0f);\n\
                    gl_Position = pos.xyww;\n\
                }\n";


        const char * fragment = "#version 330\n\
                uniform sampler2D Texture;\n\
                in vec2 TextureCoordinate;\n\
                out vec4 Color;\n\
                void main(void)\n\
                {\n\
                    Color = texture(Texture, TextureCoordinate);\n\
                }\n";

        m_program.addShaderFromSourceCode(QGLShader::Vertex,vertex );
        m_program.addShaderFromSourceCode(QGLShader::Fragment,fragment );
        m_program.link();
    }

}

void SkySphere::draw(QGLXCamera camera)
{

    glDisable(GL_DEPTH_TEST);
    m_program.bind();
    m_program.setUniformValue("WVP",camera.projection() * camera.view());

    m_indexBuffer.bind(QGLXBuffer::IndexBuffer);
    m_positionBuffer.bind(QGLXBuffer::ArrayBuffer);
    m_program.setAttributeBuffer( "Vert_Position", GL_FLOAT,  0 ,3,sizeof(GL_FLOAT) * 3);
    m_program.enableAttributeArray("Vert_Position");
    glVertexAttribDivisor( m_program.attributeLocation("Vert_Position"),0);

    m_coordinateBuffer.bind(QGLXBuffer::ArrayBuffer);
    m_program.setAttributeBuffer( "Vert_Coord", GL_FLOAT,  0 ,2,sizeof(GL_FLOAT) * 2);
    m_program.enableAttributeArray("Vert_Coord");
    glVertexAttribDivisor( m_program.attributeLocation("Vert_Coord"), 0);

    glActiveTexture(GL_TEXTURE0);
    m_texture.bind();
    m_program.setUniformValue("Texture",0);

    glDrawElements(GL_TRIANGLES,m_count ,GL_UNSIGNED_INT,0 );

    m_texture.release();
    m_coordinateBuffer.release();
    m_positionBuffer.release();
    m_indexBuffer.release();

    m_program.release();
    glEnable(GL_DEPTH_TEST);
}
void SkySphere::destroy()
{
    m_positionBuffer.destroy();
    m_coordinateBuffer.destroy();
    m_indexBuffer.destroy();
}
