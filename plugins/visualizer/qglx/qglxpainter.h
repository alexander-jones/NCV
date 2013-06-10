#ifndef QGLXPAINTER_H
#define QGLXPAINTER_H
#include "qglxbuffer.h"
#include "qglxframebufferobject.h"
#include <QGLShaderProgram>
#include <QPainter>
#include <QImage>

/*!
    \class QGLXPainter
    \author Alex Jones
    \brief This class allows for seamless interaction between QPainter calls and OpenGL 3+ draw calls.
    \note Implemented as Qt 4.8's QPainter utilizes fixed function pipeline calls to perform draw operations. See \htmlonly<a href="http://qt-project.org/doc/qt-4.8/qpainter.html"> QPainter</a>  \endhtmlonly
*/
class QGLXPainter : public QPainter
{
public:
    /*!
        \brief This constructor instantiates a QGLXPainter object.
    */
    QGLXPainter();
    /*!
        \param device The paint device to utilize for drawing.
        \brief This constructor instantiates a QGLXPainter object utilizing the paint engine of the specified device.
    */
    QGLXPainter(QPaintDevice * device);

    /*!
        \brief Destructor.
    */
    ~QGLXPainter();
    /*!
        \param device The paint device to utilize for drawing.
        \brief This call begins painting the paint device and returns true if successful; otherwise returns false.
    */
    bool begin(QPaintDevice * device);

    /*!
        \param device The paint device to utilize for drawing.
        \brief Ends painting. Any resources used while painting are released. You don't normally need to call this since it is called by the destructor.
    */
    bool end();

private:
    QGLXTexture2D m_texture;
    QImage m_image;
    QGLShaderProgram m_program;
    QGLXBuffer m_screenVertices,m_screenCoords;
};

#endif // QGLXPAINTER_H
