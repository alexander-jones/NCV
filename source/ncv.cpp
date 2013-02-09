#include "ncv.h"
#include <QTimer>
#include <QDir>
#include <QRect>
#include "time.h"
#include <QMessageBox>

#include "qcustomplot.h"


NCV::NCV(  const QGLFormat& format, QWidget* parent )
    : QGLWidget( format,parent )
{
    m_versionCapable = QGLFormat::openGLVersionFlags().testFlag(QGLFormat::OpenGL_Version_4_0);

    if (!m_versionCapable)
    {
        qDebug() << "Version Incapable";
        return;
    }

    this->setAutoBufferSwap(false);
    this->setAutoFillBackground(false);
    m_leftMouseDown = false;
    m_rightMouseDown = false;
    m_moveSpeed = 2000;
    m_turnSpeed = 0.0025f;
    m_shiftDown = false;
    m_worldSize = QVector3D(5000,5000,5000);
    m_renderOnlySelection = false;
    m_neuronsToCreate = 0;
    m_connectionsToCreate = 0;
    m_renderNeurons= true;
    m_renderConnections= true;
    m_initialized = false;
    m_neuronIDs = nullptr;
    m_connectionIDs = nullptr;
    m_connectionNeuronIDs = nullptr;
    m_connectionAttribToRender = NULL;
    m_neuronAttribToRender = NULL;
    srand(time(NULL));


}


NCV::~NCV()
{
}



bool NCV::m_compileShaderProgram(QGLShaderProgram * program,  QString vertexShaderPath, QString fragmentShaderPath )
{

    // First we load and compile the vertex shader->..
    bool success = program->addShaderFromSourceFile( QGLShader::Vertex, vertexShaderPath );
    if ( !success )
    {
        qDebug() << program->log();
        return false;
    }

    // ...now the fragment shader->..
    success = program->addShaderFromSourceFile( QGLShader::Fragment, fragmentShaderPath );
    if ( !success )
    {
        qDebug() << program->log();
        return false;
    }


    // ...and finally we link them to resolve any references.
    success = program->link();
    if ( !success )
    {
        qDebug() << "Could not link shader program:" << program->log();
        return false;
    }
    return true;
}

bool NCV::m_compileShaderProgram(QGLShaderProgram * program, QString vertexShaderPath, QString geometryShaderPath, QString fragmentShaderPath )
{
    // First we load and compile the vertex shader->..
    bool success = program->addShaderFromSourceFile( QGLShader::Vertex, vertexShaderPath );
    if ( !success )
    {
        qDebug() << program->log();
        return false;
    }

    // ...now the geometry shader->..
    success = program->addShaderFromSourceFile( QGLShader::Geometry, geometryShaderPath );
    if ( !success )
    {
        qDebug() << program->log();
        return false;
    }

    // ...now the fragment shader->.. what we need done
    success = program->addShaderFromSourceFile( QGLShader::Fragment, fragmentShaderPath );
    if ( !success )
    {
        qDebug() << program->log();
        return false;
    }


    // ...and finally we link them to resolve any references.
    success = program->link();
    if ( !success )
    {
        qDebug() << "Could not link shader program:" << program->log();
        return false;
    }
    return true;
}



void NCV::m_establishWorldBounds(int numNeurons,QVector3D * neuronPositions)
{

    QVector3D lowBound = QVector3D(100000000,100000000,100000000);
    QVector3D highBound = QVector3D(-100000000,-100000000,-100000000);

    // loop through all positions, establish lowest/ highest xyz coords
    for (int i = 0 ; i < numNeurons; i++)
    {
        if (neuronPositions[i].x() < lowBound.x())
            lowBound.setX(neuronPositions[i].x());
        else if (neuronPositions[i].x() > highBound.x())
            highBound.setX(neuronPositions[i].x());

        if (neuronPositions[i].y() < lowBound.y())
            lowBound.setY(neuronPositions[i].y());

        else if (neuronPositions[i].y() > highBound.y())
            highBound.setY(neuronPositions[i].y());

        if (neuronPositions[i].z() < lowBound.z())
            lowBound.setZ(neuronPositions[i].z());

        else if (neuronPositions[i].z() > highBound.z())
            highBound.setZ(neuronPositions[i].z());

    }
    m_worldSize = QVector3D(highBound.x() - lowBound.x(),highBound.y() - lowBound.y(),highBound.z() - lowBound.z());
    m_worldCenter = QVector3D(lowBound.x()+ m_worldSize.x()/2, lowBound.y() + m_worldSize.y()/2,lowBound.z()+ m_worldSize.z()/2);
    m_moveSpeed = m_worldSize.x()/50;
}


void NCV::initializeGL()
{

    if (!isValid())
    {
        invalidGraphicsConfigurationDetected();
        return;
    }
    glewInit();

    QGLFormat glFormat = this->format();
    if ( !glFormat.sampleBuffers() )
        qWarning() << "Could not enable sample buffers";

    // Set the clear color to black
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glLineWidth(4);
    glDepthFunc(GL_LEQUAL);

    m_camera = new QGLXCamera();
    m_camera->setView(QVector3D(-m_worldSize.x(),m_worldSize.y(),-m_worldSize.z()), QVector3D(0,0,1),QVector3D(0,1,0));
    m_camera->setProjection(35.0, 4.0/3.0, 0.1f, m_worldSize.length()*2.0f );


    // initialize target bufferm_neuronsToCreate
    m_frameBufferObject.create();

    QString names[4] = {"diffuse","normal","id","depth"};
    GLenum formats[4] = {GL_RGB8,GL_RGB8,GL_R32UI,GL_DEPTH_COMPONENT32};
    GLuint samples[4] = {1,1,1,1};
    GLint maxSamples;
    glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);

    for(int i = 0 ; i < 4; i ++)
    {
        m_maps.insert(names[i],QGLXTexture2D());
        m_maps[names[i]].create();
        m_maps[names[i]].bind();
        m_maps[names[i]].allocate(1000,1000,formats[i],samples[i]);
        m_maps[names[i]].setMinFilter(QGLXTexture2D::Nearest);
        m_maps[names[i]].setMagFilter(QGLXTexture2D::Nearest);
        m_maps[names[i]].setWrapFunction(QGLXTexture2D::Clamp,QGLXTexture2D::Clamp);
        m_maps[names[i]].release();
    }

    // compile shader
    m_flagNeuronProgram = new QGLShaderProgram(this->context(),this);
    if (!m_compileShaderProgram(m_flagNeuronProgram,":/shaders/neuronBit.vert",":/shaders/bit.frag"))
        return;
    if ( !m_flagNeuronProgram->bind() )
    {
        qDebug() << "Could not bind shader program to context";
        return;
    }
    m_flagNeuronProgram->release();

    //compile shader
    m_rangeNeuronProgram = new QGLShaderProgram(this->context(),this);
    if (!m_compileShaderProgram(m_rangeNeuronProgram,":/shaders/neuronFloat.vert",":/shaders/float.frag"))
        return;
    if ( !m_rangeNeuronProgram->bind() )
    {
        qDebug() << "Could not bind shader program to context";
        return;
    }
    m_rangeNeuronProgram->release();

    //compile shader
    m_flagConnectionProgram = new QGLShaderProgram(this->context(),this);
    if (!m_compileShaderProgram(m_flagConnectionProgram,":/shaders/connectionBit.vert",":/shaders/connectionBit.geom",":/shaders/bit.frag"))
        return;
    if ( !m_flagConnectionProgram->bind() )
    {
        qDebug() << "Could not bind shader program to context";
        return;
    }
    m_flagConnectionProgram->release();


    //compile shader
    m_rangeConnectionProgram = new QGLShaderProgram(this->context(),this);
    if (!m_compileShaderProgram(m_rangeConnectionProgram,":/shaders/connectionFloat.vert",":/shaders/connectionFloat.geom",":/shaders/float.frag"))
        return;
    if ( !m_rangeConnectionProgram->bind() )
    {
        qDebug() << "Could not bind shader program to context";
        return;
    }
    m_rangeConnectionProgram->release();


    //compile shader
    m_silhouetteNeuronProgram = new QGLShaderProgram(this->context(),this);
    if (!m_compileShaderProgram(m_silhouetteNeuronProgram,":/shaders/neuronSilhouette.vert",":/shaders/silhouette.frag"))
        return;
    if ( !m_silhouetteNeuronProgram->bind() )
    {
        qDebug() << "Could not bind shader program to context";
        return;
    }
    m_silhouetteNeuronProgram->release();

    m_silhouetteConnectionProgram = new QGLShaderProgram(this->context(),this);
    if (!m_compileShaderProgram(m_silhouetteConnectionProgram,":/shaders/connectionSilhouette.vert",":/shaders/connectionSilhouette.geom",":/shaders/silhouette.frag"))
        return;
    if ( !m_silhouetteConnectionProgram->bind() )
    {
        qDebug() << "Could not bind shader program to context";
        return;
    }
    m_silhouetteConnectionProgram->release();

    // establish initial values for neuron shader uniforms
    m_neuronScale = QMatrix4x4();
    m_neuronScale.scale((m_worldSize.length() / m_neuronsToCreate) * 40.0f );


    QVector3D screenVerts[4] = {QVector3D(-1,-1,0.5),QVector3D(1,-1,0.5),QVector3D(1,1,0.5),QVector3D(-1,1,0.5)};
    QVector2D screenCoords[4] = {QVector2D(0,0),QVector2D(1,0),QVector2D(1,1),QVector2D(0,1)};

    m_screenVertices.create();
    m_screenVertices.bind();
    m_screenVertices.setUsagePattern( QGLXBuffer::StaticDraw );
    m_screenVertices.allocate(&screenVerts[0],4 * sizeof(QVector3D),GL_FLOAT);
    m_screenVertices.release();

    m_screenCoords.create();
    m_screenCoords.bind();
    m_screenCoords.setUsagePattern( QGLXBuffer::StaticDraw );
    m_screenCoords.allocate(&screenCoords[0],4 * sizeof(QVector2D),GL_FLOAT);
    m_screenCoords.release();

    m_neuronRangeMap = new QGLXTexture1D();
    m_neuronRangeMap->create();
    m_neuronRangeMap->bind();
    m_neuronRangeMap->setMinFilter(QGLXTexture1D::Linear);
    m_neuronRangeMap->setMagFilter(QGLXTexture1D::Linear);
    m_neuronRangeMap->setWrapFunction(QGLXTexture1D::Clamp);
    m_neuronRangeMap->release();

    m_connectionRangeMap = new QGLXTexture1D();
    m_connectionRangeMap->create();
    m_connectionRangeMap->bind();
    m_connectionRangeMap->setMinFilter(QGLXTexture1D::Linear);
    m_connectionRangeMap->setMagFilter(QGLXTexture1D::Linear);
    m_connectionRangeMap->setWrapFunction(QGLXTexture1D::Clamp);
    m_connectionRangeMap->release();


    this->startTimer(1);
    m_initialized = true;
    initialized();
}

void NCV::timerEvent(QTimerEvent * e)
{

    if (!isValid())
        return;
    e->accept();
    // perform refresh
    paintGL();
    GLuint err = glGetError();
    if (err != 0)
        qDebug() <<err;

}




void NCV::resizeGL( int w, int h )
{
    if (!isValid())
        return;

    // Set the viewport to window dimensions
    m_width = w;
    m_height = h;
    glViewport( 0, 0, w, qMax( h, 1 ) );
    int targetWidth = m_width;
    int targetHeight = m_height;

    m_camera->setAspectRatio((float)m_width/m_height);

    for (QMap<QString,QGLXTexture2D>::iterator it = m_maps.begin();it != m_maps.end(); it++)
        it.value().setSize(targetWidth,targetHeight);
}

bool NCV::isValid()
{
    return m_versionCapable && QGLWidget::isValid();
}

bool NCV::isInitialized()
{
    return isValid() && m_initialized;
}


int NCV::connectionCount()
{

    return m_connections.numObjects();
}

int NCV::neuronCount()
{
    return m_neurons.numObjects();
}

void NCV::m_createNeurons()
{
    if (!isValid())
        return;


    QVector3D vertices[8] = {
        QVector3D(-0.57735, -0.57735, -0.57735) ,
        QVector3D(-0.57735, 0.57735, -0.57735) ,
        QVector3D(0.57735, 0.57735, -0.57735) ,
        QVector3D(0.57735, -0.57735, -0.57735),
        QVector3D(0.57735, -0.57735, 0.57735) ,
        QVector3D(-0.57735, -0.57735, 0.57735) ,
        QVector3D(-0.57735, 0.57735, 0.57735) ,
        QVector3D(0.57735, 0.57735, 0.57735) };


    QVector3D normals[8] = {
        QVector3D(-0.57735, -0.57735, -0.57735) ,
        QVector3D(-0.408248, 0.816497, -0.408248) ,
        QVector3D(0.408248, 0.408248, -0.816497) ,
        QVector3D(0.816497, -0.408248, -0.408248) ,
        QVector3D(0.408248, -0.816497, 0.408248) ,
        QVector3D(-0.408248, -0.408248, 0.816497),
        QVector3D(-0.816497, 0.408248, 0.408248) ,
        QVector3D(0.57735, 0.57735, 0.57735) };

    GLuint indices[36] = {0,1,2, 0,2,3, 0, 3,4, 0,4,5 ,0,5,6 ,0,6,1 ,7,4,3 ,7,3,2 ,7,2,1 , 7,1,6 ,7,6,5, 7,5,4};

    // create neuron system
    m_neurons.create(m_neuronsToCreate,8,36,1);
    m_neurons.setPrimitiveType(QGLXSystem::Triangle);

    // set per-vertex attributes of model
    m_neurons.setVertexAttributeIndices(&indices[0]);
    m_neurons.setVertexAttributeArray("Vert_Normal",normals,sizeof(QVector3D),GL_FLOAT);
    m_neurons.setVertexAttributeArray("Vert_Position",vertices,sizeof(QVector3D),GL_FLOAT);


    int startIndex = qMax(1,m_connections.numObjects());

    GLuint * sysIDs = new GLuint[m_neuronsToCreate];
    for (int i =0; i < m_neuronsToCreate; i ++)
        sysIDs[i] = startIndex +i;

    m_neurons.setInstanceAttributeArray("Inst_ID",sysIDs,sizeof(GLuint),GL_FLOAT,1);
    //m_neurons.setInstanceAttributeArray("Custom_ID",m_neuronIDs,sizeof(GLuint),GL_FLOAT,1);

    m_neuronsToCreate = 0;
    m_neuronIDs = nullptr;


}


void NCV::m_createConnections()
{

    if (!isValid())
        return;
    // create connection system
    m_connections.create(m_connectionsToCreate,2,0,0);
    m_connections.setPrimitiveType(QGLXSystem::Line);


    int startIndex = qMax(m_neuronsToCreate,m_neurons.numObjects());
    if (startIndex == 0)
        startIndex ++;
    GLuint * sysIDs = new GLuint[m_connectionsToCreate * 2];
    for (int i =0; i < m_connectionsToCreate * 2; i ++)
        sysIDs[i] = startIndex  + 1+ (i/2);

    m_connections.setVertexAttributeArray("Inst_ID",sysIDs,sizeof(GLuint),GL_FLOAT);
    //m_connections.setVertexAttributeArray("Custom_ID",m_connectionIDs,sizeof(GLuint),GL_FLOAT);
    m_connections.setVertexAttributeArray("Neuron_ID",m_connectionNeuronIDs,sizeof(GLuint),GL_FLOAT);

    m_connectionsToCreate = 0;
    m_connectionIDs = nullptr;
    m_connectionNeuronIDs = nullptr;
}

void NCV::m_bindAttribute(NetworkAttribute  * attribute)
{
    if (!attribute->buffer->isCreated())
        attribute->buffer->create();

    attribute->buffer->bind();

    if (attribute->unboundBufferData != NULL)
    {
        int numNeurons = m_neurons.numObjects();
        int numConnections = m_connections.numObjects();
        if (attribute->type == NetworkAttribute::Position)
        {
            GLuint componentSize = QGLXTexture::getComponentSize(GL_FLOAT);
            GLenum textureFormat  = QGLXTexture::bufferFormatToTextureFormat(GL_FLOAT,3,componentSize);

            attribute->buffer->allocate(attribute->unboundBufferData,3* componentSize * numNeurons,textureFormat);
            delete []( GLfloat *)attribute->unboundBufferData;

        }
        else if (attribute->type == NetworkAttribute::Range)
        {
            GLuint componentSize = QGLXTexture::getComponentSize(GL_FLOAT);
            GLenum textureFormat  = QGLXTexture::bufferFormatToTextureFormat(GL_FLOAT,1,componentSize);

            if (attribute->owner == NetworkAttribute::Neuron)
                attribute->buffer->allocate(attribute->unboundBufferData, componentSize * numNeurons,textureFormat);
            else
                attribute->buffer->allocate(attribute->unboundBufferData,componentSize * numConnections,textureFormat);
            delete []( GLfloat *)attribute->unboundBufferData;
        }
        else if (attribute->type == NetworkAttribute::Flag)
        {
            GLuint componentSize = QGLXTexture::getComponentSize(GL_UNSIGNED_INT);
            GLenum textureFormat  = QGLXTexture::bufferFormatToTextureFormat(GL_UNSIGNED_INT,1,componentSize);

            if (attribute->owner == NetworkAttribute::Neuron)
            {
                if (numNeurons %32 == 0)
                    attribute->buffer->allocate(attribute->unboundBufferData, componentSize * numNeurons/32,textureFormat);
                else
                    attribute->buffer->allocate(attribute->unboundBufferData, componentSize * (numNeurons/32 + 1),textureFormat);
            }
            else
            {
                if (numConnections %32 == 0)
                    attribute->buffer->allocate(attribute->unboundBufferData, componentSize * numConnections/32,textureFormat);
                else
                    attribute->buffer->allocate(attribute->unboundBufferData, componentSize * (numConnections/32 + 1),textureFormat);
            }
            delete [] ( GLuint *)attribute->unboundBufferData;
        }
        attribute->unboundBufferData = NULL;
    }
}
QGLShaderProgram * NCV::m_neuronProgram()
{
    if ( m_neuronAttribToRender != NULL)
    {

        if (m_neuronAttribToRender->type == NetworkAttribute::Flag)
            return m_flagNeuronProgram;
        else if (m_neuronAttribToRender->type == NetworkAttribute::Range)
            return m_rangeNeuronProgram;

    }

    return NULL;
}

QGLShaderProgram * NCV::m_connectionProgram()
{
    if ( m_connectionAttribToRender != NULL)
    {

        if (m_connectionAttribToRender->type == NetworkAttribute::Flag)
            return m_flagConnectionProgram;
        else if (m_connectionAttribToRender->type == NetworkAttribute::Range)
            return m_rangeConnectionProgram;

    }

    return NULL;

}


void NCV::m_bindConnections(QGLShaderProgram * program)
{

    if ( m_connectionAttribToRender != NULL )
    {
        program->bind();


        m_connections.bind(program);

        int textureSlot = 0;

        glActiveTexture(GL_TEXTURE0 + textureSlot);
        m_bindAttribute(&m_translationBuffer);

        program->setUniformValue("Inst_Translation", textureSlot);

        textureSlot++;

        program->setUniformValue("WVP",m_camera->projection() * m_camera->view());
        program->setUniformValue("ConnectionWidth",(float)(m_worldSize.length()/m_neurons.numObjects()) * 5.0f );
        program->setUniformValue("CameraPosition",m_camera->position());

        if (program == m_flagConnectionProgram ||program == m_rangeConnectionProgram )
        {
            QString name = m_connectionAttribToRender->name;
            if (m_neuronAttributes.contains(name)  &&m_neuronAttributes[name].unboundBufferData == NULL && m_connectionAttribToRender->owner == m_neuronAttributes[name].owner)
                m_connectionAttribToRender->unboundBufferData = NULL;

            glActiveTexture(GL_TEXTURE0 + textureSlot);
            m_bindAttribute(m_connectionAttribToRender);


            program->setUniformValue("ScreenSize",QVector2D(m_width,m_height));

            program->setUniformValue("ConnectionIDStart",m_neurons.numObjects());
            program->setUniformValue("FogEnd",m_camera->farPlane());
            program->setUniformValue("FogStart",m_camera->nearPlane());
            program->setUniformValue("WorldCenter",m_worldCenter);
            program->setUniformValue("WorldSize",m_worldSize);
            program->setUniformValue("DeselectionColor",QVector4D(1,1,1,0.05));

            int attribLoc = program->uniformLocation("Inst_Attribute");
            program->setUniformValue(attribLoc, textureSlot);

            if (m_connectionAttribToRender->type == NetworkAttribute::Flag)
            {
                program->setUniformValue("OnColor", m_connectionAttribToRender->onColor);
                program->setUniformValue("OffColor", m_connectionAttribToRender->offColor);
            }
            else if (m_connectionAttribToRender->type == NetworkAttribute::Range)
            {
                program->setUniformValue("MinimumValue", m_connectionAttribToRender->minValue);
                program->setUniformValue("MaximumValue", m_connectionAttribToRender->maxValue);

                glActiveTexture(GL_TEXTURE2);
                m_connectionRangeMap->bind();
                program->setUniformValue("RangeMap",2);

            }
        }
        else if (program == m_silhouetteConnectionProgram)
        {

            program->setUniformValue("SilhouetteColor",QVector3D(0,0,0));
            program->setUniformValue("ZFar",m_camera->farPlane());
            program->setUniformValue("ZNear",m_camera->nearPlane());
            program->setUniformValue("ZStop",m_camera->farPlane()/10);
        }
    }
}

void NCV::m_releaseConnections(QGLShaderProgram * program)
{
    m_connectionRangeMap->release();
    m_translationBuffer.buffer->release();
    m_connectionAttribToRender->buffer->release();
    m_connections.release(program);
    program->release();
}

void NCV::m_bindNeurons(QGLShaderProgram * program)
{


    if (m_neuronAttribToRender != NULL)
    {

        program->bind();

        int textureSlot = 0;

        // bind neuron position buffer to first texture slot
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        m_bindAttribute(&m_translationBuffer);

        // bind position buffer to current neuron shader program
        program->setUniformValue("Inst_Translation", textureSlot);

        textureSlot++;


        program->setUniformValue("WVP",m_camera->projection() * m_camera->view());
        program->setUniformValue("Scale",m_neuronScale);
        program->setUniformValue("CameraPosition",m_camera->position());


        // bind geometric data associated with neurons
        m_neurons.bind(program);

        if (program == m_flagNeuronProgram ||program == m_rangeNeuronProgram )
        {
            QString name = m_neuronAttribToRender->name;
            if (m_connectionAttributes.contains(name)&&m_connectionAttributes[name].unboundBufferData == NULL && m_neuronAttribToRender->owner == m_connectionAttributes[name].owner)
                m_neuronAttribToRender->unboundBufferData = NULL;
            // bind current neuron attribute to next texture slot
            glActiveTexture(GL_TEXTURE0 + textureSlot);
            m_bindAttribute(m_neuronAttribToRender);

            program->setUniformValue("ScreenSize",QVector2D(m_width,m_height));
            program->setUniformValue("FogEnd",m_camera->farPlane());
            program->setUniformValue("FogStart",m_camera->nearPlane());
            program->setUniformValue("DeselectionColor",QVector4D(1,1,1,0.05));
            program->setUniformValue("WorldCenter",m_worldCenter);
            program->setUniformValue("WorldSize",m_worldSize);

            // bind attribute to current shader program
            int attribLoc = program->uniformLocation("Inst_Attribute");
            program->setUniformValue(attribLoc, textureSlot);

            // depending on which type of attribute the neuron attribute is, bind related uniforms to shader program
            if (m_neuronAttribToRender->type == NetworkAttribute::Flag)
            {
                program->setUniformValue("OnColor", m_neuronAttribToRender->onColor);
                program->setUniformValue("OffColor", m_neuronAttribToRender->offColor);
            }
            else if (m_neuronAttribToRender->type  == NetworkAttribute::Range)
            {
                program->setUniformValue("MinimumValue", m_neuronAttribToRender->minValue);
                program->setUniformValue("MaximumValue", m_neuronAttribToRender->maxValue);
                glActiveTexture(GL_TEXTURE2);
                m_neuronRangeMap->bind();
                program->setUniformValue("RangeMap",2);
            }

        }
        else if (program == m_silhouetteNeuronProgram)
        {

            program->setUniformValue("SilhouetteColor",QVector3D(0,0,0));
            program->setUniformValue("ZFar",m_camera->farPlane());
            program->setUniformValue("ZNear",m_camera->nearPlane());
            program->setUniformValue("ZStop",m_camera->farPlane()/5);
        }

    }

}

void NCV::m_releaseNeurons(QGLShaderProgram * program)
{

    m_neuronRangeMap->release();
    m_translationBuffer.buffer->release();
    m_neuronAttribToRender->buffer->release();
    m_neurons.release(program);
    program->release();
}
void NCV::m_performRegularRender()
{

    // bind fbo and related targets
    m_frameBufferObject.bind();
    glEnable(GL_MULTISAMPLE);
    m_frameBufferObject.enableColorAttachments(3);
    m_maps["diffuse"].bind(QGLXTexture2D::Draw,QGLXTexture2D::Color0);
    m_maps["normal"].bind(QGLXTexture2D::Draw,QGLXTexture2D::Color1);
    m_maps["id"].bind(QGLXTexture2D::Draw,QGLXTexture2D::Color2);
    m_maps["depth"].bind(QGLXTexture2D::Draw,QGLXTexture2D::Depth);

    // clear targets
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


    if (m_translationBuffer.buffer != NULL)
    {

        if (m_connections.numObjects() > 0 && m_renderConnections && m_connectionAttribToRender != NULL)
        {
            m_bindConnections(m_connectionProgram());
            m_connections.draw();
            m_releaseConnections(m_connectionProgram());
        }

        // render neurons
        if (m_neurons.numObjects() > 0 &&  m_renderNeurons && m_neuronAttribToRender != NULL)
        {
            m_bindNeurons(m_neuronProgram());
            m_neurons.draw();
            m_releaseNeurons(m_neuronProgram());
        }

    }


    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(1,1.5);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0001f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    if (m_translationBuffer.buffer != NULL)
    {

        if (m_connections.numObjects() > 0 && m_renderConnections && m_connectionAttribToRender != NULL)
        {
            m_bindConnections(m_silhouetteConnectionProgram);
            m_connections.draw();
            m_releaseConnections(m_silhouetteConnectionProgram);
        }

        // render neurons
        if (m_neurons.numObjects() > 0 &&  m_renderNeurons && m_neuronAttribToRender != NULL)
        {
            m_bindNeurons(m_silhouetteNeuronProgram);
            m_neurons.draw();
            m_releaseNeurons(m_silhouetteNeuronProgram);
        }

    }


    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    // release frame buffer and related targets
    m_maps["diffuse"].release();
    m_maps["normal"].release();
    m_maps["id"].release();
    m_maps["depth"].release();
    m_frameBufferObject.enableColorAttachments(0);
    glDisable(GL_MULTISAMPLE);
    m_frameBufferObject.release();
}

void NCV::m_performSelectionRender()
{
    m_performRegularRender();

    // bind frame buffer and all first pass targets
    m_frameBufferObject.bind();
    m_frameBufferObject.enableColorAttachments(3);
    m_maps["diffuse"].bind(QGLXTexture2D::Draw,QGLXTexture2D::Color0);
    m_maps["normal"].bind(QGLXTexture2D::Draw,QGLXTexture2D::Color1);
    m_maps["id"].bind(QGLXTexture2D::Draw,QGLXTexture2D::Color2);
    m_maps["depth"].bind(QGLXTexture2D::Draw,QGLXTexture2D::Depth);

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


    if (m_renderNeurons && m_neuronAttribToRender != NULL)
    {
        m_bindNeurons(m_neuronProgram());
        for (QVector<Range>::iterator it = m_ranges.begin(); it != m_ranges.end() ; it++)
        {
            Range range = *it;
            if (range.start > m_neurons.numObjects())
                break;
            else if (range.end > m_neurons.numObjects())
            {
                m_neurons.drawSubset(range.start-1,m_neurons.numObjects()-range.start);
                break;
            }
            else
                m_neurons.drawSubset(range.start-1,range.end-range.start);
        }
        m_releaseNeurons(m_neuronProgram());
    }
    if (m_renderConnections && m_connectionAttribToRender != NULL)
    {

        m_bindConnections(m_connectionProgram());
        for (QVector<Range>::iterator it = m_ranges.begin() ; it != m_ranges.end(); it++)
        {
            Range range = *it;

            if (range.end <= m_neurons.numObjects())
                continue;
            else if (range.start <= m_neurons.numObjects())
                m_connections.drawSubset(0,range.end -(m_neurons.numObjects() +1));
            else
                m_connections.drawSubset(range.start-1 - m_neurons.numObjects(),range.end-range.start);

        }

        m_releaseConnections(m_connectionProgram());

    }


    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(1,1.5);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0001f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);


    if (m_renderNeurons && m_neuronAttribToRender != NULL)
    {
        m_bindNeurons(m_silhouetteNeuronProgram);
        for (QVector<Range>::iterator it = m_ranges.begin(); it != m_ranges.end() ; it++)
        {
            Range range = *it;
            if (range.start > m_neurons.numObjects())
                break;
            else if (range.end > m_neurons.numObjects())
            {
                m_neurons.drawSubset(range.start-1,m_neurons.numObjects()-range.start);
                break;
            }
            else
                m_neurons.drawSubset(range.start-1,range.end-range.start);
        }
        m_releaseNeurons(m_silhouetteNeuronProgram);
    }
    if (m_renderConnections && m_connectionAttribToRender != NULL)
    {

        m_bindConnections(m_silhouetteConnectionProgram);
        for (QVector<Range>::iterator it = m_ranges.begin() ; it != m_ranges.end(); it++)
        {
            Range range = *it;

            if (range.end <= m_neurons.numObjects())
                continue;
            else if (range.start <= m_neurons.numObjects())
                m_connections.drawSubset(0,range.end -(m_neurons.numObjects() +1));
            else
                m_connections.drawSubset(range.start-1 - m_neurons.numObjects(),range.end-range.start);

        }

        m_releaseConnections(m_silhouetteConnectionProgram);

    }

    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    if (!m_renderOnlySelection)
    {

        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0001f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

        if (m_renderNeurons  && m_neuronAttribToRender != NULL)
        {
            m_bindNeurons(m_neuronProgram());
            QGLShaderProgram * program = m_neuronProgram();
            program->setUniformValue("Deselected",1);
            int previousEnd = 1;
            for (QVector<Range>::iterator it = m_ranges.begin() ; it != m_ranges.end(); it++)
            {
                Range range = *it;
                if (range.start > m_neurons.numObjects())
                    break;
                else if (range.start > previousEnd)
                    m_neurons.drawSubset(previousEnd-1,range.start-previousEnd);
                previousEnd = range.end;
            }
            if (previousEnd < m_neurons.numObjects())
                m_neurons.drawSubset(previousEnd-1,m_neurons.numObjects()-previousEnd);
            program->setUniformValue("Deselected",0);
            m_releaseNeurons(m_neuronProgram());
        }
        if (m_renderConnections && m_connectionAttribToRender != NULL)
        {
            m_bindConnections(m_connectionProgram());
            QGLShaderProgram * program = m_connectionProgram();
            program->setUniformValue("Deselected",1);
            int previousEnd = m_neurons.numObjects() +1;
            int endOfItems = m_neurons.numObjects() + m_connections.numObjects();
            for (QVector<Range>::iterator it = m_ranges.begin() ; it != m_ranges.end(); it++)
            {
                Range range = *it;
                if (range.end <= m_neurons.numObjects())
                    continue;
                else if (range.start > previousEnd)
                    m_connections.drawSubset(previousEnd-1- m_neurons.numObjects(),range.start-previousEnd);
                previousEnd = range.end;
            }
            if (previousEnd < endOfItems)
                m_connections.drawSubset(previousEnd-1- m_neurons.numObjects(),endOfItems-previousEnd);

            program->setUniformValue("Deselected",0);

            m_releaseConnections(m_connectionProgram());

            glDisable(GL_ALPHA_TEST);
            glDisable(GL_BLEND);
        }
        m_maps["diffuse"].release();


        m_maps["normal"].release();
        m_maps["id"].release();
        m_maps["depth"].release();
        m_translationBuffer.buffer->release();
        m_neuronAttribToRender->buffer->release();
        m_connectionAttribToRender->buffer->release();

        m_frameBufferObject.release();
    }




}

void NCV::paintGL()
{

    if (!isValid())
        return;


    // check to see if any changes were requested externally

    if( m_neuronsToCreate > 0)
        m_createNeurons();
    if( m_connectionsToCreate > 0)
        m_createConnections();


    if (m_neuronAttribToRender->unboundTextureData  !=NULL)
    {
        m_neuronRangeMap->bind();
        m_neuronRangeMap->allocate(m_neuronAttribToRender->unboundTextureWidth,GL_RGB32F,m_neuronAttribToRender->unboundTextureData);
        m_neuronRangeMap->release();
        delete [] (QVector3D *)m_neuronAttribToRender->unboundTextureData;
        m_neuronAttribToRender->unboundTextureData = NULL;
    }

    if (m_connectionAttribToRender->unboundTextureData  !=NULL)
    {
        m_connectionRangeMap->bind();
        m_connectionRangeMap->allocate(m_connectionAttribToRender->unboundTextureWidth,GL_RGB32F,m_connectionAttribToRender->unboundTextureData);
        m_connectionRangeMap->release();
        delete [] (QVector3D *)m_connectionAttribToRender->unboundTextureData;
        m_connectionAttribToRender->unboundTextureData = NULL;
    }

    if (m_selectedObjects.count() >0)
        m_performSelectionRender();
    else
        m_performRegularRender();


    m_frameBufferObject.blitTexture(m_maps["diffuse"],QGLXTexture::Color0,QRect(0,0,m_width,m_height),QRect(0,0,m_width,m_height));

    glDisable(GL_MULTISAMPLE);
    glDisable(GL_DITHER);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_POLYGON_SMOOTH);
    glFlush();
    glFinish();
    m_painter.begin(this);
    QPen pen = m_painter.pen();
    pen.setColor(QColor(255,255,255));
    pen.setStyle(Qt::DotLine);
    m_painter.setPen(pen);
    QBrush brush = m_painter.brush();
    brush.setColor(QColor(125,125,125));
    brush.setStyle(Qt::Dense4Pattern);
    m_painter.setBrush(brush);
    if (m_leftMouseDown)
        m_painter.drawRect(m_selectionRect);

    m_painter.end();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DITHER);
    glEnable(GL_CULL_FACE);
    glEnable(GL_POLYGON_SMOOTH);
    glFlush();
    glFinish();
    this->swapBuffers();
    frameRendered();

}




void NCV::leaveEvent(QEvent * )
{
    if (!isValid())
        return;
    this->parentWidget()->setFocus();
}

void NCV::enterEvent(QEvent * )
{
    if (!isValid())
        return;
    setFocus();
}
void NCV::mouseMoveEvent(QMouseEvent* e)
{
    if (!isValid())
        return;
    QVector2D newPos = QVector2D(e->x(),e->y());
    // if left mouse button down, expand selection rectangle
    if (m_leftMouseDown)
    {
        int newWidth = e->x() - m_selectionRect.x();
        int newHeight = e->y() - m_selectionRect.y();
        if (newWidth != 0)
            m_selectionRect.setWidth(newWidth);
        else
            m_selectionRect.setWidth(1);

        if (newHeight != 0)
            m_selectionRect.setHeight(newHeight);
        else
            m_selectionRect.setHeight(1);
    }
    // if right mouse button down, rotate camera
    else if (m_rightMouseDown)
    {
        m_camera->rotate((float)(newPos.x() - m_mousePosition.x()) * -m_turnSpeed,(float)(newPos.y() - m_mousePosition.y()) * -m_turnSpeed);
        m_mousePosition = newPos;
        cameraUpdated(m_camera);
    }

}
void NCV::wheelEvent(QWheelEvent *e)
{
    if (!isValid())
        return;

    // scale neuron
    if(e->orientation() == Qt::Vertical)
    {

        m_neuronScale.scale((2000.0 - (double)e->delta()) / 2000.0);

    }
}
void NCV::mousePressEvent(QMouseEvent* e)
{
    m_mousePosition = QVector2D(e->x(),e->y());
    // if left button pressed, start building selection rect.
    if (e->button() == Qt::LeftButton)
    {
        m_leftMouseDown = true;
        m_selectionRect = QRect(e->x(),e->y(),1,1);
    }


    else if (e->button() == Qt::RightButton)
        m_rightMouseDown = true;

}

void  NCV::mouseReleaseEvent(QMouseEvent* e)
{

    if (!isValid())
        return;
    if (e->button() == Qt::LeftButton)
    {
        // correct selection rectanglesc
        int width = abs(m_selectionRect.width());
        int height = abs(m_selectionRect.height());

        m_selectionRect.setLeft(qMax(0,qMin(m_selectionRect.left(),m_selectionRect.right())));
        m_selectionRect.setTop(qMax(0,qMin(m_selectionRect.top(),m_selectionRect.bottom())));

        if (m_selectionRect.bottom() > m_height)
            m_selectionRect.setHeight(m_height - m_selectionRect.top());
        else
            m_selectionRect.setHeight(height);

        if (m_selectionRect.right() > m_width)
            m_selectionRect.setWidth(m_width - m_selectionRect.left());
        else
            m_selectionRect.setWidth(width);


        // get pixels of ID map in area of selection window
        GLuint * objectID;
        objectID = new GLuint[ m_selectionRect.width() *  m_selectionRect.height()];
        m_frameBufferObject.getTextureData(m_maps["id"],m_selectionRect,objectID);

        // if not performing compound selection, deselect previously selected items
        if (!m_shiftDown )
            m_selectedObjects.clear();

        // compile new selections into a unique set
        for (int i =0; i < m_selectionRect.width() *  m_selectionRect.height(); i ++)
        {
            if (objectID[i] != 0 )
            {
                GLuint id = objectID[i];
                m_selectedObjects.insert(id);
            }
        }


        m_ranges.clear();
        if (m_selectedObjects.count() > 0)
        {
            QList<GLuint> list = m_selectedObjects.toList();
            qSort(list);
            Range r = Range();
            r.start = list[0];
            r.end = list[0]+1;
            m_ranges.push_back(r);
            int currentRange = 0;
            for (QList<GLuint>::iterator it = list.begin() +1; it != list.end(); it++)
            {
                int id = *it;
                if (m_ranges[currentRange].end == id)
                    m_ranges[currentRange].end = id+1;
                else
                {
                    Range r = Range();
                    r.start = id;
                    r.end = id+1;
                    m_ranges.push_back(r);
                    currentRange ++;
                }

            }


        }


        m_leftMouseDown = false;
    }
    else if (e->button() == Qt::RightButton)
    {
        m_rightMouseDown = false;
    }
}


void NCV::keyPressEvent( QKeyEvent* e )
{

    if (!isValid())
        return;
    switch ( e->key() )
    {
        case Qt::Key_Escape:
            QCoreApplication::instance()->quit();
            break;

        case Qt::Key_A:
            m_camera->pan(-m_moveSpeed,0.0f,0.0f);
            cameraUpdated(m_camera);
            break;

        case Qt::Key_D:
            m_camera->pan(m_moveSpeed,0.0f,0.0f);
            cameraUpdated(m_camera);
            break;


        case Qt::Key_R:
            m_camera->pan(0.0f,0.0f,m_moveSpeed/ 25.0f);
            cameraUpdated(m_camera);
            break;


        case Qt::Key_N:
            m_renderNeurons= !m_renderNeurons;
            break;

        case Qt::Key_C:
            m_renderConnections= !m_renderConnections;
            break;

        case Qt::Key_Shift:
            m_shiftDown = true;
            break;
        case Qt::Key_T:
            m_camera->pan(0.0f,0.0f,m_moveSpeed/ 25.0f);
            cameraUpdated(m_camera);
            break;


        case Qt::Key_P:
            m_camera->pan(0.0f,0.0f,m_moveSpeed/ 25.0f);
            cameraUpdated(m_camera);
            break;

        case Qt::Key_V:
            m_camera->pan(0.0f,0.0f,-m_moveSpeed/ 10.0f);
            cameraUpdated(m_camera);
            break;

        case Qt::Key_W:
            m_camera->pan(0.0f,0.0f,m_moveSpeed);
            cameraUpdated(m_camera);
            break;

        case Qt::Key_S:
            m_camera->pan(0.0f,0.0f,-m_moveSpeed);
            cameraUpdated(m_camera);
            break;

        case Qt::Key_Q:
            m_camera->pan(0.0f,m_moveSpeed,0.0f);
            cameraUpdated(m_camera);
            break;

        case Qt::Key_E:
            m_camera->pan(0.0f,-m_moveSpeed,0.0f);
            cameraUpdated(m_camera);
            break;

        case Qt::Key_B:
            m_renderOnlySelection = !m_renderOnlySelection;
            break;




        case Qt::Key_K:
            //m_octree.setLevels(m_octree.levels() -1);
            break;
        default:
            QGLWidget::keyPressEvent( e );
    }
}
void NCV::keyReleaseEvent(QKeyEvent *e)
{
    if (!isValid())
        return;
    switch (e->key())
    {
    case Qt::Key_Shift:
        m_shiftDown = false;
        break;
    }

}



void NCV::createNeurons(int number,QVector3D * positions,GLuint * ids  )
{
    if (!isValid())
        return;
    m_neuronsToCreate = number;
    m_translationBuffer = NetworkAttribute();
    m_translationBuffer.buffer = new QGLXBuffer(QGLXBuffer::TextureBuffer);
    m_translationBuffer.owner = NetworkAttribute::Neuron;
    m_translationBuffer.type = NetworkAttribute::Position;
    m_translationBuffer.unboundBufferData = positions;

    m_neuronIDs = ids;

    m_establishWorldBounds(number,positions);

}
void NCV::createConnections(int number,GLuint * neruonIN,GLuint * neruonOUT,GLuint * ids  )
{
    if (!isValid())
        return;
    m_connectionsToCreate = number;
    m_connectionIDs = ids;

    m_connectionNeuronIDs = new GLuint[m_connectionsToCreate * 2];
    for (int i =0; i < m_connectionsToCreate * 2; i ++)
    {
        if(i %2 ==0)
            m_connectionNeuronIDs[i] = neruonIN[i/2];
        else
            m_connectionNeuronIDs[i] = neruonOUT[i/2];
    }
}
void NCV::createNeuronRangeAttribute(QString name,GLfloat minValue, GLfloat maxValue)
{

    m_neuronAttributes[name] = NetworkAttribute();
    m_neuronAttributes[name].buffer = new QGLXBuffer(QGLXBuffer::TextureBuffer);
    m_neuronAttributes[name].owner = NetworkAttribute::Neuron;
    m_neuronAttributes[name].type = NetworkAttribute::Range;
    m_neuronAttributes[name].name = name;
    m_neuronAttributes[name].unboundBufferData = NULL;
    m_neuronAttributes[name].unboundTextureData = NULL;
    m_neuronAttributes[name].minValue = minValue;
    m_neuronAttributes[name].maxValue = maxValue;

    newNeuronRangeAttribute(name,minValue,maxValue);
}
void NCV::createConnectionRangeAttribute(QString name,GLfloat minValue, GLfloat maxValue)
{
    m_connectionAttributes[name] = NetworkAttribute();
    m_connectionAttributes[name].buffer =new QGLXBuffer(QGLXBuffer::TextureBuffer);
    m_connectionAttributes[name].owner = NetworkAttribute::Connection;
    m_connectionAttributes[name].type = NetworkAttribute::Range;
    m_connectionAttributes[name].name = name;
    m_connectionAttributes[name].unboundBufferData = NULL;
    m_connectionAttributes[name].unboundTextureData = NULL;
    m_connectionAttributes[name].minValue = minValue;
    m_connectionAttributes[name].maxValue = maxValue;

    newConnectionRangeAttribute(name,minValue,maxValue);
}


void NCV::createNeuronFlagAttribute(QString name, QVector3D onColor,QVector3D offColor)
{

    if (!isValid())
        return;

    m_neuronAttributes[name] = NetworkAttribute();
    m_neuronAttributes[name].buffer = new QGLXBuffer(QGLXBuffer::TextureBuffer);
    m_neuronAttributes[name].owner = NetworkAttribute::Neuron;
    m_neuronAttributes[name].type = NetworkAttribute::Flag;
    m_neuronAttributes[name].name = name;
    m_neuronAttributes[name].unboundBufferData = NULL;
    m_neuronAttributes[name].unboundTextureData = NULL;
    m_neuronAttributes[name].onColor = onColor;
    m_neuronAttributes[name].offColor = offColor;

    newNeuronBitAttribute(name,QColor(offColor.x() * 255,offColor.y() * 255,offColor.z() * 255),QColor(onColor.x() * 255,onColor.y() * 255,onColor.z() * 255));

}

void NCV::createConnectionFlagAttribute(QString name, QVector3D onColor,QVector3D offColor)
{

    if (!isValid())
        return;

    m_connectionAttributes[name] = NetworkAttribute();
    m_connectionAttributes[name].buffer = new QGLXBuffer(QGLXBuffer::TextureBuffer);
    m_connectionAttributes[name].owner = NetworkAttribute::Connection;
    m_connectionAttributes[name].type = NetworkAttribute::Flag;
    m_connectionAttributes[name].name = name;
    m_connectionAttributes[name].unboundBufferData = NULL;
    m_connectionAttributes[name].unboundTextureData = NULL;
    m_connectionAttributes[name].onColor = onColor;
    m_connectionAttributes[name].offColor = offColor;

    newConnectionBitAttribute(name,QColor(offColor.x() * 255,offColor.y() * 255,offColor.z() * 255),QColor(onColor.x() * 255,onColor.y() * 255,onColor.z() * 255));

}

void NCV::setNeuronRangeAttribute(QString name, GLfloat * data)
{
    if (!isValid())
        return;

    if (m_neuronAttributes.contains(name))
    {
        m_neuronAttributes[name].unboundBufferData = data;
        m_connectionAttributes[name].unboundBufferData = data;
    }

}

void NCV::setNeuronFlagAttribute(QString name, GLuint * data)
{

    if (!isValid())
        return;

    if (m_neuronAttributes.contains(name))
    {
        m_neuronAttributes[name].unboundBufferData = data;
        m_connectionAttributes[name].unboundBufferData = data;
    }


}

void NCV::setConnectionRangeAttribute(QString name, GLfloat * data)
{

    if (!isValid())
        return;
    if (m_connectionAttributes.contains(name))
        m_connectionAttributes[name].unboundBufferData = data;

}

void NCV::setConnectionFlagAttribute(QString name, GLuint * data)
{

    if (!isValid())
        return;

    if (m_connectionAttributes.contains(name))
        m_connectionAttributes[name].unboundBufferData = data;


}



void NCV::setNeuronAttributeToRender(QString name)
{
    if (m_neuronAttributes.contains(name))
    {
        m_neuronAttribToRender = &m_neuronAttributes[name];

    }

}

void NCV::setConnectionAttributeToRender(QString name)
{

    if (m_connectionAttributes.contains(name))
    {
        m_connectionAttribToRender = &m_connectionAttributes[name];
    }
}

void NCV::setNeuronRangeColoration(QString name,QRgb * data, int width)
{
    QVector3D * normalizedPixels = new QVector3D[width];
    for (int i = 0; i < width; i ++)
    {
        QColor pixel = QColor::fromRgb(data[i]);
        normalizedPixels[i] = QVector3D(pixel.redF(),pixel.greenF(),pixel.blueF());
    }
    m_neuronAttributes[name].unboundTextureData = normalizedPixels;
    m_neuronAttributes[name].unboundTextureWidth = width;
}

void NCV::setNeuronFlagColoration( QString name,QColor offColor,QColor onColor)
{
    if (m_neuronAttribToRender != NULL )
    {
        m_neuronAttributes[name].onColor = QVector3D(onColor.redF(),onColor.greenF(),onColor.blueF());
        m_neuronAttributes[name].offColor = QVector3D(offColor.redF(),offColor.greenF(),offColor.blueF());
    }
}

void NCV::setConnectionRangeColoration(QString name,QRgb * data, int width)
{
    QVector3D * normalizedPixels = new QVector3D[width];
    for (int i = 0; i < width; i ++)
    {
        QColor pixel = QColor::fromRgb(data[i]);
        normalizedPixels[i] = QVector3D(pixel.redF(),pixel.greenF(),pixel.blueF());
    }
    m_connectionAttributes[name].unboundTextureData = normalizedPixels;
    m_connectionAttributes[name].unboundTextureWidth = width;
}

void NCV::setConnectionFlagColoration(QString name,QColor offColor,QColor onColor)
{
    if (m_connectionAttribToRender != NULL )
    {
        m_connectionAttributes[name].onColor = QVector3D(onColor.redF(),onColor.greenF(),onColor.blueF());
        m_connectionAttributes[name].offColor = QVector3D(offColor.redF(),offColor.greenF(),offColor.blueF());
    }
}
