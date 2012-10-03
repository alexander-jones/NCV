#ifndef MULTIPLETARGETFRAMEBUFFER_H
#define MULTIPLETARGETFRAMEBUFFER_H
#pragma comment( lib, "glew32.lib" )

#include <gl/glew.h>
#include <QGLWidget>
#include <QImage>
#include <algorithm>
#include <map>

struct FrameBufferTarget
{
    GLuint texture;
    GLenum format, dpi,attachment ;
    int width;
    int height;
};

class MultipleTargetFrameBuffer
{
public:

    MultipleTargetFrameBuffer();
    void Init();

    void Bind();
    void Release();

    void AddTarget(QString name, GLenum dpi, GLenum format, int width, int height);
    void RemoveTarget(QString name);

    void TargetToScreen(QString name, int x, int y, int width, int height);
    void SizeTargets(int width, int height);

    QColor GetTargetPixel(QString name,int x, int y);

private:
    GLuint  m_fbo;
    bool m_depthEnabled;
    std::vector<GLenum>   m_buffers ;
    std::map<QString,FrameBufferTarget>  m_targets;
};

#endif // MULTIPLETARGETFRAMEBUFFER_H
