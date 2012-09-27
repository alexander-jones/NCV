#include "glwidget.h"

#include <QCoreApplication>
#include <QKeyEvent>
#include <QTimer>

GLWidget::GLWidget( const QGLFormat& format, QWidget* parent )
    : QGLWidget( format, parent ),
      m_vertexBuffer( QGLBuffer::VertexBuffer )
{
    camera = Camera();
    mouseDown = false;
    mouseInitialized = false;
    moveSpeed = 0.05f;
    turnSpeed = 0.005f;
}



void GLWidget::initializeGL()
{
    QGLFormat glFormat = QGLWidget::format();
    if ( !glFormat.sampleBuffers() )
        qWarning() << "Could not enable sample buffers";

    // Set the clear color to black
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    // Prepare a complete shader program...
    if ( !prepareShaderProgram( ":/simple.vert", ":/simple.frag" ) )
        return;

    // We need us some vertex data. Start simple with a triangle ;-)
    float points[] = { -0.5f, -0.5f, 5.0f, 1.0f,
                        0.5f, -0.5f, 5.0f, 1.0f,
                        0.0f,  0.5f, 5.0f, 1.0f };
    m_vertexBuffer.create();
    m_vertexBuffer.setUsagePattern( QGLBuffer::StaticDraw );
    if ( !m_vertexBuffer.bind() )
    {
        qWarning() << "Could not bind vertex buffer to the context";
        return;
    }
    m_vertexBuffer.allocate( points, 3 * 4 * sizeof( float ) );

    // Bind the shader program so that we can associate variables from
    // our application to the shaders
    if ( !m_shader.bind() )
    {
        qWarning() << "Could not bind shader program to context";
        return;
    }

    // initialize uniforms
    m_shader.setUniformValue("World",QMatrix4x4());
    m_shader.setUniformValue("View",camera.GetViewMatrix());
    m_shader.setUniformValue("Projection",camera.GetProjectionMatrix());
    m_shader.setUniformValue("WVP",camera.GetProjectionMatrix()* camera.GetViewMatrix() * QMatrix4x4());


    // Enable the "vertex" attribute to bind it to our currently bound
    // vertex buffer.
    m_shader.setAttributeBuffer( "vertex", GL_FLOAT, 0, 4 );
    m_shader.enableAttributeArray( "vertex" );
}

void GLWidget::resizeGL( int w, int h )
{
    // Set the viewport to window dimensions
    glViewport( 0, 0, w, qMax( h, 1 ) );
    camera.SetAspectRatio((float)w / (float)h);
    m_shader.setUniformValue("Projection",camera.GetProjectionMatrix());
    m_shader.setUniformValue("WVP",camera.GetProjectionMatrix()* camera.GetViewMatrix() * QMatrix4x4());
}

void GLWidget::paintGL()
{
    m_shader.bind();
    // Clear the buffer with the current clearing color
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Draw stuff
    glDrawArrays( GL_TRIANGLES, 0, 3 );
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
            m_shader.setUniformValue("View",camera.GetViewMatrix());
            m_shader.setUniformValue("WVP",camera.GetProjectionMatrix()* camera.GetViewMatrix() * QMatrix4x4());
            qDebug() << camera.GetViewMatrix();
            paintGL();
        }
        mousePosition = newPos;
        mouseInitialized = true;
    }
}

void GLWidget::mousePressEvent(QMouseEvent* e)
{
    mouseDown = true;
}

void  GLWidget::mouseReleaseEvent(QMouseEvent* e)
{
    mouseDown = false;
    mouseInitialized = false;
}


void GLWidget::keyPressEvent( QKeyEvent* e )
{
    bool rebindView = false;
    switch ( e->key() )
    {
        case Qt::Key_Escape:
            QCoreApplication::instance()->quit();
            break;

        case Qt::Key_A:
            camera.Strafe(-moveSpeed,0.0f,0.0f);
            rebindView = true;
            break;

        case Qt::Key_D:
            camera.Strafe(moveSpeed,0.0f,0.0f);
            rebindView = true;
            break;


        case Qt::Key_W:
            camera.Strafe(0.0f,moveSpeed,0.0f);
            rebindView = true;
            break;

        case Qt::Key_S:
            camera.Strafe(0.0f,-moveSpeed,0.0f);
            rebindView = true;
            break;

        case Qt::Key_Q:
            camera.Strafe(0.0f,0.0f,moveSpeed);
            rebindView = true;
            break;

        case Qt::Key_E:
            camera.Strafe(0.0f,0.0f,-moveSpeed);
            rebindView = true;
            break;


        default:
            QGLWidget::keyPressEvent( e );
    }
    if (rebindView)
    {
        m_shader.setUniformValue("View",camera.GetViewMatrix());
        m_shader.setUniformValue("WVP",camera.GetProjectionMatrix()* camera.GetViewMatrix() * QMatrix4x4());
        qDebug() << camera.GetViewMatrix();
        paintGL();
    }
}

bool GLWidget::prepareShaderProgram( const QString& vertexShaderPath,
                                     const QString& fragmentShaderPath )
{
    // First we load and compile the vertex shader...
    bool result = m_shader.addShaderFromSourceFile( QGLShader::Vertex, vertexShaderPath );
    if ( !result )
        qWarning() << m_shader.log();

    // ...now the fragment shader...
    result = m_shader.addShaderFromSourceFile( QGLShader::Fragment, fragmentShaderPath );
    if ( !result )
        qWarning() << m_shader.log();

    // ...and finally we link them to resolve any references.
    result = m_shader.link();
    if ( !result )
        qWarning() << "Could not link shader program:" << m_shader.log();

    return result;
}
