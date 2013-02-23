#ifndef NCVCANVAS_H
#define NCVCANVAS_H

#include "qglxframebufferobject.h"
#include "qglxtexture1d.h"
#include "qglxcamera.h"
#include "discretecolorselector.h"
#include "qglxpainter.h"
#include "ncvneuronset.h"
#include "ncvconnectionset.h"
#include "ncvdiscreteattribute.h"
#include "ncvcontinuousattribute.h"
#include <QTextDocument>
#include <QCoreApplication>
#include "time.h"




/*!
    \class NCV
    \author Alex Jones
    \brief The OpenGL Widget housing the core NCV rendering context.
*/
class NCVCanvas : public QGLWidget
{
    Q_OBJECT
public:

    /*!
        \param format The format of the OpenGL context used with this application.
        If the graphics hardware being used cannot support OpenGL 4, an error will be raised.
        \param parent The parent widget to house this canvas.
        \brief The OpenGL Widget housing the core NCV rendering context.
    */
    NCVCanvas( const QGLFormat&,QWidget* parent = 0 );
    ~NCVCanvas();

    /*!
        \brief This function indicates whether a proper OpenGL 4.X Context is established.
    */
    bool isValid();

public slots:
    void setNeurons(NCVNeuronSet * neurons);
    void setConnections(NCVConnectionSet * connections);


    bool isInitialized();


signals:

    void initialized();
    void frameRendered();
    void cameraUpdated(QGLXCamera camera);
    void invalidGraphicsConfigurationDetected();


protected:
    void initializeGL();
    void resizeGL( int w, int h );
    void paintEvent(QPaintEvent *);
    void wheelEvent(QWheelEvent *e);
    void keyPressEvent( QKeyEvent* e );
    void keyReleaseEvent(QKeyEvent *e);
    void mouseMoveEvent(QMouseEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void timerEvent(QTimerEvent *e);
    void leaveEvent(QEvent * e);
    void enterEvent(QEvent * e);


private:
    void m_performRegularRender();
    void m_performSelectionRender();
    void m_drawLegend();

    QGLXPainter m_painter;
    bool m_renderOnlySelection;
    QTime m_timer, m_idleTimer;
    GLfloat m_legendShowTime;
    NCVNeuronSet * m_neurons;
    NCVConnectionSet * m_connections;
    QMap<QString,QGLXTexture2D> m_maps;
    QGLXFrameBufferObject  m_frameBufferObject;
    QVector3D m_worldSize,m_worldCenter;
    int m_width, m_height;
    QGLXCamera m_camera;
    QVector2D m_mousePosition;
    QRect m_selectionRect;
    float m_moveSpeed, m_turnSpeed;
    bool m_leftMouseDown,m_rightMouseDown,m_shiftDown,m_renderNeurons,m_renderConnections,m_versionCapable,m_initialized;
    QGLXBuffer m_screenVertices,m_screenCoords;
    QSet<GLuint> m_selectedObjects;
    QVector<Range> m_ranges;
};

#endif // NCVCANVAS_H
