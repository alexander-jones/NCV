#ifndef QGLXCANVAS_H
#define QGLXCANVAS_H

#include <QGLWidget>

class QGLXCanvas : public QGLWidget
{
    Q_OBJECT
public:
    explicit QGLXCanvas ( QWidget * parent = 0, const QGLWidget * shareWidget = 0, Qt::WindowFlags f = 0 );
    explicit QGLXCanvas ( QGLContext * context, QWidget * parent = 0, const QGLWidget * shareWidget = 0, Qt::WindowFlags f = 0 );
    explicit QGLXCanvas ( const QGLFormat & format, QWidget * parent = 0, const QGLWidget * shareWidget = 0, Qt::WindowFlags f = 0 );
    
protected:
    bool eventFilter(QObject *, QEvent *);

signals:
    
public slots:
    
};

#endif // QGLXCANVAS_H
