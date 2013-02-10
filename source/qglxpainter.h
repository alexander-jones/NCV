#ifndef QGLXPAINTER_H
#define QGLXPAINTER_H

#include <QPainter>
#include <QImage>
#include "qglxbuffer.h"
#include "qglxframebufferobject.h"

class QGLXPainter : public QPainter
{
public:
    QGLXPainter();
    QGLXPainter(QPaintDevice *);
    void begin(QPaintDevice *);
    void end();
    QRect rect();
private:
    QGLXTexture2D m_texture;
    QImage m_image;
    //QGLXFrameBufferObject m_fbo;
    QGLShaderProgram m_program;
    QGLXBuffer m_screenVertices,m_screenCoords;
};

#endif // QGLXPAINTER_H
