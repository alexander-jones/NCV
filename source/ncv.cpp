#include "ncv.h"
#include <QTimer>
#include <QDir>
#include <QRect>
#include "time.h"

NCV::NCV( const QGLFormat& format, QWidget* parent )
    : QGLWidget( format, parent )
{
    m_camera = QGLXCamera();
    m_camera.setViewMatrix(QVector3D(0,0,0), QVector3D(0,0,-1),QVector3D(0,1,0));
    m_camera.setProjectionMatrix(45.0, 4.0/3.0, 0.1, 500000.0);
    m_leftMouseDown = false;
    m_rightMouseDown = false;
    m_frameTargetBuffer =  QGLXDynamicFrameBufferObject();
    m_moveSpeed = 5000;
    m_turnSpeed = 0.005f;
    m_renderSetting = Diffuse;
    m_worldSize = QVector3D(5000,5000,5000);
    m_frameCount = 0;
    m_neuronsToCreate = 0;
    m_connectionsToCreate = 0;
    m_shiftDown = false;
    m_startSelectionID = -1;
    m_renderNeurons= true;
    m_renderConnections= true;
    m_newVisualizationParameters = false;
    m_connectionSelectionState = Regular;
    srand(time(NULL));
    setFocus();
}


NCV::~NCV()
{
}

void NCV::establishWorldBounds(int numNeurons,QVector3D * neuronPositions)
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
    GLenum res = glewInit();

    QGLFormat glFormat = QGLWidget::format();
    if ( !glFormat.sampleBuffers() )
        qWarning() << "Could not enable sample buffers";


    // Set the clear color to black
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glLineWidth(1);

    // initialize target bufferm_neuronsToCreate
    m_frameTargetBuffer.init();
    m_frameTargetBuffer.addTarget("diffuse",1000,1000,GL_RGB8,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER);
    m_frameTargetBuffer.addTarget("position",1000,1000,GL_RGB8,GL_COLOR_ATTACHMENT1,GL_RENDERBUFFER);
    m_frameTargetBuffer.addTarget("voltage",1000,1000,GL_RGB8,GL_COLOR_ATTACHMENT2,GL_RENDERBUFFER);
    m_frameTargetBuffer.addTarget("firing",1000,1000,GL_RGB8,GL_COLOR_ATTACHMENT3,GL_RENDERBUFFER);
    m_frameTargetBuffer.addTarget("pick",1000,1000,GL_R32UI,GL_COLOR_ATTACHMENT4,GL_TEXTURE_2D);
    m_frameTargetBuffer.addTarget("depth",1000,1000,GL_DEPTH_COMPONENT32,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER);

    m_firstPassTargets.push_back("diffuse");
    m_firstPassTargets.push_back("position");
    m_firstPassTargets.push_back("voltage");
    m_firstPassTargets.push_back("firing");
    m_firstPassTargets.push_back("pick");

    // compile shader
    if (!compileShaderProgram(m_neuronProgram,":/shaders/neuron.vert",":/shaders/neuron.frag"))
        return;
    if ( !m_neuronProgram.bind() )
    {
        qDebug() << "Could not bind shader program to context";
        return;
    }

    // establish initial values for neuron shader uniforms
    m_neuronScale = QMatrix4x4();
    m_neuronScale.scale(100.0f);
    m_neuronProgram.setUniformValue("Scale",m_neuronScale);
    m_neuronProgram.setUniformValue("View",m_camera.getViewMatrix());
    m_neuronProgram.setUniformValue("Projection",m_camera.getProjectionMatrix());
    m_neuronProgram.setUniformValue("WorldSize",m_worldSize);
    m_neuronProgram.setUniformValue("SelectionColor",QVector3D(1,1,0));
    m_neuronProgram.setUniformValue("View",m_camera.getViewMatrix());
    m_neuronProgram.setUniformValue("SelectionID",0);
    m_neuronProgram.release();


    // compile shader
    if (!compileShaderProgram(m_connectionProgram,":/shaders/synapse.vert",":/shaders/synapse.geom",":/shaders/synapse.frag"))
        return;
    if ( !m_connectionProgram.bind() )
    {
        qDebug() << "Could not bind shader program to context";
        return;
    }

    // establish initial values for connection shader uniforms
    m_connectionProgram.setUniformValue("View",m_camera.getViewMatrix());
    m_connectionProgram.setUniformValue("Projection",m_camera.getProjectionMatrix());
    m_connectionProgram.setUniformValue("WorldSize",m_worldSize);
    m_connectionProgram.setUniformValue("SelectionState",m_connectionSelectionState);
    m_connectionProgram.setUniformValue("NueronSelectionColor",QVector3D(1,0,0));
    m_connectionProgram.setUniformValue("SelectionColor",QVector3D(1,1,0));
    m_connectionProgram.setUniformValue("SelectionID",0);
    m_connectionProgram.setUniformValue("ColorIntensity",1.0f);
    m_connectionProgram.release();

    // compile selection shader
    if (!compileShaderProgram(m_selectionRectProgram,":/shaders/postProcess.vert",":/shaders/selectionRect.frag"))
        return;
    if ( !m_selectionRectProgram.bind() )
    {
        qDebug() << "Could not bind shader program to context";
        return;
    }
    m_selectionRectProgram.setUniformValue("SelectionColor",QVector3D(1,1,1));
    m_selectionRectProgram.release();


    // initialize buffer to hold selection rect draw points
    m_selectionRectVertices.create();
    m_selectionRectVertices.bind();
    m_selectionRectVertices.setUsagePattern( QGLXBuffer::StaticDraw );
    m_selectionRectVertices.allocate(NULL,5 * sizeof(QVector3D),GL_FLOAT);
    m_selectionRectVertices.release();
    // start refresh rate timer
    this->startTimer(5);
}

void NCV::timerEvent(QTimerEvent * e)
{
    // perform refresh
    paintGL();

    GLuint err = glGetError();
    if (err != 0)
        qDebug() <<err;

}
void NCV::onViewChanged()
{
    m_connectionProgram.bind();
    m_connectionProgram.setUniformValue("View",m_camera.getViewMatrix());
    m_connectionProgram.release();
    m_neuronProgram.bind();
    m_neuronProgram.setUniformValue("View",m_camera.getViewMatrix());
    m_neuronProgram.release();

}



void NCV::resizeGL( int w, int h )
{
    // Set the viewport to window dimensions
    m_width = w;
    m_height = h;
    glViewport( 0, 0, w, qMax( h, 1 ) );
    sizeTargets();

}
void NCV::sizeTargets()
{
    // resize the targets of the frame buffer
    QString targets[6] = {"diffuse","position","voltage","firing","pick","depth"};
    int targetWidth = m_width;
    int targetHeight = m_height;
    if (m_renderSetting == All)
    {
        targetWidth = m_width/2;
        targetHeight = m_height/2;
    }
    m_frameTargetBuffer.sizeTargets(6,&targets[0],targetWidth,targetHeight);

    // update camera, get new projection matrix for use with targets
    m_camera.setAspectRatio((float)targetWidth / (float)targetHeight);
    m_connectionProgram.bind();
    m_connectionProgram.setUniformValue("Projection",m_camera.getProjectionMatrix());
    m_connectionProgram.release();

    m_neuronProgram.bind();
    m_neuronProgram.setUniformValue("Projection",m_camera.getProjectionMatrix());
    m_neuronProgram.release();
}

void NCV::createNetwork()
{
    Assimp::Importer importer;
    // load instance model ,do some post import optimizations
    const aiScene * model = importer.ReadFile("../source/assets/cube.nff", aiProcess_JoinIdenticalVertices  | aiProcess_GenSmoothNormals
                                                | aiProcess_Triangulate|aiProcess_MakeLeftHanded  |aiProcess_FixInfacingNormals );

    QVector<GLuint> indices;

    for (unsigned int i = 0; i < model->mMeshes[0]->mNumFaces; i ++)
        for (unsigned int j = 0; j < model->mMeshes[0]->mFaces[i].mNumIndices; j ++)
            indices.push_back(model->mMeshes[0]->mFaces[i].mIndices[j]);

    // create neuron system
    m_neurons.create(m_neuronsToCreate,model->mMeshes[0]->mNumVertices,indices.size(),1);
    m_neurons.setPrimitiveType(QGLXSystem::Triangle);
    m_neurons.attachShaderProgram(&m_neuronProgram);


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
    m_connections.create(m_connectionsToCreate, 1,0,1);
    m_connections.setPrimitiveType(QGLXSystem::Point);
    m_connections.attachShaderProgram(&m_connectionProgram);

    // establish system ids
    GLuint * connectionIDs = new GLuint[m_connectionsToCreate];
    for (int i =0; i < m_connectionsToCreate; i ++)
        connectionIDs[i] = m_neuronsToCreate+ i +1;
    m_connections.setInstanceAttributeArray("Inst_ID",connectionIDs,sizeof(GLuint),GL_FLOAT,1);

    // initialize selection buffer
    GLubyte * selected = new GLubyte[m_neuronsToCreate + m_connectionsToCreate];
    for (int i = 0; i <m_neuronsToCreate+ m_connectionsToCreate; i ++)
        selected [i] =0;
    setCompoundAttributeArray("Inst_Selection",selected,GL_UNSIGNED_BYTE,1,Shared);

    // set world information based off positions inputted
    m_neuronProgram.bind();
    m_neuronProgram.setUniformValue("WorldSize",m_worldSize);
    m_neuronProgram.setUniformValue("WorldCenter",m_worldCenter);
    m_neuronProgram.release();
    m_connectionProgram.bind();
    m_connectionProgram.setUniformValue("WorldSize",m_worldSize);
    m_neuronProgram.setUniformValue("WorldCenter",m_worldCenter);
    m_connectionProgram.release();
    m_connectionsToCreate = 0;
    m_neuronsToCreate = 0;
}
void NCV::createAttributes()
{
    // loop through every attribute being created.
    for (QMap<QString,DataSet>::iterator it =  m_attributesToCreate.begin(); it != m_attributesToCreate.end(); it++)
    {
        DataSet attribToWrite = it.value();
        QString name = it.key();
        bool useTextureBuffer = false;

        // if nueron, decide how to store data
        if (attribToWrite.type == DataSet::Neuron)
        {
            if (attribToWrite.access == NeuronOnly)
                m_neurons.setInstanceAttributeArray(name,attribToWrite.data,attribToWrite.stride,attribToWrite.componentType,1);
            else
                useTextureBuffer = true;
        }
        // if connection, decide how to store data
        else if (attribToWrite.type == DataSet::Connection)
        {
            if (attribToWrite.access == ConnectionOnly)
                m_connections.setInstanceAttributeArray(name,attribToWrite.data,attribToWrite.stride,attribToWrite.componentType,1);
            else
                useTextureBuffer = true;
        }
        // if compound, use texture buffer
        else
            useTextureBuffer = true;

        // if using a texture buffer, create it and see what programs it should be bound to
        if (useTextureBuffer)
        {
            // if the buffer is already created, don't recreate it.
            if (!m_textureBuffers[it.key()].isCreated())
            {
                m_textureBuffers[it.key()] = QGLXBuffer(QGLXBuffer::TextureBuffer);
                m_textureBuffers[it.key()].create();
                m_textureBuffers[it.key()].setTextureSlot(m_textureBuffers.size()-1);
            }
            m_textureBuffers[it.key()].bind();
            GLenum textureFormat  = QGLXCore::bufferFormatToTextureFormat(attribToWrite.componentType,attribToWrite.tupleSize,attribToWrite.componentSize);
            if (attribToWrite.type == DataSet::Neuron)
                m_textureBuffers[it.key()].allocate(attribToWrite.data,attribToWrite.stride * m_neurons.numObjects(),textureFormat);
            else if (attribToWrite.type == DataSet::Connection)
                m_textureBuffers[it.key()].allocate(attribToWrite.data,attribToWrite.stride *  m_connections.numObjects(),textureFormat);
            else if (attribToWrite.type == DataSet::Compound)
                m_textureBuffers[it.key()].allocate(attribToWrite.data,attribToWrite.stride * (m_neurons.numObjects() + m_connections.numObjects()),textureFormat);

            if (attribToWrite.access != ConnectionOnly  )
            {
                // bind texture of texture buffer to neuron shader program
                m_neuronProgram.bind();
                int nLoc = m_neuronProgram.uniformLocation(it.key());
                m_neuronProgram.setUniformValue(nLoc, m_textureBuffers.size()-1);
                m_neuronProgram.release();
            }
            if (attribToWrite.access != NeuronOnly   )
            {
                // bind texture of texture buffer to connection shader program
                m_connectionProgram.bind();
                int sLoc = m_connectionProgram.uniformLocation(it.key());
                m_connectionProgram.setUniformValue(sLoc, m_textureBuffers.size()-1);
                m_connectionProgram.release();
            }

            // bind texture to neuron shader program
            m_textureBuffers[it.key()].release();
        }

    }
    m_attributesToCreate.clear();
}


void NCV::paintGL()
{
    // FPS counter
    m_frameCount ++;
    if (m_time.elapsed() >= 1000)
    {
        qDebug() << "FPS: " <<(float)m_frameCount * 1000 /m_time.elapsed() ;
        m_time.start();
        m_frameCount = 0;
    }


    if (m_neuronsToCreate >0 || m_connectionsToCreate > 0)
        createNetwork();

    if (m_attributesToCreate.size() > 0)
        createAttributes();

    if (m_newVisualizationParameters)
        setVisualizationParameters();

    // bind multiple target buffer
    m_frameTargetBuffer.bindTargets(m_firstPassTargets.size(),&m_firstPassTargets[0]);

    // clear screen
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    if (m_renderConnections)
    {

        m_connectionProgram.bind();
        int counter = 0;
        for (QMap<QString,QGLXBuffer>::iterator it = m_textureBuffers.begin(); it != m_textureBuffers.end(); it++)
        {
            it.value().bind();
            counter++;
        }
        m_connections.draw();
        m_connectionProgram.release();
    }


    if (m_renderNeurons)
    {
        m_neuronProgram.bind();
        int nCounter = 0;
        for (QMap<QString,QGLXBuffer>::iterator it = m_textureBuffers.begin(); it != m_textureBuffers.end(); it++)
        {
            it.value().bind();
            nCounter++;
        }
        m_neurons.draw();
        m_neuronProgram.release();
    }

    // release frame buffer and render to back buffer
    m_frameTargetBuffer.releaseTargets();

    // decide how to display maps on screen
    if (m_renderSetting == All)
    {
        m_frameTargetBuffer.targetToScreen("diffuse", QRect(m_width/2,0,m_width,m_height/2),QRect(m_width/2,0,m_width,m_height/2));
        m_frameTargetBuffer.targetToScreen("position", QRect(m_width/2,0,m_width,m_height/2),QRect(0, 0,m_width/2,m_height/2));
        m_frameTargetBuffer.targetToScreen("voltage", QRect(m_width/2,0,m_width,m_height/2),QRect(0,m_height/2,m_width/2,m_height));
        m_frameTargetBuffer.targetToScreen("firing", QRect(m_width/2,0,m_width,m_height/2),QRect(m_width/2,m_height/2,m_width,m_height));
    }
    else if (m_renderSetting == Diffuse)
        m_frameTargetBuffer.targetToScreen("diffuse", QRect(0,0,m_width,m_height),QRect(0,0,m_width,m_height));
    else if (m_renderSetting == Firing)
        m_frameTargetBuffer.targetToScreen("firing", QRect(0,0,m_width,m_height),QRect(0,0,m_width,m_height));
    else if (m_renderSetting == Voltage)
        m_frameTargetBuffer.targetToScreen("voltage", QRect(0,0,m_width,m_height),QRect(0,0,m_width,m_height));
    else if (m_renderSetting == Position)
        m_frameTargetBuffer.targetToScreen("position", QRect(0,0,m_width,m_height),QRect(0,0,m_width,m_height));

    // if user is currently selecting objects, render a selection rectangle.
    if (m_leftMouseDown)
    {
        QRect selectionRect = m_selectionRect;
        m_selectionRectProgram.bind();
        m_selectionRectVertices.bind();
        QPoint selectionPoints2D[5] = {selectionRect.topLeft(),selectionRect.topRight(),selectionRect.bottomRight(),selectionRect.bottomLeft(),selectionRect.topLeft()};
        QVector3D selectionPoints[5];
        for (int i = 0 ; i < 5; i ++)
            selectionPoints[i] = QVector3D(2*((float)selectionPoints2D[i].x() / (float)m_width) - 1.0f,2.0f * (1.0f - (float)selectionPoints2D[i].y()/(float)m_height) - 1.0f,0.5);

        m_selectionRectVertices.write(0,selectionPoints,5 * sizeof(QVector3D));
        m_selectionRectProgram.setAttributeBuffer( "Vert_Position", GL_FLOAT,  0 ,3, sizeof(QVector3D));
        m_selectionRectProgram.enableAttributeArray("Vert_Position");
        glVertexAttribDivisor( m_selectionRectProgram.attributeLocation("Vert_Position"), 0);
        glClear( GL_DEPTH_BUFFER_BIT );
        glLineWidth(2);
        glDrawArrays(GL_LINE_STRIP,0,5);
        glLineWidth(1);
        m_selectionRectVertices.release();
        m_selectionRectProgram.release();
    }

    this->swapBuffers();

}


void NCV::mouseMoveEvent(QMouseEvent* e)
{
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
        m_camera.rotate((float)(newPos.x() - m_mousePosition.x()) * -m_turnSpeed,(float)(newPos.y() - m_mousePosition.y()) * -m_turnSpeed);
        m_mousePosition = QVector2D(e->x(),e->y());
        onViewChanged();
    }

}
void NCV::wheelEvent(QWheelEvent *e)
{
    // scale neuron
    if(e->orientation() == Qt::Vertical)
    {

        m_neuronScale.scale((2000.0 - (double)e->delta()) / 2000.0);
        m_neuronProgram.bind();
        m_neuronProgram.setUniformValue("Scale",m_neuronScale);
        m_neuronProgram.release();

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

    // if middle button pressed, change render style
    else
    {
        if (m_renderSetting == All)
        {
             m_renderSetting = Diffuse;
             sizeTargets();
        }

        else if (m_renderSetting == Diffuse)
            m_renderSetting = Position;

        else if (m_renderSetting == Position)
            m_renderSetting = Voltage;

        else if (m_renderSetting == Voltage)
            m_renderSetting = Firing;

        else if (m_renderSetting == Firing)
        {
            m_renderSetting = All;
            sizeTargets();
        }
    }
}


inline GLuint SwapEndian(GLuint val)
{
   return (val<<24) | ((val<<8) & 0x00ff0000) |
          ((val>>8) & 0x0000ff00) | (val>>24);
}

void  NCV::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        // correct selection rectangle
        int width = abs(m_selectionRect.width());
        int height = abs(m_selectionRect.height());

        m_selectionRect.setLeft(max(0,min(m_selectionRect.left(),m_selectionRect.right())));
        m_selectionRect.setTop(max(0,min(m_selectionRect.top(),m_selectionRect.bottom())));
        if (m_renderSetting == All)
        {
            if (m_selectionRect.left() >( m_width / 2))
                m_selectionRect.setLeft(m_selectionRect.left() -  (m_width / 2));

            if (m_selectionRect.top() > (m_height / 2))
                m_selectionRect.setTop(m_selectionRect.top() -  (m_height / 2));

        }
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
        m_frameTargetBuffer.getTargetPixels("pick",m_selectionRect,objectID);

        GLubyte selected = 1;
        GLubyte deselected = 0;

        QSet<GLuint> selectedObjects;
        m_textureBuffers["Inst_Selection"].bind();
        // if not performing compound selection, deselect previously selected items
        if (!m_shiftDown )
        {
            for (int i =0; i < m_selectionDomain.size(); i ++)
            {
                int index = m_startSelectionID + i-1;
                if (m_selectionDomain[i] >0)
                    m_textureBuffers["Inst_Selection"].write(index* sizeof(GLubyte),&deselected,sizeof(GLubyte));
            }
            m_startSelectionID = -1;
            m_selectionDomain.clear();
        }
        // compile new selections into a unique set
        for (int i =0; i < m_selectionRect.width() *  m_selectionRect.height(); i ++)
            if (objectID[i] != 0)
                selectedObjects.insert(objectID[i]);

        // compile these selections into a vector, padding locations in between with deselections
        for (QSet<GLuint>::iterator it = selectedObjects.begin(); it !=selectedObjects.end(); it++)
        {
            GLuint id = *it;
            if (m_startSelectionID == -1)
            {
                m_selectionDomain.push_back(selected);
                m_startSelectionID = id;
            }
            else if (id <m_startSelectionID)
            {

                while (id < (m_startSelectionID-1))
                {
                    m_selectionDomain.push_front(0);
                   m_startSelectionID --;
                }
                m_selectionDomain.push_front(1);
                m_startSelectionID --;

            }
            else if (id > (m_startSelectionID + m_selectionDomain.size()-1))
            {
                while (id > (m_startSelectionID + m_selectionDomain.size()))
                   m_selectionDomain.push_back(0);
                m_selectionDomain.push_back(1);
            }
            else
                m_selectionDomain[id - m_startSelectionID] = 1;
        }

        // update selection buffer memory
        if ( m_selectionDomain.size() > 0)
            m_textureBuffers["Inst_Selection"].write((m_startSelectionID -1)* sizeof(GLubyte),&m_selectionDomain[0],sizeof(GLubyte)*m_selectionDomain.size());

        // rebind selection to shaders
        m_connectionProgram.bind();
        int sLoc = m_connectionProgram.uniformLocation("Inst_Selection");
        m_connectionProgram.setUniformValue(sLoc, m_textureBuffers["Inst_Selection"].textureSlot());
        m_connectionProgram.release();

        m_neuronProgram.bind();
        int nLoc = m_neuronProgram.uniformLocation("Inst_Selection");
        m_neuronProgram.setUniformValue(nLoc, m_textureBuffers["Inst_Selection"].textureSlot());
        m_neuronProgram.release();

        m_textureBuffers["Inst_Selection"].release();
        m_leftMouseDown = false;
    }
    else if (e->button() == Qt::RightButton)
    {
        m_rightMouseDown = false;
    }
}


void NCV::keyPressEvent( QKeyEvent* e )
{

    switch ( e->key() )
    {
        case Qt::Key_Escape:
            QCoreApplication::instance()->quit();
            break;

        case Qt::Key_A:
            m_camera.pan(-m_moveSpeed,0.0f,0.0f);
            onViewChanged();
            break;

        case Qt::Key_D:
            m_camera.pan(m_moveSpeed,0.0f,0.0f);
            onViewChanged();
            break;


        case Qt::Key_R:
            m_camera.pan(0.0f,0.0f,m_moveSpeed/ 10.0f);
            onViewChanged();
            break;

        case Qt::Key_B:
            if (m_connectionSelectionState == None)
                m_connectionSelectionState = Regular;
            else if (m_connectionSelectionState == Regular)
                m_connectionSelectionState = Connectivity;
            else
                m_connectionSelectionState = None;
            m_connectionProgram.bind();
            m_connectionProgram.setUniformValue("SelectionState",m_connectionSelectionState);
            m_connectionProgram.release();
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
            m_camera.pan(0.0f,0.0f,m_moveSpeed/ 25.0f);
            onViewChanged();
            break;


        case Qt::Key_P:
            m_camera.pan(0.0f,0.0f,m_moveSpeed/ 25.0f);
            onViewChanged();
            break;

        case Qt::Key_V:
            m_camera.pan(0.0f,0.0f,-m_moveSpeed/ 10.0f);
            onViewChanged();
            break;

        case Qt::Key_W:
            m_camera.pan(0.0f,0.0f,m_moveSpeed);
            onViewChanged();
            break;

        case Qt::Key_S:
            m_camera.pan(0.0f,0.0f,-m_moveSpeed);
            onViewChanged();
            break;

        case Qt::Key_Q:
            m_camera.pan(0.0f,m_moveSpeed,0.0f);
            onViewChanged();
            break;

        case Qt::Key_E:
            m_camera.pan(0.0f,-m_moveSpeed,0.0f);
            onViewChanged();
            break;



        case Qt::Key_J:
            //m_octree.setLevels(m_octree.levels() +1);
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
    switch (e->key())
    {
    case Qt::Key_Shift:
        m_shiftDown = false;
        break;
    }

}

bool NCV::compileShaderProgram(QGLShaderProgram & program, const QString& vertexShaderPath,const QString& fragmentShaderPath )
{
    // First we load and compile the vertex shader->..
    bool success = program.addShaderFromSourceFile( QGLShader::Vertex, vertexShaderPath );
    if ( !success )
        qDebug() << program.log();

    // ...now the fragment shader->..
    success = program.addShaderFromSourceFile( QGLShader::Fragment, fragmentShaderPath );
    if ( !success )
        qDebug() << program.log();

    // ...and finally we link them to resolve any references.
    success = program.link();
    if ( !success )
        qDebug() << "Could not link shader program:" << program.log();
    return success;
}

bool NCV::compileShaderProgram(QGLShaderProgram & program, const QString& vertexShaderPath, const QString & geometryShaderPath, const QString& fragmentShaderPath )
{
    // First we load and compile the vertex shader->..
    bool success = program.addShaderFromSourceFile( QGLShader::Vertex, vertexShaderPath );
    if ( !success )
        qDebug() << program.log();

    // ...now the geometry shader->..
    success = program.addShaderFromSourceFile( QGLShader::Geometry, geometryShaderPath );
    if ( !success )
        qDebug() << program.log();

    // ...now the fragment shader->..
    success = program.addShaderFromSourceFile( QGLShader::Fragment, fragmentShaderPath );
    if ( !success )
        qDebug() << program.log();


    // ...and finally we link them to resolve any references.
    success = program.link();
    if ( !success )
        qDebug() << "Could not link shader program:" << program.log();
    return success;
}

void NCV::createNeurons(int number,QVector3D * positions )
{
    m_neuronsToCreate = number;
    m_attributesToCreate["Inst_Translation"] = DataSet(positions,GL_FLOAT,3,Shared,DataSet::Neuron,1);

    establishWorldBounds(number,positions);

}
void NCV::createNeurons(const QString & filename )
{
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

    establishWorldBounds(m_neuronsToCreate,positions);
}

void NCV::createConnections(int number,GLuint * inNeurons,GLuint * outNeurons )
{
    m_connectionsToCreate = number;
    m_attributesToCreate["Inst_Neuron_IN"] = DataSet(inNeurons,GL_FLOAT,1,ConnectionOnly,DataSet::Connection,1);
    m_attributesToCreate["Inst_Neuron_OUT"] = DataSet(outNeurons,GL_FLOAT,1,ConnectionOnly,DataSet::Connection,1);
}
void NCV::createConnections(const QString & filename )
{
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


    m_attributesToCreate["Inst_Neuron_IN"] = DataSet(inNeurons,GL_FLOAT,1,ConnectionOnly,DataSet::Connection,1);
    m_attributesToCreate["Inst_Neuron_OUT"] = DataSet(outNeurons,GL_FLOAT,1,ConnectionOnly,DataSet::Connection,1);
}


void NCV::setConnectionAttributeArray(QString name, void * data,  GLenum componentType, int tupleSize, AttributeAccess access )
{
    if(componentType == GL_UNSIGNED_INT && access != Shared)
        componentType == GL_FLOAT;
    m_attributesToCreate[name] = DataSet(data,componentType,tupleSize,access,DataSet::Connection,1);
}

void NCV::setNeuronAttributeArray(QString name, void * data, GLenum componentType,  int tupleSize,AttributeAccess access)
{
    if(componentType == GL_UNSIGNED_INT && access != Shared)
        componentType == GL_FLOAT;
    m_attributesToCreate[name] = DataSet(data,componentType,tupleSize,access,DataSet::Neuron,1);
}


void NCV::setCompoundAttributeArray(QString name, void * data,GLenum componentType, int tupleSize, AttributeAccess access )
{
    if(componentType == GL_UNSIGNED_INT && access != Shared)
        componentType == GL_FLOAT;
    m_attributesToCreate[name] = DataSet(data,componentType,tupleSize,access,DataSet::Compound,1);
}

void NCV::setVisualizationParameter(const char * name, GLint value, AttributeAccess access)
{
    m_intParametersToCreate[name] = Parameter<GLint>(value,access);
    m_newVisualizationParameters = true;
}
void NCV::setVisualizationParameter(const char * name, QVector2D value, AttributeAccess access)
{
    m_vector2DParametersToCreate[name] = Parameter<QVector2D>(value,access);
    m_newVisualizationParameters = true;
}

void NCV::setVisualizationParameter(const char * name, QVector3D value, AttributeAccess access)
{
    m_vector3DParametersToCreate[name] = Parameter<QVector3D>(value,access);
    m_newVisualizationParameters = true;
}

void NCV::setVisualizationParameter(const char * name,  QMatrix4x4 value, AttributeAccess access)
{
    m_mat4x4ParametersToCreate[name] = Parameter<QMatrix4x4>(value,access);
    m_newVisualizationParameters = true;
}

void NCV::setVisualizationParameter(const char * name,GLuint value, AttributeAccess access)
{
    m_uintParametersToCreate[name] = Parameter<GLuint>(value,access);
    m_newVisualizationParameters = true;
}

void NCV::setVisualizationParameter(const char * name,GLfloat value, AttributeAccess access)
{
    m_floatParametersToCreate[name] = Parameter<GLfloat>(value,access);
    m_newVisualizationParameters = true;
}

void NCV::setVisualizationParameter(const char * name,GLubyte value, AttributeAccess access)
{
    m_ubyteParametersToCreate[name] = Parameter<GLubyte>(value,access);
    m_newVisualizationParameters = true;
}
void NCV::setVisualizationParameters()
{
    for (QMap<const char *,Parameter<GLubyte> >::iterator it = m_ubyteParametersToCreate.begin(); it != m_ubyteParametersToCreate.end(); it ++)
    {
        if (it.value().access != ConnectionOnly)
        {
            m_neuronProgram.bind();
            m_neuronProgram.setUniformValue(it.key(),it.value().value);
            m_neuronProgram.release();
        }
        if (it.value().access != NeuronOnly)
        {
            m_connectionProgram.bind();
            m_connectionProgram.setUniformValue(it.key(),it.value().value);
            m_connectionProgram.release();
        }
    }

    for (QMap<const char *,Parameter<GLfloat> >::iterator it = m_floatParametersToCreate.begin(); it != m_floatParametersToCreate.end(); it ++)
    {
        if (it.value().access != ConnectionOnly)
        {
            m_neuronProgram.bind();
            m_neuronProgram.setUniformValue(it.key(),it.value().value);
            m_neuronProgram.release();
        }
        if (it.value().access != NeuronOnly)
        {
            m_connectionProgram.bind();
            m_connectionProgram.setUniformValue(it.key(),it.value().value);
            m_connectionProgram.release();
        }
    }

    for (QMap<const char *,Parameter<GLint> >::iterator it = m_intParametersToCreate.begin(); it != m_intParametersToCreate.end(); it ++)
    {
        if (it.value().access != ConnectionOnly)
        {
            m_neuronProgram.bind();
            m_neuronProgram.setUniformValue(it.key(),it.value().value);
            m_neuronProgram.release();
        }
        if (it.value().access != NeuronOnly)
        {
            m_connectionProgram.bind();
            m_connectionProgram.setUniformValue(it.key(),it.value().value);
            m_connectionProgram.release();
        }
    }

    for (QMap<const char *,Parameter<GLuint> >::iterator it = m_uintParametersToCreate.begin(); it != m_uintParametersToCreate.end(); it ++)
    {
        if (it.value().access != ConnectionOnly)
        {
            m_neuronProgram.bind();
            m_neuronProgram.setUniformValue(it.key(),it.value().value);
            m_neuronProgram.release();
        }
        if (it.value().access != NeuronOnly)
        {
            m_connectionProgram.bind();
            m_connectionProgram.setUniformValue(it.key(),it.value().value);
            m_connectionProgram.release();
        }
    }

    for (QMap<const char *,Parameter<QVector2D> >::iterator it = m_vector2DParametersToCreate.begin(); it != m_vector2DParametersToCreate.end(); it ++)
    {
        if (it.value().access != ConnectionOnly)
        {
            m_neuronProgram.bind();
            m_neuronProgram.setUniformValue(it.key(),it.value().value);
            m_neuronProgram.release();
        }
        if (it.value().access != NeuronOnly)
        {
            m_connectionProgram.bind();
            m_connectionProgram.setUniformValue(it.key(),it.value().value);
            m_connectionProgram.release();
        }
    }

    for (QMap<const char *,Parameter<QVector3D> >::iterator it = m_vector3DParametersToCreate.begin(); it != m_vector3DParametersToCreate.end(); it ++)
    {
        if (it.value().access != ConnectionOnly)
        {
            m_neuronProgram.bind();
            m_neuronProgram.setUniformValue(it.key(),it.value().value);
            m_neuronProgram.release();
        }
        if (it.value().access != NeuronOnly)
        {
            m_connectionProgram.bind();
            m_connectionProgram.setUniformValue(it.key(),it.value().value);
            m_connectionProgram.release();
        }
    }

    for (QMap<const char *,Parameter<QMatrix4x4> >::iterator it = m_mat4x4ParametersToCreate.begin(); it != m_mat4x4ParametersToCreate.end(); it ++)
    {
        if (it.value().access != ConnectionOnly)
        {
            m_neuronProgram.bind();
            m_neuronProgram.setUniformValue(it.key(),it.value().value);
            m_neuronProgram.release();
        }
        if (it.value().access != NeuronOnly)
        {
            m_connectionProgram.bind();
            m_connectionProgram.setUniformValue(it.key(),it.value().value);
            m_connectionProgram.release();
        }
    }
    m_newVisualizationParameters = false;
}
