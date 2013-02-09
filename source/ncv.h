#ifndef NCV_H
#define NCV_H

#include "qglxframebufferobject.h"
#include "qglxtexture1d.h"
#include "qglxsystem.h"
#include "qglxoctree.h"
#include "qglxcamera.h"
#include <QCoreApplication>
#include <QVector2D>
#include <QKeyEvent>
#include <QTime>
#include <QMap>
#include <QList>
#include <QLabel>
#include <QTimer>
#include <QDir>
#include <QRect>
#include "time.h"


struct Range
{
    int start;
    int end;
};

struct NetworkAttribute
{
    enum Owner
    {
        Neuron,
        Connection
    };
    enum Type
    {
        Range,
        Position,
        Flag,
        Invalid
    };
    QGLXBuffer * buffer;
    GLvoid * unboundBufferData;
    GLvoid * unboundTextureData;
    Type type;
    QVector3D onColor, offColor;
    float minValue, maxValue;
    Owner owner;
    QString name;
    int unboundTextureWidth;
};


/*!
    \class NCV
    \author Alex Jones
    \brief The OpenGL Widget housing the core NCV rendering context.
*/
class NCV : public QGLWidget
{
    Q_OBJECT
public:

    /*!
        \param format The format of the OpenGL context used with this application.
        If the graphics hardware being used cannot support OpenGL 4, an error will be raised.
        \param parent The parent widget to house this canvas.
        \brief The OpenGL Widget housing the core NCV rendering context.
    */
    NCV( const QGLFormat&,QWidget* parent = 0 );
    ~NCV();

    /*!
        \brief This function indicates whether a proper OpenGL 4.X Context is established.
    */
    bool isValid();

public slots:
    /*!
        \param number The number of connections being created
        \param inNeurons The ids of all incoming neurons  for these connections;
        \param outNeurons The ids of all outgoing neurons for these connections;
        \brief This function creates connections as soon as the OpenGL is ready.
        \note This function is asynchronous and thus non-blocking.
    */
    void createConnections(int number,GLuint * inNeurons,GLuint * outNeurons,GLuint * ids);

    /*!
        \param number The number of neurons being created
        \param positions The positions of these neurons in space.
        \brief This function creates neurons from a file as soon as the OpenGL is ready.
        \note This function is asynchronous and thus non-blocking.
    */
    void createNeurons(int number,QVector3D * positions,GLuint * ids );

    void createNeuronRangeAttribute(QString name,GLfloat minValue, GLfloat maxValue);

    void createNeuronFlagAttribute(QString name, QVector3D onColor,QVector3D offColor);

    void createConnectionRangeAttribute(QString name,GLfloat minValue, GLfloat maxValue);

    void createConnectionFlagAttribute(QString name, QVector3D onColor,QVector3D offColor);

    void setNeuronRangeAttribute(QString name, GLfloat * data);

    void setNeuronFlagAttribute(QString name, GLuint * data);

    void setConnectionRangeAttribute(QString name, GLfloat * data);

    void setConnectionFlagAttribute(QString name, GLuint * data);

    void setNeuronRangeColoration(QString name,QRgb * data, int width);

    void setNeuronFlagColoration(QString name, QColor offColor,QColor onColor);

    void setConnectionRangeColoration(QString name,QRgb * data, int width);

    void setConnectionFlagColoration(QString name,QColor offColor,QColor onColor);

    void setNeuronAttributeToRender(QString name);

    void setConnectionAttributeToRender(QString name);

    int connectionCount();

    int neuronCount();

    bool isInitialized();


signals:

    void initialized();
    void frameRendered();
    void cameraUpdated(QGLXCamera * camera);
    void invalidGraphicsConfigurationDetected();

    void newNeuronRangeAttribute(QString name, float minVal, float maxVal);
    void newNeuronBitAttribute(QString name, QColor offColor, QColor onColor);
    void newConnectionRangeAttribute(QString name, float minVal, float maxVal);
    void newConnectionBitAttribute(QString name, QColor offColor, QColor onColor);

protected:
    void initializeGL();
    void resizeGL( int w, int h );
    void paintGL();
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
    bool m_compileShaderProgram(QGLShaderProgram * program, QString vertexShaderPath,  QString fragmentShaderPath );
    bool m_compileShaderProgram(QGLShaderProgram * program, QString vertexShaderPath, QString geometryShaderPath, QString fragmentShaderPath );

    void m_bindAttribute(NetworkAttribute * attribute);
    void m_setVisualizationParameters();
    void m_createNeurons();
    void m_createConnections();
    void m_establishWorldBounds(int numNeurons,QVector3D * neuronPositions);
    void m_performRegularRender();
    void m_performSelectionRender();
    QGLShaderProgram *m_neuronProgram();
    QGLShaderProgram *m_connectionProgram();
    void m_bindNeurons(QGLShaderProgram * program);
    void m_bindConnections(QGLShaderProgram * program);
    void m_releaseNeurons(QGLShaderProgram * program);
    void m_releaseConnections(QGLShaderProgram * program);

    GLuint * m_neuronIDs,* m_connectionIDs, * m_connectionNeuronIDs;
    QPainter m_painter;
    bool m_renderOnlySelection;
    NetworkAttribute *m_neuronAttribToRender, *m_connectionAttribToRender;
    QTime m_timer;
    QMap<QString,NetworkAttribute> m_neuronAttributes;
    QMap<QString,NetworkAttribute> m_connectionAttributes;
    NetworkAttribute m_translationBuffer;
    QMap<QString,QGLXTexture2D> m_maps;
    QGLXTexture1D * m_neuronRangeMap, * m_connectionRangeMap;
    QGLXSystem m_neurons , m_connections;
    QGLXFrameBufferObject  m_frameBufferObject;
    QGLShaderProgram *m_flagNeuronProgram,*m_rangeNeuronProgram,*m_flagConnectionProgram,
    *m_rangeConnectionProgram,*m_silhouetteNeuronProgram,*m_silhouetteConnectionProgram;
    QMatrix4x4 m_neuronScale;
    QVector3D m_worldSize,m_worldCenter;
    int m_width, m_height;
    QGLXCamera * m_camera;
    QVector2D m_mousePosition;
    QRect m_selectionRect;
    int m_neuronsToCreate,m_connectionsToCreate;
    float m_moveSpeed, m_turnSpeed;
    bool m_leftMouseDown,m_rightMouseDown,m_shiftDown,m_renderNeurons,m_renderConnections,m_versionCapable,m_initialized;
    QGLXBuffer m_screenVertices,m_screenCoords;
    QSet<GLuint> m_selectedObjects;
    QVector<Range> m_ranges;
};

#endif // NCV_H
