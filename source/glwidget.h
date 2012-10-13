#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "multipletargetframebuffer.h"

#include <QCoreApplication>
#include <QGLShader>
#include <QGLBuffer>
#include <QVector2D>
#include <QKeyEvent>

#include "camera.h"
#include "glhelper.h"

enum RenderSetting
{
    Diffuse,
    Normal,
    Position
};

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
    virtual void  wheelEvent(QWheelEvent *e);
    virtual void keyPressEvent( QKeyEvent* e );
    void keyReleaseEvent(QKeyEvent *e);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);
    virtual void timerEvent(QTimerEvent *e);
    void BindModel( QString filename);

private:
    void AddInstance(qreal x, qreal y, qreal z);

    MultipleTargetFrameBuffer  m_frameTargetBuffer;
    QGLShaderProgram m_firstPassProgram;
    QGLShaderProgram m_finalPassProgram;
    std::map<QString,QGLBuffer> m_buffers;
    QMatrix4x4 m_scale,m_spread;
    Assimp::Importer m_importer;
    QVector3D m_WorldSize;
    RenderSetting m_renderSetting;
    QString m_firstPassTargets[5];
    int m_width, m_height;
    Camera camera;
    QVector2D mousePosition;
    float moveSpeed, turnSpeed;
    bool mouseMovementInitialized,leftMouseDown,rightMouseDown,  renderPosition,renderNormal;
};

#endif // GLWIDGET_H
