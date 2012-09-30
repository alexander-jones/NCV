#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QVector2D>
#include <QGLBuffer>
#include <QCoreApplication>
#include <QKeyEvent>
#include "camera.h"
#include "picker.h"

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    GLWidget( const QGLFormat& format, QWidget* parent = 0 );
    ~GLWidget();

    QMatrix4x4 GetWVP();

protected:
    virtual void initializeGL();
    virtual void resizeGL( int w, int h );
    virtual void paintGL();
    virtual void keyPressEvent( QKeyEvent* e );
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);
    virtual void timerEvent(QTimerEvent *e);
private:

    QGLShader * m_vertex, *m_fragment;
    QGLShaderProgram* m_program;
    Picker * m_picker;
    QGLBuffer m_vertexBuffer;
    Camera camera;
    QVector2D mousePosition;
    float moveSpeed, turnSpeed;
    bool mouseInitialized,mouseDown;
};

#endif // GLWIDGET_H
