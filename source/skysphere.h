#ifndef SKYSPHERE_H
#define SKYSPHERE_H
#include "qglxbuffer.h"
#include "qglxcamera.h"
#include "qglxtexture2d.h"


class SkySphere
{
public:
    SkySphere();
    void create(float radius, QImage texture, QVector2D textureRepeat);
    void draw(QGLXCamera camera);
    void destroy();
    float radius();
    int rings();
    int sectors();

private:
    QGLXBuffer m_indexBuffer,m_coordinateBuffer,m_positionBuffer;
    QGLShaderProgram m_program;
    QGLXTexture2D m_texture;
    int m_count,m_rings,m_sectors;
    float m_radius;
};

#endif // SKYSPHERE_H
