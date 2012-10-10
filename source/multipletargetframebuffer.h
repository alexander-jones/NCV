#ifndef MULTIPLETARGETFRAMEBUFFER_H
#define MULTIPLETARGETFRAMEBUFFER_H

#ifdef WIN32
    #pragma comment(lib,"glew32.lib")

#endif
#include <GL/glew.h>
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

    void BindTargets(int num, QString * names);
    void BindTarget(QString name);
    void BindTexture(QString name);
    void Release();

    void AddTarget(QString name, GLenum dpi, int width, int height,GLenum attachment);
    void RemoveTarget(QString name);
    FrameBufferTarget GetTargetInfo(QString name);

    void TargetToScreen(QString name, int x, int y, int width, int height);
    void SizeTargets(int width, int height);

    QColor GetTargetPixel(QString name,int x, int y);

private:
    GLuint  m_fbo;
    bool m_depthEnabled;
    std::map<QString,FrameBufferTarget>  m_targets;
};

#endif // MULTIPLETARGETFRAMEBUFFER_H
