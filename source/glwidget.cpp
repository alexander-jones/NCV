#include "glwidget.h"
#include <QTimer>


GLWidget::GLWidget( const QGLFormat& format, QWidget* parent )
    : QGLWidget( format, parent ),
      m_vertexBuffer( QGLBuffer::VertexBuffer )
{
    camera = Camera();
    m_vertexBuffer = QGLBuffer(QGLBuffer::VertexBuffer);
    leftMouseDown = false;
    rightMouseDown = false;
    m_fbo = new MultipleTargetFrameBuffer();
    mouseMovementInitialized = false;
    moveSpeed = 0.05;
    turnSpeed = 0.005f;
    setFocus();
}

GLWidget::~GLWidget()
{
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


    // initialize target buffer
    m_fbo->Init();
    m_fbo->AddTarget("color",GL_RGB8,GL_RGB,1320,620);
    m_fbo->AddTarget("pick",GL_RGB8,GL_RGB,1320,620);
    m_fbo->AddTarget("depth",GL_DEPTH_COMPONENT16,GL_DEPTH_COMPONENT,1320,620);



    // We need us some vertex data. Start simple with a triangle ;-)
    BufferData points[] = { {{-0.5f, -0.5f, 1.0f},1.0f},
                            {{0.5f, -0.5f, 1.0f},1.0f},
                            {{0.0f,  0.5f, 1.0f},1.0f},
                            {{-1.5f, -1.5f, 12.0f},2.0f},
                            {{-0.5f, -1.5f, 12.0f},2.0f},
                            {{-1.0f,  -0.5f, 12.0f},2.0f}};

    // create buffer to hold vertex data
    m_vertexBuffer.create();
    m_vertexBuffer.setUsagePattern( QGLBuffer::StaticDraw );
    if ( !m_vertexBuffer.bind() )
    {
        qWarning() << "Could not bind vertex buffer to the context";
        return;
    }
    m_vertexBuffer.allocate( points, 6 * sizeof( BufferData ));


    // compile shader
    m_program = new QGLShaderProgram();
    if (!GLHelper::CompileShaderProgram(m_program,":/simple.vert",":/simple.frag"))
        return;
    // Bind the shader program so that we can associate variables from
    // our application to the shaders
    if ( !m_program->bind() )
    {
        qDebug() << "Could not bind shader program to context";
        return;
    }


    // bind per-vertex attributes
    m_program->setAttributeBuffer( "vertex", GL_FLOAT, 0, 3,  sizeof( BufferData ) );
    m_program->enableAttributeArray( "vertex" );

    m_program->setAttributeBuffer( "id", GL_FLOAT,3* sizeof( float )  ,1, sizeof( BufferData ));
    m_program->enableAttributeArray( "id" );
    m_program->setUniformValue("World",QMatrix4x4());

    // start refresh rate timer
    this->startTimer(15);

    // Enable the "vertex" attribute to bind it to our currently bound
    // vertex buffer.


    /*
glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[WVP_MAT_VB]);

    for (unsigned int i = 0; i < 4 ; i++) {
        glEnableVertexAttribArray(WVP_LOCATION + i);
        glVertexAttribPointer(WVP_LOCATION + i, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4f),
                                (const GLvoid*)(sizeof(GLfloat) * i * 4));
        glVertexAttribDivisor(WVP_LOCATION + i, 1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[WORLD_MAT_VB]);

    for (unsigned int i = 0; i < 4 ; i++) {
        glEnableVertexAttribArray(WORLD_LOCATION + i);
        glVertexAttribPointer(WORLD_LOCATION + i, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4f),
                                (const GLvoid*)(sizeof(GLfloat) * i * 4));
        glVertexAttribDivisor(WORLD_LOCATION + i, 1);
    } */

}

void GLWidget::timerEvent(QTimerEvent * e)
{
    // perform refresh
    paintGL();

}


void GLWidget::resizeGL( int w, int h )
{
    // Set the viewport to window dimensions
    m_width = w;
    m_height = h;
    glViewport( 0, 0, w, qMax( h, 1 ) );
    camera.SetAspectRatio((float)w / (float)h);

    // resize targets

    m_fbo->SizeTargets(w,h);
}

void GLWidget::paintGL()
{
    QMatrix4x4 WVP = camera.GetProjectionMatrix()* camera.GetViewMatrix() * QMatrix4x4();

    // bind multiple target buffer
    m_fbo->Bind();

    // clear screen
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // bind shader program and current parameters
    m_program->bind();
    m_program->setUniformValue("View",camera.GetViewMatrix());
    m_program->setUniformValue("Projection",camera.GetProjectionMatrix());
    m_program->setUniformValue("WVP",WVP);
    m_program->enableAttributeArray( "vertex" );
    m_program->enableAttributeArray( "id" );

    // Draw stuff
    glDrawArrays( GL_TRIANGLES, 0, 6 );

    // release frame buffer and render to back buffer
    m_fbo->Release();
    if (rightMouseDown)
        m_fbo->TargetToScreen("pick",0,0,m_width,m_height);
    else
        m_fbo->TargetToScreen("color",0,0,m_width,m_height);

    this->swapBuffers();

    /*All semantics that are needed used:d
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[WVP_MAT_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Matrix4f) * NumInstances, WVPMats, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[WORLD_MAT_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Matrix4f) * NumInstances, WorldMats, GL_DYNAMIC_DRAW);


    for (each object) :

        glDrawElementsInstanced(GL_TRIANGLES,
                                         m_Entries[i].NumIndices,
                                         GL_UNSIGNED_INT,
                                         NumInstances;
    indices),
*/
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
    }
    // if left mouse down, pick / select objects
    else if (leftMouseDown)
    {

    }
}

void GLWidget::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
        leftMouseDown = true;

    else if (e->button() == Qt::RightButton)
        rightMouseDown = true;

    QColor pixel = m_fbo->GetTargetPixel("pick",e->x(),e->y());
    GLfloat id = pixel.red() + pixel.green()  * 256.0 + pixel.blue()  * 256.0 * 256.0 ;
    m_program->setUniformValue("highlightID",id);
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
            break;

        case Qt::Key_D:
            camera.Strafe(moveSpeed,0.0f,0.0f);
            break;


        case Qt::Key_W:
            camera.Strafe(0.0f,moveSpeed,0.0f);
            break;

        case Qt::Key_S:
            camera.Strafe(0.0f,-moveSpeed,0.0f);
            break;

        case Qt::Key_Q:
            camera.Strafe(0.0f,0.0f,moveSpeed);
            break;

        case Qt::Key_E:
            camera.Strafe(0.0f,0.0f,-moveSpeed);
            break;


        default:
            QGLWidget::keyPressEvent( e );
    }

}



