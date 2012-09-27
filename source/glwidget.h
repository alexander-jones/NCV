#ifndef GLWIDGET_H
#define GLWIDGET_H
#include "camera.h"

#include <QGLWidget>
#include <QVector2D>
#include <QGLBuffer>
#include <QGLShaderProgram>

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    GLWidget( const QGLFormat& format, QWidget* parent = 0 );

protected:
    virtual void initializeGL();
    virtual void resizeGL( int w, int h );
    virtual void paintGL();
    virtual void keyPressEvent( QKeyEvent* e );
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);
private:

    bool prepareShaderProgram( const QString& vertexShaderPath,
                               const QString& fragmentShaderPath );

    QGLShaderProgram m_shader;
    QGLBuffer m_vertexBuffer;
    Camera camera;
    QVector2D mousePosition;
    float moveSpeed, turnSpeed;
    bool mouseInitialized,mouseDown;
};

#endif // GLWIDGET_H
