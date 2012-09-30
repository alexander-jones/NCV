#include "glwidget.h"
#include <QTimer>


GLWidget::GLWidget( const QGLFormat& format, QWidget* parent )
    : QGLWidget( format, parent ),
      m_vertexBuffer( QGLBuffer::VertexBuffer )
{
    camera = Camera();
    m_picker = new Picker();
    m_vertexBuffer = QGLBuffer(QGLBuffer::VertexBuffer);

    mouseDown = false;
    mouseInitialized = false;
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
    QGLFormat glFormat = QGLWidget::format();
    if ( !glFormat.sampleBuffers() )
        qWarning() << "Could not enable sample buffers";

    // Set the clear color to black
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    // We need us some vertex data. Start simple with a triangle ;-)
    BufferData points[] = { {{-0.5f, -0.5f, 1.0f},1},
                            {{0.5f, -0.5f, 1.0f},1},
                            {{0.0f,  0.5f, 1.0f},1},
                            {{-1.5f, -1.5f, 12.0f},2},
                            {{-0.5f, -1.5f, 12.0f},2},
                            {{-1.0f,  -0.5f, 12.0f},2}};
    m_vertexBuffer.create();
    //m_vertexBuffer.setUsagePattern( QGLBuffer::StaticDraw );
    if ( !m_vertexBuffer.bind() )
    {
        qWarning() << "Could not bind vertex buffer to the context";
        return;
    }
    m_vertexBuffer.allocate( points, 6 * sizeof( BufferData ));
    m_picker->Init();

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


    // Enable the "vertex" attribute to bind it to our currently bound
    // vertex buffer.

    m_program->setAttributeBuffer( "vertex", GL_FLOAT, 0, 3,  sizeof( BufferData ) );
    m_program->enableAttributeArray( "vertex" );

    m_program->setAttributeBuffer( "id", GL_FLOAT,3 * sizeof( float ) ,1, sizeof( BufferData ));
    m_program->enableAttributeArray( "id" );
    m_program->setUniformValue("World",QMatrix4x4());
    this->startTimer(15);
    glEnable(GL_DEPTH_TEST);
}

void GLWidget::timerEvent(QTimerEvent * e)
{
    paintGL();
}


void GLWidget::resizeGL( int w, int h )
{
    // Set the viewport to window dimensions
    glViewport( 0, 0, w, qMax( h, 1 ) );
    camera.SetAspectRatio((float)w / (float)h);
    m_picker->Size(w,h);
}

void GLWidget::paintGL()
{
    QMatrix4x4 WVP = camera.GetProjectionMatrix()* camera.GetViewMatrix() * QMatrix4x4();
    // draw pick map
   m_picker->Start(WVP);
   glDrawArrays( GL_TRIANGLES, 0, 6 );
   m_picker->End();

    // draw scene

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    m_program->bind();
    m_program->setUniformValue("View",camera.GetViewMatrix());
    m_program->setUniformValue("Projection",camera.GetProjectionMatrix());
    m_program->setUniformValue("WVP",WVP);



    // Draw stuff
    glDrawArrays( GL_TRIANGLES, 0, 6 );


    this->swapBuffers();

}


void GLWidget::mouseMoveEvent(QMouseEvent* e)
{
    QVector2D newPos = QVector2D(e->x(),e->y());

    if (mouseDown)
    {
        if (mouseInitialized )
        {
            camera.Rotate((float)(newPos.x() - mousePosition.x()) * -turnSpeed,(float)(newPos.y() - mousePosition.y()) * -turnSpeed);
        }
        mousePosition = newPos;
        mouseInitialized = true;
    }
}

void GLWidget::mousePressEvent(QMouseEvent* e)
{

    int query = m_picker->PickAt(e->x(),e->y());
    m_program->setUniformValue("highlightID",query);
    mouseDown = true;
}

void  GLWidget::mouseReleaseEvent(QMouseEvent* e)
{
    mouseDown = false;
    mouseInitialized = false;
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



