#ifndef NCVGLWIDGET_H
#define NCVGLWIDGET_H

#include "qglxdynamicframebuffer.h"
#include "qglxsystem.h"
#include "qglxoctree.h"
#include "qglxcamera.h"

#ifdef WIN32
    #pragma comment(lib,"assimp.lib")
    #include <assimp/scene.h>
    #include <assimp/Importer.hpp>
    #include <assimp/postprocess.h>
#else
    #include <assimp/aiPostProcess.h>
    #include <assimp/assimp.hpp>
    #include <assimp/aiScene.h>

#endif
#include <QCoreApplication>
#include <QVector2D>
#include <QKeyEvent>
#include <QGLShaderProgram>
#include <QTime>
#include <QMap>
#include <QList>
#include <QTimer>
#include <QDir>
#include <QRect>
#include "time.h"

enum RenderSetting
{
    Diffuse,
    Position,
    Voltage,
    Firing,
    All
};

enum SelectionState
{
    None = 0,
    Regular = 1,
    Connectivity =2
};


/*!
    \class NCVGLWidget
    \author Alex Jones
    \brief The OpenGL Widget housing the core NCV rendering context.
*/
class NCVGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    enum AttributeAccess
    {
        ConnectionOnly,
        NeuronOnly,
        Shared
    };
    /*!
        \param format The format of the OpenGL context used with this application.
        If the graphics hardware being used cannot support OpenGL 4, an error will be raised.
        \param parent The parent widget to house this canvas.
        \brief The OpenGL Widget housing the core NCV rendering context.
    */
    NCVGLWidget( const QGLFormat& format, QWidget* parent = 0 );
    ~NCVGLWidget();

public slots:
    /*!
        \param number The number of connections being created
        \param inNeurons The ids of all incoming neurons  for these connections;
        \param outNeurons The ids of all outgoing neurons for these connections;
        \brief This function creates connections as soon as the OpenGL is ready.
        \note This function is asynchronous and thus non-blocking.
    */
    void createConnections(int number,GLuint * inNeurons,GLuint * outNeurons);

    /*!
        \param filename The filename of a .connection file that describes the connections.
        \brief This function creates connections from a file as soon as the OpenGL is ready.
        \note This function is asynchronous and thus non-blocking.
    */
    void createConnections(const QString & filename);

    /*!
        \param number The number of neurons being created
        \param positions The positions of these neurons in space.
        \brief This function creates neurons from a file as soon as the OpenGL is ready.
        \note This function is asynchronous and thus non-blocking.
    */
    void createNeurons(int number,QVector3D * positions );

    /*!
        \param filename The filename of a .neuron file that describes the neurons.
        \brief This function creates neurons from a file as soon as the OpenGL is ready.
        \note This function is asynchronous and thus non-blocking.
    */
    void createNeurons(const QString & filename );

    /*!
        \param name The name of the attribute being set.
        \param data The data to bind.
        \param stride The stride (in bytes) of each element in data;
        \param componentType The OpenGL data type used for this attribute.
        \brief This function binds an attribute that spans all neurons and all connections to the rendering context.
        \note All compound attributes are accessible by both neurons and connections;
    */
    void setCompoundAttribute(QString name, void * data, int stride, GLenum componentType, AttributeAccess access);

    /*!
        \param name The name of the attribute being set.
        \param data The data to bind.
        \param stride The stride (in bytes) of each element in data;
        \param componentType The OpenGL data type used for this attribute.
        \param access Describes what data set(s) can access this attribute.
        \brief This function binds an attribute that spans all connections to the rendering context.
    */
    void setConnectionAttribute(QString name, void * data, int stride, GLenum componentType,AttributeAccess access);

    /*!
        \param name The name of the attribute being set.
        \param data The data to bind.
        \param stride The stride (in bytes) of each element in data;
        \param componentType The OpenGL data type used for this attribute.
        \param access Describes what data set(s) can access this attribute.
        \brief This function binds an attribute that spans all neurons to the rendering context.
    */
    void setNeuronAttribute(QString name, void * data, int stride, GLenum componentType,AttributeAccess access);


    void setVisualizationParameter(const char * name, GLint value, AttributeAccess access);
    void setVisualizationParameter(const char * name, QVector2D value, AttributeAccess access);
    void setVisualizationParameter(const char * name, QVector3D value, AttributeAccess access);
    void setVisualizationParameter(const char * name,  QMatrix4x4 value, AttributeAccess access);
    void setVisualizationParameter(const char * name,GLuint value, AttributeAccess access);
    void setVisualizationParameter(const char * name,GLfloat value, AttributeAccess access);
    void setVisualizationParameter(const char * name,GLubyte value, AttributeAccess access);

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
    bool compileShaderProgram(QGLShaderProgram & program, const QString& vertexShaderPath,const QString& fragmentShaderPath );
    bool compileShaderProgram(QGLShaderProgram & program, const QString& vertexShaderPath, const QString & geometryShaderPath, const QString& fragmentShaderPath );

private:

    struct DataSet
    {
        enum AttributeType
        {
            Neuron,
            Connection,
            Compound
        };
        DataSet ()
        {
            this->data = NULL;
            this->stride = 0;
            this->componentType = 0;
        }

        DataSet (void * data, int stride, GLenum componentType,AttributeAccess access,AttributeType type, int divisor = 0)
        {
            this->data = data;
            this->stride = stride;
            this->componentType = componentType;
            this->divisor = divisor;
            this->type = type;
            this->access = access;
        }
        void * data;
        int stride;
        int divisor;
        GLenum componentType;
        AttributeType type;
        AttributeAccess access;
    };


    template< class T >
    struct Parameter
    {
        Parameter ()
        {
        }
        Parameter (T value, AttributeAccess access)
        {
            this->value = value;
            this->access = access;
        }
        AttributeAccess access;
        T value;
    };
    void setVisualizationParameters();
    void createNetwork();
    void createAttributes();
    void establishWorldBounds(int numNeurons,QVector3D * neuronPositions);
    void onViewChanged();
    void sizeTargets();


    int m_frameCount;
    QTime m_time;
    QGLXSystem m_neurons , m_connections;
    QGLXDynamicFrameBufferObject  m_frameTargetBuffer;
    QGLShaderProgram m_neuronProgram, m_connectionProgram, m_selectionRectProgram;
    QMatrix4x4 m_neuronScale;
    QVector3D m_worldSize,m_worldCenter;
    QGLXOctree<QVector3D> m_octree;
    RenderSetting m_renderSetting;
    QList<QString> m_firstPassTargets;
    int m_width, m_height;
    QGLXCamera m_camera;
    QVector2D m_mousePosition;
    QRect m_selectionRect;
    QMap<QString,DataSet> m_attributesToCreate;
    int m_neuronsToCreate,m_connectionsToCreate;
    float m_moveSpeed, m_turnSpeed;
    bool m_leftMouseDown,m_rightMouseDown,m_shiftDown,m_renderNeurons,m_renderConnections,m_newVisualizationParameters;
    SelectionState m_connectionSelectionState;
    QMap<QString,QGLXBuffer> m_textureBuffers;
    QGLXBuffer m_selectionRectVertices;
    QVector<GLubyte> m_selectionDomain;
    int m_startSelectionID;

    QMap<const char *,Parameter<QVector2D> > m_vector2DParametersToCreate;
    QMap<const char *,Parameter<QVector3D> > m_vector3DParametersToCreate;
    QMap<const char *,Parameter<QMatrix4x4> > m_mat4x4ParametersToCreate;
    QMap<const char *,Parameter<GLfloat> > m_floatParametersToCreate;
    QMap<const char *,Parameter<GLuint> > m_uintParametersToCreate;
    QMap<const char *,Parameter<GLint> > m_intParametersToCreate;
    QMap<const char *,Parameter<GLubyte> > m_ubyteParametersToCreate;

};

#endif // NCVGLWIDGET_H
