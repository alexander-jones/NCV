#ifndef NCV_H
#define NCV_H

#ifdef WIN32
    #pragma comment(lib,"assimp.lib")
#endif


#include "qglxframebufferobject.h"
#include "qglxtexture1d.h"
#include "qglxsystem.h"
#include "qglxoctree.h"
#include "qglxcamera.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <QCoreApplication>
#include <QVector2D>
#include <QKeyEvent>
#include <QTime>
#include <QMap>
#include <QList>
#include <QTimer>
#include <QDir>
#include <QRect>
#include "time.h"


struct Range
{
    int start;
    int end;
};


enum RangeInferenceFunction
{
    Max,
    Min,
    Subtract,
    Add,
    Interpolate
};

enum FlagInferenceFunction
{
    Or,
    And,
    Xor
};



class Attribute : public QGLXBuffer
{
public:
    enum AttributeOwner
    {
        Neuron,
        Connection
    };
    enum AttributeType
    {
        Range,
        Position,
        Flag,
        Invalid
    };
    Attribute( )
        :QGLXBuffer(QGLXBuffer::TextureBuffer)
    {
        this->owner = Neuron;
        unboundData = NULL;
        type = Invalid;
    }

    Attribute(AttributeOwner owner, GLvoid * unboundData)
        :QGLXBuffer(QGLXBuffer::TextureBuffer)
    {
        this->owner = owner;
        this->unboundData = unboundData;
        type = Position;
    }

    Attribute(AttributeOwner owner, QVector3D onColor, QVector3D offColor, GLvoid * data)
        :QGLXBuffer(QGLXBuffer::TextureBuffer)
    {
        this->onColor = onColor;
        this->offColor = offColor;
        this->owner = owner;
        this->unboundData = data;
        type = Flag;
    }
    Attribute(AttributeOwner owner,GLfloat minValue, GLfloat maxValue, GLvoid * data)
        :QGLXBuffer(QGLXBuffer::TextureBuffer)
    {
        this->minValue = minValue;
        this->maxValue = maxValue;
        this->owner = owner;
        this->unboundData = data;
        type = Range;
    }
    void bindBuffer(GLuint numNeurons, GLuint numConnections)
    {
        if (!isCreated())
            create();

        bind();

        if (unboundData != NULL)
        {
            if (type == Position)
            {
                GLuint componentSize = QGLXTexture::getComponentSize(GL_FLOAT);
                GLenum textureFormat  = QGLXTexture::bufferFormatToTextureFormat(GL_FLOAT,3,QGLXTexture::getComponentSize(GL_FLOAT));

                allocate(unboundData,3* componentSize * numNeurons,textureFormat);
            }
            else if (type == Range)
            {
                GLuint componentSize = QGLXTexture::getComponentSize(GL_FLOAT);
                GLenum textureFormat  = QGLXTexture::bufferFormatToTextureFormat(GL_FLOAT,1,QGLXTexture::getComponentSize(GL_FLOAT));

                if (owner == Neuron)
                    allocate(unboundData, componentSize * numNeurons,textureFormat);
                else
                    allocate(unboundData,componentSize * numConnections,textureFormat);
            }
            else if (type == Flag)
            {
                GLuint componentSize = QGLXTexture::getComponentSize(GL_UNSIGNED_BYTE);
                GLenum textureFormat  = QGLXTexture::bufferFormatToTextureFormat(GL_UNSIGNED_BYTE,1,QGLXTexture::getComponentSize(GL_UNSIGNED_BYTE));

                if (owner == Neuron)
                    allocate(unboundData, componentSize * numNeurons/8,textureFormat);
                else
                    allocate(unboundData,componentSize * numConnections/8,textureFormat);
            }
            delete [] unboundData;
            unboundData = NULL;
        }
    }

    AttributeOwner owner;
    AttributeType type;
    GLvoid * unboundData;
    QVector3D onColor, offColor;
    GLfloat minValue, maxValue;
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
    NCV( QWidget* parent = 0 );
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
        \param camera The camera to add to the camera set.
        \param name The name to identify this camera by
        \brief This function adds a camera to the camera set.
    */
    void addCamera(QGLXCamera *camera, QString name);


    /*!
        \param name The name of the camera being set.
        \brief This function sets a camera as the currentCamera.
    */
    void switchCamera(QString name);



    /*!
        \param name The name of the camera being removed.
        \brief This function removes a camera from the camera set.
    */
    void deleteCamera(QString name);

    /*!
        \param camera The camera to add to the camera set.
        \param name The name to identify this camera by
        \brief This function adds a camera to the camera set.
    */
    void addLight(QGLXLight *light, QString name);

    /*!
        \param name The name of the camera being removed.
        \brief This function removes a camera from the camera set.
    */
    void deleteLight(QString name);

    void setMaterial(QGLXMaterial *);

    void setNeuronRangeAttribute(QString name, GLfloat * data,GLfloat minValue, GLfloat maxValue, RangeInferenceFunction func = Interpolate);

    void setNeuronFlagAttribute(QString name, GLubyte * data, QVector3D onColor,QVector3D offColor, FlagInferenceFunction func = And);

    void setConnectionRangeAttribute(QString name, GLfloat * data,GLfloat minValue, GLfloat maxValue);

    void setConnectionFlagAttribute(QString name, GLubyte * data, QVector3D onColor,QVector3D offColor);

    void setNeuronAttributeToRender(QString name);

    void setConnectionAttributeToRender(QString name);

    //void setGroupAssociation(QString group, int startNeuron, int endNeuron, bool includeOutgoingConnections = false, QString parent = "");



signals:
    void newFPSReading(float framesPerSecond);
    void newCurrentCamera(QString name);
    void newCamera(QGLXCamera *camera, QString name);
    void cameraDeleted(QString name);
    void cameraUpdated(QString name);
    void newLight(QGLXLight* light, QString name);
    void lightDeleted(QGLXLight* light, QString name);
    void invalidGraphicsConfigurationDetected();

protected:
    void initializeGL();
    void resizeGL( int w, int h );
    void paintGL();
    void  wheelEvent(QWheelEvent *e);
    void keyPressEvent( QKeyEvent* e );
    void keyReleaseEvent(QKeyEvent *e);
    void mouseMoveEvent(QMouseEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void timerEvent(QTimerEvent *e);
    void leaveEvent(QEvent * e);
    void enterEvent(QEvent * e);


private:

    void m_setVisualizationParameters();
    void m_createNetwork();
    void m_establishWorldBounds(int numNeurons,QVector3D * neuronPositions);
    void m_performRegularRender();
    void m_performSelectionRender();
    QGLShaderProgram *m_neuronProgram();
    QGLShaderProgram *m_connectionProgram();
    void m_bindNeurons();
    void m_bindConnections();
    void m_releaseNeurons();
    void m_releaseConnections();

    int m_frameCount;
    float m_fps;
    bool m_renderOnlySelection;
    GLuint *m_inNeurons, *m_outNeurons;
    Attribute *m_neuronAttribToRender, *m_connectionAttribToRender;
    QString m_currentCamera;
    QTime m_timer;
    QMap<QString,Attribute> m_attributes;
    Attribute m_translationBuffer;
    QMap<QString,QGLXTexture2D> m_maps;
    QGLXTexture1D * m_neuronRangeMap, * m_connectionRangeMap;
    QMap<QString,QGLXLight *> m_lights;
    QGLXSystem m_neurons , m_connections;
    QGLXFrameBufferObject  m_frameBufferObject;
    QGLShaderProgram *m_blendProgram,*m_selectionRectProgram,*m_lightingProgram,*m_selectionProgram, *m_bitNeuronProgram,
    *m_floatNeuronProgram,*m_bitConnectionProgram,*m_floatConnectionProgram ;
    QMatrix4x4 m_neuronScale;
    QVector3D m_worldSize,m_worldCenter;
    QGLXOctree<QVector3D> m_octree;
    int m_width, m_height;
    QMap<QString,QGLXCamera *> m_cameras;
    QVector2D m_mousePosition;
    QRect m_selectionRect;
    int m_neuronsToCreate,m_connectionsToCreate;
    float m_moveSpeed, m_turnSpeed;
    bool m_leftMouseDown,m_rightMouseDown,m_shiftDown,m_renderNeurons,m_renderConnections,m_newVisualizationParameters,m_versionCapable,m_initialized;

    QGLXBuffer m_selectionRectVertices,m_screenVertices,m_screenCoords;
    QSet<GLuint> m_selectedObjects,m_selectedConnections,m_selectedNeurons;
    QVector<Range> m_ranges;
    int m_dividerIndex;
    bool m_lightingEnabled;


};

#endif // NCV_H
