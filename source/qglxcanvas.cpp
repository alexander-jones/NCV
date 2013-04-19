#include "qglxcanvas.h"
#include <QEvent>

QGLXCanvas::QGLXCanvas ( QWidget * parent , const QGLWidget * shareWidget , Qt::WindowFlags f )
    :QGLWidget(parent,shareWidget,f)
{
    this->installEventFilter(this);
}
QGLXCanvas::QGLXCanvas ( QGLContext * context, QWidget * parent , const QGLWidget * shareWidget, Qt::WindowFlags f )
    :QGLWidget(context,parent,shareWidget,f)
{
    this->installEventFilter(this);
}
QGLXCanvas::QGLXCanvas ( const QGLFormat & format, QWidget * parent, const QGLWidget * shareWidget, Qt::WindowFlags f )
    :QGLWidget(format,parent,shareWidget,f)
{
    this->installEventFilter(this);
}

bool QGLXCanvas::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);
    if (!isValid())
        return false;

    if (event->type() == QEvent::MouseButtonPress)
    {
        mousePressEvent((QMouseEvent *)event);
        return true;
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        mouseReleaseEvent((QMouseEvent *)event);
        return true;
    }
    else if (event->type() == QEvent::MouseMove)
    {
        mouseMoveEvent((QMouseEvent *)event);
        return true;
    }
    else if (event->type() == QEvent::KeyPress)
    {
        keyPressEvent((QKeyEvent *)event);
        return true;
    }
    else if (event->type() == QEvent::Paint)
    {
        paintEvent((QPaintEvent *)event);
        return true;
    }
    else if (event->type() == QEvent::Leave)
    {
        leaveEvent(event);
        return true;
    }
    else if (event->type() == QEvent::Enter)
    {
        enterEvent(event);
        return true;
    }
    return false;
}
