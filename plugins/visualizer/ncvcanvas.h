#ifndef NCVCANVAS_H
#define NCVCANVAS_H

#include "qglx/qglxframebufferobject.h"
#include "qglx/qglxtexture1d.h"
#include "qglx/qglxcamera.h"
#include "qglx/qglxpainter.h"
#include "discretecolorselector.h"
#include "ncvneuronset.h"
#include "ncvconnectionset.h"
#include "ncvdiscreteattribute.h"
#include "ncvcontinuousattribute.h"
#include "time.h"
#include "skysphere.h"
#include <QTextDocument>
#include <QCoreApplication>
#include <QTime>


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
        \brief This function indicates whether a proper OpenGL 3.3 Context is established.
    */
    bool isValid();
	bool isInitialized();

public slots:
    void setNeurons(NCVNeuronSet * neurons);
    void setConnections(NCVConnectionSet * connections);
    void renderNeurons(bool render);
    void renderConections(bool render);
    void setSelection(QVector<NCVElementRange> selection, NCVSelectionFlag flags);
    void setBackgroundImage(QImage image);

signals:
    void selectionChanged(QVector<NCVElementRange> selection, NCVSelectionFlag flags);
	void deselectedRenderSet(bool on);
    void initialized();
    void frameRendered();
    void cameraUpdated(QGLXCamera camera);
    void invalidGraphicsConfigurationDetected();


protected:
    void initializeGL();
    void resizeGL( int w, int h );
    void paintEvent(QPaintEvent *e);
    void keyPressEvent( QKeyEvent* e );
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
    bool m_outsideOfWorld(QVector3D pos);

    QGLXPainter m_painter;
    SkySphere m_skySphere;
    NCVSelectionFlag m_NCVSelectionFlags;
    bool m_renderDirty,m_performSilhouetting;
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
    bool m_leftMouseDown,m_rightMouseDown,m_renderNeurons,m_renderConnections,m_versionCapable,m_initialized;
    QGLXBuffer m_screenVertices,m_screenCoords;
    QSet<GLuint> m_selectedObjects;
    QVector<NCVElementRange> m_ranges;
};

#endif // NCVCANVAS_H
