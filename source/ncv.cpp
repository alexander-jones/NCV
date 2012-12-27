#include "ncv.h"
#include <QTimer>
#include <QDir>
#include <QRect>
#include "time.h"
#include <QMessageBox>


NCV::NCV(  QWidget* parent )
    : QGLWidget( parent )
{
    m_versionCapable = QGLFormat::openGLVersionFlags().testFlag(QGLFormat::OpenGL_Version_4_0);

    if (!m_versionCapable)
        return;

    QGLFormat glFormat;
    glFormat.setVersion( 4,0 );
    glFormat.setDoubleBuffer(true);
    glFormat.setProfile( QGLFormat::CompatibilityProfile ); // Requires >=Qt-4.8.0
    glFormat.setSampleBuffers( true );
    this->setFormat(glFormat);
    m_leftMouseDown = false;
    m_rightMouseDown = false;
    m_frameBufferObject =  QGLXFrameBufferObject();
    m_moveSpeed = 2000;
    m_turnSpeed = 0.0025f;
    m_shiftDown = false;
    m_worldSize = QVector3D(5000,5000,5000);
    m_frameCount = 0;
    m_renderOnlySelection = false;
    m_neuronsToCreate = 0;
    m_connectionsToCreate = 0;
    m_renderNeurons= true;
    m_renderConnections= true;
    m_lightingEnabled = false;
    m_newVisualizationParameters = false;
    m_initialized = false;
    m_connectionAttribToRender = NULL;
    m_neuronAttribToRender = NULL;
    srand(time(NULL));

}


NCV::~NCV()
{
}

void NCV::addLight(QGLXLight * light, QString name)
{
    if (!isValid())
        return;
    m_lights[name] = light;
    newLight(light,name);
}

void NCV::deleteLight(QString name)
{
    m_lights.remove(name);
}

void NCV::switchCamera(QString name)
{
    if (!isValid())
        return;

    m_currentCamera = name;
    cameraUpdated(m_currentCamera);

    newCurrentCamera(name);

}
void NCV::addCamera(QGLXCamera * camera, QString name)
{
    if (!isValid())
        return;

    m_cameras[name] = camera;
    newCamera(camera,name);

}

void NCV::deleteCamera(QString name)
{
    if (!isValid())
        return;

    m_cameras.remove(name);
    m_cameras[m_currentCamera] = m_cameras.begin().value();

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

}


void NCV::initializeGL()
{

    if (!isValid())
    {
        invalidGraphicsConfigurationDetected();
        return;
    }
    glewInit();

    QGLFormat glFormat = QGLWidget::format();
    if ( !glFormat.sampleBuffers() )
        qWarning() << "Could not enable sample buffers";

    // Set the clear color to black
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_MULTISAMPLE);
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );
    glLineWidth(3);


    QGLXCamera *main = new QGLXCamera();
    main->setViewMatrix(QVector3D(0,0,0), QVector3D(0,0,1),QVector3D(0,1,0));
    main->setProjectionMatrix(55.0, 4.0/3.0, 0.1, 5000000.0);
    addCamera(main,"Camera 1");
    switchCamera("Camera 1");



    // initialize target bufferm_neuronsToCreate
    m_frameBufferObject.create();


    QString names[7] = {"diffuse","selected","deselected","position","normal","id","depth"};
    GLenum formats[7] = {GL_RGB8,GL_RGBA8,GL_RGBA8,GL_RGB32F,GL_RGB8,GL_R32UI,GL_DEPTH_COMPONENT32};
    for(int i = 0 ; i < 7; i ++)
    {
        m_maps.insert(names[i],QGLXTexture2D());
        m_maps[names[i]].create();
        m_maps[names[i]].bind();
        m_maps[names[i]].allocate(1000,1000,formats[i]);
        m_maps[names[i]].setMinFilter(QGLXTexture2D::Linear);
        m_maps[names[i]].setMagFilter(QGLXTexture2D::Linear);
        m_maps[names[i]].setWrapFunction(QGLXTexture2D::Clamp,QGLXTexture2D::Clamp);
        m_maps[names[i]].release();

    }

    // compile shader
    m_bitNeuronProgram = new QGLShaderProgram(this->context(),this);
    if (!QGLXCore::compileShaderProgram(m_bitNeuronProgram,":/shaders/neuronBit.vert",":/shaders/bit.frag"))
        return;
    if ( !m_bitNeuronProgram->bind() )
    {
        qDebug() << "Could not bind shader program to context";
        return;
    }
    m_bitNeuronProgram->release();

    //compile shader
    m_floatNeuronProgram = new QGLShaderProgram(this->context(),this);
    if (!QGLXCore::compileShaderProgram(m_floatNeuronProgram,":/shaders/neuronFloat.vert",":/shaders/float.frag"))
        return;
    if ( !m_floatNeuronProgram->bind() )
    {
        qDebug() << "Could not bind shader program to context";
        return;
    }
    m_floatNeuronProgram->release();

    //compile shader
    m_bitConnectionProgram = new QGLShaderProgram(this->context(),this);
    if (!QGLXCore::compileShaderProgram(m_bitConnectionProgram,":/shaders/connectionBit.vert",":/shaders/connectionBit.geom",":/shaders/bit.frag"))
        return;
    if ( !m_bitConnectionProgram->bind() )
    {
        qDebug() << "Could not bind shader program to context";
        return;
    }
    m_bitConnectionProgram->release();


    //compile shader
    m_floatConnectionProgram = new QGLShaderProgram(this->context(),this);
    if (!QGLXCore::compileShaderProgram(m_floatConnectionProgram,":/shaders/connectionFloat.vert",":/shaders/connectionFloat.geom",":/shaders/float.frag"))
        return;
    if ( !m_floatConnectionProgram->bind() )
    {
        qDebug() << "Could not bind shader program to context";
        return;
    }
    m_floatConnectionProgram->release();

    // establish initial values for neuron shader uniforms
    m_neuronScale = QMatrix4x4();
    m_neuronScale.scale(1000.0f);




    // compile selection shader
    m_selectionRectProgram = new QGLShaderProgram(this->context(),this);
    if (!QGLXCore::compileShaderProgram(m_selectionRectProgram,":/shaders/postProcess.vert",":/shaders/selectionRect.frag"))
        return;
    if ( !m_selectionRectProgram->bind() )
    {
        qDebug() << "Could not bind shader program to context";
        return;
    }
    m_selectionRectProgram->setUniformValue("SelectionColor",QVector3D(1,1,1));
    m_selectionRectProgram->release();


    // initialize buffer to hold selection rect draw points
    m_selectionRectVertices.create();
    m_selectionRectVertices.bind();
    m_selectionRectVertices.setUsagePattern( QGLXBuffer::StaticDraw );
    m_selectionRectVertices.allocate(NULL,5 * sizeof(QVector3D),GL_FLOAT);
    m_selectionRectVertices.release();



    // compile shader
    m_selectionProgram = new QGLShaderProgram(this->context(),this);
    if (!QGLXCore::compileShaderProgram(m_selectionProgram,":/shaders/postProcess.vert",":/shaders/selection.frag"))
        return;
    if ( !m_selectionProgram->bind() )
    {
        qDebug() << "Could not bind shader program to context";
        return;
    }
    m_selectionProgram->setUniformValue("SelectionColor",QVector3D(0.1,0.78,0.0));
    m_selectionProgram->release();

    // compile shader
    m_lightingProgram = new QGLShaderProgram(this->context(),this);
    if (!QGLXCore::compileShaderProgram(m_lightingProgram,":/shaders/postProcess.vert",":/shaders/lighting.frag"))
        return;
    if ( !m_lightingProgram->bind() )
    {
        qDebug() << "Could not bind shader program to context";
        return;
    }



    m_lightingProgram->setUniformValue("AmbientIntensity",0.5f);
    m_lightingProgram->setUniformValue("DiffuseIntensity",0.05f);
    m_lightingProgram->setUniformValue("SpecularIntensity",0.2f);
    // establish initial values for connection shader uniforms
    m_lightingProgram->release();

    // compile shader
    m_blendProgram = new QGLShaderProgram(this->context(),this);
    if (!QGLXCore::compileShaderProgram(m_blendProgram,":/shaders/postProcess.vert",":/shaders/simpleBlend.frag"))
        return;
    if ( !m_blendProgram->bind() )
    {
        qDebug() << "Could not bind shader program to context";
        return;
    }


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

    QVector3D range[2] = {QVector3D(1,0,1),QVector3D(1,0,0)};
    m_neuronRangeMap = new QGLXTexture1D();
    m_neuronRangeMap->create();
    m_neuronRangeMap->bind();
    m_neuronRangeMap->allocate(2,GL_RGB32F,range);
    m_neuronRangeMap->setMinFilter(QGLXTexture1D::Linear);
    m_neuronRangeMap->setMagFilter(QGLXTexture1D::Linear);
    m_neuronRangeMap->setWrapFunction(QGLXTexture1D::Clamp);
    m_neuronRangeMap->release();


    m_connectionRangeMap = new QGLXTexture1D();
    m_connectionRangeMap->create();
    m_connectionRangeMap->bind();
    m_connectionRangeMap->allocate(2,GL_RGB32F,range);
    m_connectionRangeMap->setMinFilter(QGLXTexture1D::Linear);
    m_connectionRangeMap->setMagFilter(QGLXTexture1D::Linear);
    m_connectionRangeMap->setWrapFunction(QGLXTexture1D::Clamp);
    m_connectionRangeMap->release();


    m_timer.start();
    this->startTimer(1);
    m_initialized = true;
}

void NCV::timerEvent(QTimerEvent * e)
{

    if (!isValid())
        return;
    e->accept();
    // perform refresh
    paintGL();
    newFPSReading(m_fps);
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

    for (QMap<QString,QGLXTexture2D>::iterator it = m_maps.begin();it != m_maps.end(); it++)
        it.value().setSize(targetWidth,targetHeight);
}

bool NCV::isValid()
{
    return m_versionCapable && QGLWidget::isValid();
}


void NCV::m_createNetwork()
{
    if (!isValid())
        return;
    Assimp::Importer importer;
    // load instance model ,do some post import optimizations
    const aiScene * model = importer.ReadFile("../source/assets/cube.nff", aiProcess_Triangulate |aiProcess_JoinIdenticalVertices| aiProcess_FindInvalidData| aiProcess_GenSmoothNormals |aiProcess_FixInfacingNormals   );

    QVector<GLuint> indices;

    for (unsigned int i = 0; i < model->mMeshes[0]->mNumFaces; i ++)
        for (unsigned int j = 0; j < model->mMeshes[0]->mFaces[i].mNumIndices; j ++)
            indices.push_back(model->mMeshes[0]->mFaces[i].mIndices[j]);

    // create neuron system
    m_neurons.create(m_neuronsToCreate,model->mMeshes[0]->mNumVertices,indices.size(),1);
    m_neurons.setPrimitiveType(QGLXSystem::Triangle);

    // set per-vertex attributes of model
    m_neurons.setVertexAttributeIndices(&indices[0]);
    m_neurons.setVertexAttributeArray("Vert_Normal",model->mMeshes[0]->mNormals,sizeof(aiVector3D),GL_FLOAT);
    m_neurons.setVertexAttributeArray("Vert_Position",model->mMeshes[0]->mVertices,sizeof(aiVector3D),GL_FLOAT);

    // establish system ids
    GLuint * neuronIDs = new GLuint[m_neuronsToCreate];
    for (int i =0; i < m_neuronsToCreate; i ++)
        neuronIDs[i] = i +1;
    m_neurons.setInstanceAttributeArray("Inst_ID",neuronIDs,sizeof(GLuint),GL_FLOAT,1);


    // create connection system
    m_connections.create(m_connectionsToCreate,2,0,1);
    m_connections.setPrimitiveType(QGLXSystem::Line);

    // establish system ids
    GLuint offset[2] = {0,1};
    m_connections.setVertexAttributeArray("Vert_Offset",offset,sizeof(GLuint),GL_FLOAT);

    // establish system ids
    GLuint * connectionIDs = new GLuint[m_connectionsToCreate];
    for (int i =0; i < m_connectionsToCreate; i ++)
        connectionIDs[i] = m_neuronsToCreate+ i +1;
    m_connections.setInstanceAttributeArray("Inst_ID",connectionIDs,sizeof(GLuint),GL_FLOAT,1);
    m_connections.setInstanceAttributeArray("Inst_Neuron_IN",m_inNeurons,sizeof(GLuint),GL_FLOAT,1);
    m_connections.setInstanceAttributeArray("Inst_Neuron_OUT",m_outNeurons,sizeof(GLuint),GL_FLOAT,1);


    // set world information based off positions inputted
    m_cameras[m_currentCamera]->setNearPlane(10.0f);
    m_cameras[m_currentCamera]->setFarPlane(100000);

    m_lightingProgram->bind();
    m_lightingProgram->setUniformValue("WorldSize",m_worldSize);
    m_lightingProgram->setUniformValue("WorldCenter",m_worldCenter);
    m_lightingProgram->release();

    m_connectionsToCreate = 0;
    m_neuronsToCreate = 0;
}


QGLShaderProgram * NCV::m_neuronProgram()
{
    if ( m_neuronAttribToRender != NULL)
    {

        if (m_neuronAttribToRender->type == Attribute::Flag)
            return m_bitNeuronProgram;
        else if (m_neuronAttribToRender->type == Attribute::Range)
            return m_floatNeuronProgram;

    }

    return NULL;
}

QGLShaderProgram * NCV::m_connectionProgram()
{
    if ( m_connectionAttribToRender != NULL)
    {

        if (m_connectionAttribToRender->type == Attribute::Flag)
            return m_bitConnectionProgram;
        else if (m_connectionAttribToRender->type == Attribute::Range)
            return m_floatConnectionProgram;

    }

    return NULL;

}

void NCV::m_bindConnections()
{

    if (m_connectionProgram() !=NULL && m_connectionAttribToRender != NULL )
    {

        QGLShaderProgram  *connectionProgram = m_connectionProgram();

        connectionProgram->bind();
        connectionProgram->setUniformValue("View",m_cameras[m_currentCamera]->getViewMatrix());
        connectionProgram->setUniformValue("Projection",m_cameras[m_currentCamera]->getProjectionMatrix());
        connectionProgram->setUniformValue("ScreenSize",QVector2D(m_width,m_height));
        connectionProgram->setUniformValue("CameraPosition",m_cameras[m_currentCamera]->getPosition());
        connectionProgram->setUniformValue("ConnectionWidth",80.0f);
        connectionProgram->setUniformValue("CameraDirection",m_cameras[m_currentCamera]->getForward());
        connectionProgram->setUniformValue("FogEnd",m_cameras[m_currentCamera]->getFarPlane());
        connectionProgram->setUniformValue("FogStart",m_cameras[m_currentCamera]->getNearPlane());
        connectionProgram->setUniformValue("WorldCenter",m_worldCenter);
        connectionProgram->setUniformValue("WorldSize",m_worldSize);
        connectionProgram->setUniformValue("DeselectionColor",QVector4D(1,1,1,0.05));
        int rangeLoc = connectionProgram->attributeLocation("RangeMap");

        if (rangeLoc != -1)
        {
            glActiveTexture(GL_TEXTURE3);
            m_connectionRangeMap->bind();
            connectionProgram->setUniformValue("RangeMap",3);
        }

        m_connections.bind(connectionProgram);

        int textureSlot = 0;

        glActiveTexture(GL_TEXTURE0 + textureSlot);
        m_translationBuffer.bindBuffer(m_neurons.numObjects(),m_connections.numObjects());

        int posLoc = connectionProgram->uniformLocation("Inst_Translation");
        connectionProgram->setUniformValue(posLoc, textureSlot);

        textureSlot++;


        glActiveTexture(GL_TEXTURE0 + textureSlot);
        m_connectionAttribToRender->bindBuffer(m_neurons.numObjects(),m_connections.numObjects());

        int attribLoc = connectionProgram->uniformLocation("Inst_Attribute");
        connectionProgram->setUniformValue(attribLoc, textureSlot);

        if (m_connectionAttribToRender->type == Attribute::Flag)
        {
            connectionProgram->setUniformValue("OnColor", m_connectionAttribToRender->onColor);
            connectionProgram->setUniformValue("OffColor", m_connectionAttribToRender->offColor);
        }
        else if (m_connectionAttribToRender->type == Attribute::Range)
        {
            connectionProgram->setUniformValue("MinimumValue", m_connectionAttribToRender->minValue);
            connectionProgram->setUniformValue("MaximumValue", m_connectionAttribToRender->maxValue);
        }
    }

}

void NCV::m_releaseConnections()
{
    m_translationBuffer.release();
    m_connectionAttribToRender->release();
    m_connections.release(m_connectionProgram());
    m_connectionProgram()->release();
}

void NCV::m_bindNeurons()
{


    if (m_neuronProgram() != NULL && m_neuronAttribToRender != NULL)
    {
        // bind current state of neurons to current neuron program being rendered. (as it could have been switched)
        QGLShaderProgram * neuronProgram = m_neuronProgram();
        neuronProgram->bind();
        neuronProgram->setUniformValue("View",m_cameras[m_currentCamera]->getViewMatrix());
        neuronProgram->setUniformValue("Projection",m_cameras[m_currentCamera]->getProjectionMatrix());
        neuronProgram->setUniformValue("ScreenSize",QVector2D(m_width,m_height));
        neuronProgram->setUniformValue("Scale",m_neuronScale);
        neuronProgram->setUniformValue("CameraPosition",m_cameras[m_currentCamera]->getPosition());
        neuronProgram->setUniformValue("FogEnd",m_cameras[m_currentCamera]->getFarPlane());
        neuronProgram->setUniformValue("FogStart",m_cameras[m_currentCamera]->getNearPlane());
        neuronProgram->setUniformValue("DeselectionColor",QVector4D(1,1,1,0.05));
        neuronProgram->setUniformValue("WorldCenter",m_worldCenter);
        neuronProgram->setUniformValue("WorldSize",m_worldSize);

        // if the attribute being displayed is a range attribute, bind the associated range map (pulled from ui)
        int rangeLoc = neuronProgram->attributeLocation("RangeMap");
        if (rangeLoc != -1)
        {
            glActiveTexture(GL_TEXTURE3);
            m_neuronRangeMap->bind();
            neuronProgram->setUniformValue("RangeMap",3);
        }

        // bind geometric data associated with neurons
        m_neurons.bind(neuronProgram);

        int textureSlot = 0;

        // bind neuron position buffer to first texture slot
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        m_translationBuffer.bindBuffer(m_neurons.numObjects(),m_connections.numObjects());

        // bind position buffer to current neuron shader program
        int posLoc = neuronProgram->uniformLocation("Inst_Translation");
        neuronProgram->setUniformValue(posLoc, textureSlot);

        textureSlot++;

        // bind current neuron attribute to next texture slot
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        m_neuronAttribToRender->bindBuffer(m_neurons.numObjects(),m_connections.numObjects());

        // bind attribute to current shader program
        int attribLoc = neuronProgram->uniformLocation("Inst_Attribute");
        neuronProgram->setUniformValue(attribLoc, textureSlot);

        // depending on which type of attribute the neuron attribute is, bind related uniforms to shader program
        if (m_neuronAttribToRender->type == Attribute::Flag)
        {
            neuronProgram->setUniformValue("OnColor", m_neuronAttribToRender->onColor);
            neuronProgram->setUniformValue("OffColor", m_neuronAttribToRender->offColor);
        }
        else if (m_neuronAttribToRender->type  == Attribute::Range)
        {
            neuronProgram->setUniformValue("MinimumValue", m_neuronAttribToRender->minValue);
            neuronProgram->setUniformValue("MaximumValue", m_neuronAttribToRender->maxValue);
        }


    }

}

void NCV::m_releaseNeurons()
{

    m_translationBuffer.release();
    m_neuronAttribToRender->release();
    m_neurons.release(m_neuronProgram());
    m_neuronProgram()->release();
}
void NCV::m_performRegularRender()
{

    // bind fbo and related targets
    m_frameBufferObject.bind();

    m_frameBufferObject.enableColorAttachments(4);
    m_maps["diffuse"].bindAsFrameBufferTexture(QGLXTexture2D::Draw,QGLXTexture2D::Color0);
    m_maps["position"].bindAsFrameBufferTexture(QGLXTexture2D::Draw,QGLXTexture2D::Color1);
    m_maps["normal"].bindAsFrameBufferTexture(QGLXTexture2D::Draw,QGLXTexture2D::Color2);
    m_maps["id"].bindAsFrameBufferTexture(QGLXTexture2D::Draw,QGLXTexture2D::Color3);
    m_maps["depth"].bindAsFrameBufferTexture(QGLXTexture2D::Draw,QGLXTexture2D::Depth);

    // clear targets
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // render connections
    if (m_renderConnections && m_connectionAttribToRender != NULL)
    {
        m_bindConnections();
        m_connections.draw();
        m_releaseConnections();

    }

    // render neurons
    if (m_renderNeurons && m_neuronAttribToRender != NULL)
    {
        m_bindNeurons();
        m_neurons.draw();
        m_releaseNeurons();
    }
    // release frame buffer and related targets
    m_maps["diffuse"].release();
    m_maps["position"].release();
    m_maps["normal"].release();
    m_maps["id"].release();
    m_maps["depth"].release();
    m_frameBufferObject.enableColorAttachments(0);
    m_frameBufferObject.release();
}

void NCV::m_performSelectionRender()
{

    // bind frame buffer and all first pass targets
    m_frameBufferObject.bind();
    m_frameBufferObject.enableColorAttachments(4);
    m_maps["selected"].bindAsFrameBufferTexture(QGLXTexture2D::Draw,QGLXTexture2D::Color0);
    m_maps["position"].bindAsFrameBufferTexture(QGLXTexture2D::Draw,QGLXTexture2D::Color1);
    m_maps["normal"].bindAsFrameBufferTexture(QGLXTexture2D::Draw,QGLXTexture2D::Color2);
    m_maps["id"].bindAsFrameBufferTexture(QGLXTexture2D::Draw,QGLXTexture2D::Color3);
    m_maps["depth"].bindAsFrameBufferTexture(QGLXTexture2D::Draw,QGLXTexture2D::Depth);

    // clear targets, enable alpha blending
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0001f);
    glClear( GL_COLOR_BUFFER_BIT);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    if (m_renderNeurons && m_neuronAttribToRender != NULL)
    {
        m_bindNeurons();
        QGLShaderProgram * program = m_neuronProgram();
        program->setUniformValue("Deselected",0);

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
        program->setUniformValue("Deselected",1);
        m_releaseNeurons();
    }
    if (m_renderConnections && m_connectionAttribToRender != NULL)
    {

        m_bindConnections();
        QGLShaderProgram * program = m_connectionProgram();
        program->setUniformValue("Deselected",0);

        for (QVector<Range>::iterator it = m_ranges.begin() ; it != m_ranges.end(); it++)
        {
            Range range = *it;

            if (range.end <= m_neurons.numObjects())
                continue;
            else if (range.start <= m_neurons.numObjects())
            {
                m_connections.drawSubset(0,range.end -(m_neurons.numObjects() +1));

            }
            else
                m_connections.drawSubset(range.start-1 - m_neurons.numObjects(),range.end-range.start);

        }
        program->setUniformValue("Deselected",1);
        m_releaseConnections();

    }

    // release frame buffer and render to back buffer

    m_maps["selected"].release();

    if (m_renderOnlySelection)
    {

        glDisable(GL_ALPHA_TEST);
        m_maps["position"].release();
        m_maps["normal"].release();
        m_maps["id"].release();
        m_maps["depth"].release();
        m_translationBuffer.release();
        m_neuronAttribToRender->release();
        m_connectionAttribToRender->release();
        m_frameBufferObject.release();
    }
    else
    {
        m_maps["id"].release();
        m_maps["deselected"].bindAsFrameBufferTexture(QGLXTexture2D::Draw,QGLXTexture2D::Color0);

        // clear only the deselected texture to keep contents of id / normal /position maps.
        m_frameBufferObject.enableColorAttachments(1);
        glClear(GL_COLOR_BUFFER_BIT );
        m_frameBufferObject.enableColorAttachments(4);

        if (m_renderNeurons  && m_neuronAttribToRender != NULL)
        {
            m_bindNeurons();
            QGLShaderProgram * program = m_neuronProgram();
            program->setUniformValue("Deselected",1);
            GLuint previousEnd = 1;
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
            m_releaseNeurons();
        }
        if (m_renderConnections && m_connectionAttribToRender != NULL)
        {
            m_bindConnections();
            QGLShaderProgram * program = m_connectionProgram();
            program->setUniformValue("Deselected",1);
            GLuint previousEnd = m_neurons.numObjects() +1;
            GLuint endOfItems = m_neurons.numObjects() + m_connections.numObjects();
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

            m_releaseConnections();
        }

        m_translationBuffer.release();
        m_neuronAttribToRender->release();
        m_connectionAttribToRender->release();

        m_maps["deselected"].release();
        m_maps["position"].release();
        m_maps["normal"].release();
        m_maps["id"].release();
        m_maps["depth"].release();


        glDisable(GL_ALPHA_TEST);

        m_frameBufferObject.enableColorAttachments(1);
        m_maps["diffuse"].bindAsFrameBufferTexture(QGLXTexture2D::Draw,QGLXTexture2D::Color0);

        glClear(GL_COLOR_BUFFER_BIT );
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);

        m_blendProgram->bind();
        m_screenVertices.bind();
        m_blendProgram->setAttributeBuffer( "Vert_Position", GL_FLOAT,  0 ,3,sizeof(GL_FLOAT) * 3);
        m_blendProgram->enableAttributeArray("Vert_Position");
        glVertexAttribDivisor( m_blendProgram->attributeLocation("Vert_Position"),0);

        m_screenCoords.bind();
        m_blendProgram->setAttributeBuffer( "Vert_Coord", GL_FLOAT,  0 ,2,sizeof(GL_FLOAT) * 2);
        m_blendProgram->enableAttributeArray("Vert_Coord");
        glVertexAttribDivisor( m_blendProgram->attributeLocation("Vert_Coord"), 0);

        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,m_maps["selected"].id());
        m_blendProgram->setUniformValue("BlendMap",0);

        glDrawArrays(GL_QUADS,0,4);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,m_maps["deselected"].id());
        m_blendProgram->setUniformValue("BlendMap",0);

        glDrawArrays(GL_QUADS,0,4);

        m_maps["diffuse"].release();
        glBindTexture(GL_TEXTURE_2D,0);
        m_screenCoords.release();
        m_screenVertices.release();
        m_blendProgram->release();
        m_frameBufferObject.release();
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }




}

void NCV::setMaterial(QGLXMaterial * mat)
{
    m_lightingProgram->bind();
    m_lightingProgram->setUniformValue("AmbientIntensity",mat->AmbientIntensity);
    m_lightingProgram->setUniformValue("DiffuseIntensity",mat->DiffuseIntensity);
    m_lightingProgram->setUniformValue("SpecularIntensity",mat->SpecularIntensity);
    m_lightingProgram->release();

}

void NCV::paintGL()
{
    if (!isValid())
        return;


    // FPS counter
    m_frameCount ++;
    if (m_timer.elapsed() >= 1000)
    {
        m_fps = (float)m_frameCount/((float)m_timer.elapsed() /1000.0f) ;
        m_timer.start();
        m_frameCount = 0;
    }
    // check to see if any changes were requested externally
    if (m_neuronsToCreate >0 || m_connectionsToCreate > 0)
        m_createNetwork();



    if (m_selectedObjects.count() >0)
        m_performSelectionRender();
    else
        m_performRegularRender();



    int enabledLights = 0;
    m_lightingProgram->bind();
    for(QMap<QString,QGLXLight*>::iterator it = m_lights.begin();it != m_lights.end(); it++)
    {
        QGLXLight *light = it.value();
        if (light->enabled)
        {
            QString indexStr = QString("%1").arg(enabledLights);
            QString variableNames[7] = {"SpecularPower", "ConstantAttenuation","LinearAttenuation","QuadraticAttenuation","Radius",
                                        "Color","Position"};

            float floatVars[5] = {light->specularPower, light->constantAttenuation,light->linearAttenuation,light->quadtraticAttenuation,light->radius};
            QVector3D qVecVars[2] ={light->color,light->position};
            for (int i = 0; i <7; i++)
            {
                QString wholeString = "Lights["+indexStr+"]." +variableNames[i];

                if (i < 5)
                    m_lightingProgram->setUniformValue(wholeString.toStdString().c_str(),floatVars[i]);

                else
                    m_lightingProgram->setUniformValue(wholeString.toStdString().c_str(),qVecVars[i-5]);

            }
            enabledLights++;
        }
    }
    m_lightingProgram->release();
    if (enabledLights > 0)
    {
        GLuint diffuseID = m_maps["diffuse"].id();
        m_lightingProgram->bind();
        QVector3D cameraPosition = m_cameras[m_currentCamera]->getPosition();
        m_lightingProgram->setUniformValue("CameraPosition",cameraPosition);
        m_lightingProgram->setUniformValue("CameraDirection",m_cameras[m_currentCamera]->getForward());
        m_lightingProgram->setUniformValue("NumLights",enabledLights);

        m_screenVertices.bind();
        m_lightingProgram->setAttributeBuffer( "Vert_Position", GL_FLOAT,  0 ,3,sizeof(GL_FLOAT) * 3);
        m_lightingProgram->enableAttributeArray("Vert_Position");
        glVertexAttribDivisor( m_lightingProgram->attributeLocation("Vert_Position"),0);

        m_screenCoords.bind();
        m_lightingProgram->setAttributeBuffer( "Vert_Coord", GL_FLOAT,  0 ,2,sizeof(GL_FLOAT) * 2);
        m_lightingProgram->enableAttributeArray("Vert_Coord");
        glVertexAttribDivisor( m_lightingProgram->attributeLocation("Vert_Coord"), 0);

        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,diffuseID);
        m_lightingProgram->setUniformValue("DiffuseMap",0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,m_maps["normal"].id() );
        m_lightingProgram->setUniformValue("NormalMap",1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D,m_maps["position"].id() );
        m_lightingProgram->setUniformValue("PositionMap",2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D,m_maps["id"].id() );
        m_lightingProgram->setUniformValue("IdentityMap",3);

        glClear( GL_DEPTH_BUFFER_BIT );
        glDrawArrays(GL_QUADS,0,4);

        glBindTexture(GL_TEXTURE_2D,0);
        m_screenCoords.release();
        m_screenVertices.release();
        m_lightingProgram->release();

    }

    else
        m_frameBufferObject.blitTarget(m_maps["diffuse"],QRect(0,0,m_width,m_height),QRect(0,0,m_width,m_height));



    // if user is currently selecting objects, render a selection rectangle.
    if (m_leftMouseDown)
    {
        QRect selectionRect = m_selectionRect;
        m_selectionRectProgram->bind();
        m_selectionRectVertices.bind();
        QPoint selectionPoints2D[5] = {selectionRect.topLeft(),selectionRect.topRight(),selectionRect.bottomRight(),selectionRect.bottomLeft(),selectionRect.topLeft()};
        QVector3D selectionPoints[5];
        QVector3D texCoords[5];
        for (int i = 0 ; i < 5; i ++)
            selectionPoints[i] = QVector3D(2*((float)selectionPoints2D[i].x() / (float)m_width) - 1.0f,2.0f * (1.0f - (float)selectionPoints2D[i].y()/(float)m_height) - 1.0f,0.5);

        m_selectionRectVertices.write(0,selectionPoints,5 * sizeof(QVector3D));
        m_selectionRectProgram->setAttributeBuffer( "Vert_Position", GL_FLOAT,  0 ,3, sizeof(QVector3D));
        m_selectionRectProgram->enableAttributeArray("Vert_Position");
        glVertexAttribDivisor( m_selectionRectProgram->attributeLocation("Vert_Position"), 0);


        /*m_selectionRectVertices.write(0,texCoords,5 * sizeof(QVector2D));
        m_selectionRectProgram->setAttributeBuffer( "Vert_Coord", GL_FLOAT,  0 ,2, sizeof(QVector2D));
        m_selectionRectProgram->enableAttributeArray("Vert_Coord");
        glVertexAttribDivisor( m_selectionRectProgram->attributeLocation("Vert_Coord"), 0);*/

        glClear( GL_DEPTH_BUFFER_BIT );
        glDrawArrays(GL_LINE_STRIP,0,5);
        m_selectionRectVertices.release();
        m_selectionRectProgram->release();
    }

    this->swapBuffers();

}


void NCV::leaveEvent(QEvent * e)
{
    if (!isValid())
        return;
    this->parentWidget()->setFocus();
}

void NCV::enterEvent(QEvent * e)
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
        m_cameras[m_currentCamera]->rotate((float)(newPos.x() - m_mousePosition.x()) * -m_turnSpeed,(float)(newPos.y() - m_mousePosition.y()) * -m_turnSpeed);
        m_mousePosition = newPos;
        cameraUpdated(m_currentCamera);
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
        m_frameBufferObject.getTargetPixels(m_maps["id"],m_selectionRect,objectID);

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
            GLuint currentRange = 0;
            for (QList<GLuint>::iterator it = list.begin() +1; it != list.end(); it++)
            {
                GLuint id = *it;
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
            m_cameras[m_currentCamera]->pan(-m_moveSpeed,0.0f,0.0f);
            cameraUpdated(m_currentCamera);
            break;

        case Qt::Key_D:
            m_cameras[m_currentCamera]->pan(m_moveSpeed,0.0f,0.0f);
            cameraUpdated(m_currentCamera);
            break;


        case Qt::Key_R:
            m_cameras[m_currentCamera]->pan(0.0f,0.0f,m_moveSpeed/ 10.0f);
            cameraUpdated(m_currentCamera);
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
            m_cameras[m_currentCamera]->pan(0.0f,0.0f,m_moveSpeed/ 25.0f);
            cameraUpdated(m_currentCamera);
            break;


        case Qt::Key_P:
            m_cameras[m_currentCamera]->pan(0.0f,0.0f,m_moveSpeed/ 25.0f);
            cameraUpdated(m_currentCamera);
            break;

        case Qt::Key_V:
            m_cameras[m_currentCamera]->pan(0.0f,0.0f,-m_moveSpeed/ 10.0f);
            cameraUpdated(m_currentCamera);
            break;

        case Qt::Key_W:
            m_cameras[m_currentCamera]->pan(0.0f,0.0f,m_moveSpeed);
            cameraUpdated(m_currentCamera);
            break;

        case Qt::Key_S:
            m_cameras[m_currentCamera]->pan(0.0f,0.0f,-m_moveSpeed);
            cameraUpdated(m_currentCamera);
            break;

        case Qt::Key_Q:
            m_cameras[m_currentCamera]->pan(0.0f,m_moveSpeed,0.0f);
            cameraUpdated(m_currentCamera);
            break;

        case Qt::Key_E:
            m_cameras[m_currentCamera]->pan(0.0f,-m_moveSpeed,0.0f);
            cameraUpdated(m_currentCamera);
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



void NCV::createNeurons(int number,QVector3D * positions )
{
    if (!isValid())
        return;
    m_neuronsToCreate = number;
    m_translationBuffer = Attribute(Attribute::Neuron,positions);

    m_establishWorldBounds(number,positions);

}
void NCV::createNeurons(const QString & filename )
{

    if (!isValid())
        return;
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);   // we will serialize the data into the file

    // ######## Stub Protocol
    in >> m_neuronsToCreate;
    QVector3D * positions = new QVector3D[m_neuronsToCreate];
    for (int i = 0; i> m_neuronsToCreate; i++)
        in >> positions[i];
    // ######## Stub Protocol
    file.close();

    m_translationBuffer = Attribute(Attribute::Neuron,positions);
    m_establishWorldBounds(m_neuronsToCreate,positions);
}

void NCV::createConnections(int number,GLuint * neruonIN,GLuint * neruonOut )
{
    if (!isValid())
        return;
    m_connectionsToCreate = number;
    m_inNeurons = neruonIN;
    m_outNeurons = neruonOut;
}
void NCV::createConnections(const QString & filename )
{

    if (!isValid())
        return;
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);   // we will serialize the data into the file


    // ######## Stub Protocol
    in >> m_connectionsToCreate;
    GLuint * inNeurons = new GLuint[m_connectionsToCreate];
    GLuint * outNeurons = new GLuint[m_connectionsToCreate];
    for (int i = 0; i> m_connectionsToCreate; i++)
        in >> inNeurons[i] >> outNeurons[i];
    // ######## Stub Protocol
    file.close();


    m_inNeurons = inNeurons;
    m_outNeurons = outNeurons;
}


void NCV::setNeuronRangeAttribute(QString name, GLfloat * data,GLfloat minValue, GLfloat maxValue, RangeInferenceFunction func)
{
    if (!isValid())
        return;

    m_attributes[name] = Attribute(Attribute::Neuron,minValue,maxValue,data);
}

void NCV::setNeuronFlagAttribute(QString name, GLubyte * data, QVector3D onColor,QVector3D offColor, FlagInferenceFunction func)
{

    if (!isValid())
        return;

    m_attributes[name] = Attribute(Attribute::Neuron,onColor,offColor,data);

}

void NCV::setConnectionRangeAttribute(QString name, GLfloat * data,GLfloat minValue, GLfloat maxValue)
{

    if (!isValid())
        return;

    m_attributes[name] = Attribute(Attribute::Connection,minValue,maxValue,data);
}

void NCV::setConnectionFlagAttribute(QString name, GLubyte * data, QVector3D onColor,QVector3D offColor)
{

    if (!isValid())
        return;

    m_attributes[name] = Attribute(Attribute::Connection,onColor,offColor,data);
}



void NCV::setNeuronAttributeToRender(QString name)
{
    if (m_attributes.contains(name))
        m_neuronAttribToRender = &m_attributes[name];

}

void NCV::setConnectionAttributeToRender(QString name)
{

    if (m_attributes.contains(name))
        m_connectionAttribToRender = &m_attributes[name];
}

