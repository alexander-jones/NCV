#include "glwidget.h"
#include <QTimer>

GLWidget::GLWidget( const QGLFormat& format, QWidget* parent )
    : QGLWidget( format, parent ),
      m_vertexBuffer( QGLBuffer::VertexBuffer )
{
    camera = Camera();
    m_vertexBuffer = QGLBuffer(QGLBuffer::VertexBuffer);
    m_instanceBuffer = QGLBuffer(QGLBuffer::VertexBuffer);
    m_indexBuffer = QGLBuffer(QGLBuffer::IndexBuffer);
    leftMouseDown = false;
    rightMouseDown = false;
    m_frameTargetBuffer =  MultipleTargetFrameBuffer();
    mouseMovementInitialized = false;
    moveSpeed = 50;
    turnSpeed = 0.005f;
    m_renderSetting = Diffuse;
    m_WorldSize = QVector3D(5000,5000,5000);
    setFocus();
}

GLWidget::~GLWidget()
{
}

void GLWidget::BindModel( QString filename)
{
    // load instance model ,do some post import optimizations
    const aiScene * model = m_importer.ReadFile(filename.toStdString(), aiProcess_JoinIdenticalVertices  | aiProcess_GenSmoothNormals
                                                | aiProcess_Triangulate|aiProcess_MakeLeftHanded  |aiProcess_FixInfacingNormals );

    // gather indices of all faces
    for (int i = 0; i < model->mMeshes[0]->mNumFaces; i ++)
        for (int j = 0; j < model->mMeshes[0]->mFaces[i].mNumIndices; j ++)
            m_indices.push_back(model->mMeshes[0]->mFaces[i].mIndices[j]);

    //fill attribute array
    m_attributes.clear();
    for (int i = 0; i < model->mMeshes[0]->mNumVertices; i ++)
    {
        m_attributes.push_back(VertexAtrtributes(model->mMeshes[0]->mVertices[i],model->mMeshes[0]->mNormals[i]));
    }

    // create buffers to bind mesh geometry
    m_vertexBuffer.create();
    m_indexBuffer.create();

    m_vertexBuffer.setUsagePattern( QGLBuffer::StaticDraw );
    m_indexBuffer.setUsagePattern( QGLBuffer::StaticDraw );

    if ( !m_vertexBuffer.bind() )
    {
        qDebug() << "Could not bind vertex buffer to the context";
        return;
    }

    if ( !m_indexBuffer.bind() )
    {
        qDebug() << "Could not bind index buffer to the context";
        return;
    }


    m_vertexBuffer.allocate( &m_attributes[0], m_attributes.size() * sizeof(VertexAtrtributes));
    m_indexBuffer.allocate( &(m_indices[0]),  m_indices.size() * sizeof( GLuint ));

    return;
}

QMatrix4x4 GLWidget::GetWVP()
{
    return camera.GetProjectionMatrix()* camera.GetViewMatrix() * QMatrix4x4();
}

void GLWidget::initializeGL()
{
    glewInit();

    QGLFormat glFormat = QGLWidget::format();
    if ( !glFormat.sampleBuffers() )
        qWarning() << "Could not enable sample buffers";


    // Set the clear color to black
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // initialize target buffer
    m_frameTargetBuffer.Init();
    m_frameTargetBuffer.AddTarget("diffuse",GL_RGB8,1000,1000,GL_COLOR_ATTACHMENT0);
    m_frameTargetBuffer.AddTarget("position",GL_RGB8,1000,1000,GL_COLOR_ATTACHMENT1);
    m_frameTargetBuffer.AddTarget("normal",GL_RGB8,1000,1000,GL_COLOR_ATTACHMENT2);
    m_frameTargetBuffer.AddTarget("pick",GL_RGB8,1000,1000,GL_COLOR_ATTACHMENT3);
    m_frameTargetBuffer.AddTarget("depth",GL_DEPTH_COMPONENT16,1000,1000,GL_DEPTH_ATTACHMENT);

    m_frameTargetBuffer.AddTarget("final",GL_RGB8,1000,1000,GL_COLOR_ATTACHMENT0);

    m_firstPassTargets[0] = "diffuse";
    m_firstPassTargets[1] = "position";
    m_firstPassTargets[2] = "normal";
    m_firstPassTargets[3] = "pick";

    // bind base geometry
    BindModel("../source/cube.nff");

    // fill world with instances
    int xDepth = 100, yDepth = 100, zDepth  =100;
    int spacing = 50;
    m_WorldSize = QVector3D(xDepth * spacing, yDepth * spacing, zDepth * spacing);

    for (int i = -xDepth/2; i < xDepth/2; i ++)
    {
        for (int j = -yDepth/2; j <yDepth/2; j ++)
        {
            for (int k = -zDepth/2; k < zDepth/2; k ++)
            {
                QMatrix4x4 a = QMatrix4x4();
                a.translate(i * spacing,j * spacing, k* spacing);
                qreal * root = a.data();

                // pack each instance matrix as 16 successive floats
                for (int offset = 0; offset < 16; offset ++)
                    m_modelMats.push_back((GLfloat)(*(root + offset)));
            }
        }
    }


    // compile shader
    if (!GLHelper::CompileShaderProgram(&m_firstPassProgram,":/multi_target.vert",":/multi_target.frag"))
        return;
    // Bind the shader program so that we can associate variables from
    // our application to the shaders
    if ( !m_firstPassProgram.bind() )
    {
        qDebug() << "Could not bind shader program to context";
        return;
    }

    // bind per-vertex attributes of model
    m_firstPassProgram.setAttributeBuffer( "position", GL_FLOAT, 0, 3,  6 * sizeof( float ) );
    m_firstPassProgram.enableAttributeArray( "position" );
    m_firstPassProgram.setAttributeBuffer( "normal", GL_FLOAT, 3 * sizeof( float ), 3,  6 * sizeof( float ) );
    m_firstPassProgram.enableAttributeArray( "normal" );

    // allocate memory for instances
    m_instanceBuffer.create();
    m_instanceBuffer.setUsagePattern( QGLBuffer::DynamicDraw );
    m_instanceBuffer.bind();
    m_instanceBuffer.allocate( &m_modelMats[0], m_modelMats.size() *sizeof( GLfloat));

    // attach instances to attribute in program
    GLuint mvpLoc = m_firstPassProgram.attributeLocation("Model");
    for (int i = 0; i < 4; i ++)
    {
        m_firstPassProgram.setAttributeBuffer( mvpLoc + i, GL_FLOAT,sizeof(GLfloat) * i * 4  ,4, sizeof( GLfloat) * 16);
        m_firstPassProgram.enableAttributeArray( mvpLoc + i );
        glVertexAttribDivisor(mvpLoc + i, 1);
    }


    // establish initial values for shader uniforms
    m_scale = QMatrix4x4();
    m_firstPassProgram.setUniformValue("Scale",m_scale);
    m_firstPassProgram.setUniformValue("View",camera.GetViewMatrix());
    m_firstPassProgram.setUniformValue("Projection",camera.GetProjectionMatrix());
    m_firstPassProgram.setUniformValue("WorldSize",m_WorldSize);

    // start refresh rate timer
    this->startTimer(5);

}


void GLWidget::timerEvent(QTimerEvent * e)
{
    // perform refresh
    paintGL();

    GLuint err = glGetError();
    if (err != 0)
        qDebug() <<err;

}


void GLWidget::resizeGL( int w, int h )
{
    // Set the viewport to window dimensions
    m_width = w;
    m_height = h;
    glViewport( 0, 0, w, qMax( h, 1 ) );
    camera.SetAspectRatio((float)w / (float)h);
    m_firstPassProgram.bind();
    m_firstPassProgram.setUniformValue("Projection",camera.GetProjectionMatrix());
    // resize targets
    m_frameTargetBuffer.SizeTargets(w,h);
}

void GLWidget::paintGL()
{

    // bind multiple target buffer
    m_frameTargetBuffer.BindTargets(4,m_firstPassTargets);
    m_firstPassProgram.bind();
    // clear screen
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glDrawElementsInstanced(GL_TRIANGLES,m_indices.size(),GL_UNSIGNED_INT,0,m_modelMats.size() / 16);

    // release frame buffer and render to back buffer
    m_frameTargetBuffer.Release();

    //m_frameTargetBuffer.BindTarget("final");
    if (m_renderSetting == Normal)
        m_frameTargetBuffer.TargetToScreen("normal", 0, 0,m_width,m_height);
    else  if (m_renderSetting == Position)
        m_frameTargetBuffer.TargetToScreen("position", 0, 0,m_width,m_height);

    else if (m_renderSetting == Pick)
        m_frameTargetBuffer.TargetToScreen("pick", 0, 0,m_width,m_height);

    else if (m_renderSetting == Diffuse)
        m_frameTargetBuffer.TargetToScreen("diffuse", 0, 0,m_width,m_height);

    this->swapBuffers();
}


void GLWidget::mouseMoveEvent(QMouseEvent* e)
{
    QVector2D newPos = QVector2D(e->x(),e->y());

    // if right mouse down rotate camera
    if (rightMouseDown)
    {
        if (mouseMovementInitialized )
            camera.Rotate((float)(newPos.x() - mousePosition.x()) * -turnSpeed,(float)(newPos.y() - mousePosition.y()) * -turnSpeed);

        mousePosition = newPos;
        mouseMovementInitialized = true;
        m_firstPassProgram.setUniformValue("View",camera.GetViewMatrix());
    }

}
void GLWidget::wheelEvent(QWheelEvent *e)
{
    if(e->orientation() == Qt::Vertical)
    {

        m_scale.scale((2000.0 - (double)e->delta()) / 2000.0);
        m_firstPassProgram.setUniformValue("Scale",m_scale);

    }
}
void GLWidget::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        leftMouseDown = true;
        QColor pixel = m_frameTargetBuffer.GetTargetPixel("pick",e->x(),e->y());
        GLint id = pixel.red() + pixel.green()  * 256.0f + pixel.blue()  * 256.0f * 256.0f ;
        //m_firstPassProgram.setUniformValue("SelectedID",id);
    }

    else if (e->button() == Qt::MidButton)
    {
        leftMouseDown = true;
        QColor pixel = m_frameTargetBuffer.GetTargetPixel("pick",e->x(),e->y());
        GLint id = pixel.red() + pixel.green()  * 256.0f + pixel.blue()  * 256.0f * 256.0f ;
        //m_firstPassProgram.setUniformValue("SelectedID",id);
    }


    else if (e->button() == Qt::RightButton)
        rightMouseDown = true;
}

void  GLWidget::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        leftMouseDown = false;
    }
    else if (e->button() == Qt::RightButton)
    {
        rightMouseDown = false;
        mouseMovementInitialized = false;
    }
}


void GLWidget::keyPressEvent( QKeyEvent* e )
{

    switch ( e->key() )
    {
        case Qt::Key_Escape:
            QCoreApplication::instance()->quit();
            break;

        case Qt::Key_A:
            camera.Strafe(-moveSpeed,0.0f,0.0f);
            m_firstPassProgram.setUniformValue("View",camera.GetViewMatrix());
            break;

        case Qt::Key_D:
            camera.Strafe(moveSpeed,0.0f,0.0f);
            m_firstPassProgram.setUniformValue("View",camera.GetViewMatrix());
            break;


        case Qt::Key_R:
            camera.Strafe(0.0f,moveSpeed/ 10.0f,0.0f);
            m_firstPassProgram.setUniformValue("View",camera.GetViewMatrix());
            break;

        case Qt::Key_V:
            camera.Strafe(0.0f,-moveSpeed / 10.0f,0.0f);
            m_firstPassProgram.setUniformValue("View",camera.GetViewMatrix());
            break;

        case Qt::Key_W:
            camera.Strafe(0.0f,moveSpeed,0.0f);
            m_firstPassProgram.setUniformValue("View",camera.GetViewMatrix());
            break;

        case Qt::Key_S:
            camera.Strafe(0.0f,-moveSpeed,0.0f);
            m_firstPassProgram.setUniformValue("View",camera.GetViewMatrix());
            break;

        case Qt::Key_Q:
            camera.Strafe(0.0f,0.0f,moveSpeed);
            m_firstPassProgram.setUniformValue("View",camera.GetViewMatrix());
            break;

        case Qt::Key_E:
            camera.Strafe(0.0f,0.0f,-moveSpeed);
            m_firstPassProgram.setUniformValue("View",camera.GetViewMatrix());
            break;


        case Qt::Key_1:
             m_renderSetting = Diffuse;
            break;

        case Qt::Key_2:
            m_renderSetting = Position;
            break;

        case Qt::Key_3:
            m_renderSetting = Normal;
            break;

        case Qt::Key_4:
            m_renderSetting =  Pick;
            break;

        default:
            QGLWidget::keyPressEvent( e );
    }
}
void GLWidget::keyReleaseEvent(QKeyEvent *e)
{

    switch ( e->key() )
    {


    }
}


