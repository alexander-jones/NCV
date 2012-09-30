#include "picker.h"
#include <QGLBuffer>

Picker::Picker( )
{
    // set format for picking frame buffer
    m_pickingFormat = QGLFramebufferObjectFormat();
    m_pickingFormat.setInternalTextureFormat(GL_RGB);
    m_pickingFormat.setTextureTarget(GL_TEXTURE_2D);
    m_pickingFormat.setAttachment(QGLFramebufferObject::Depth);

}

Picker::~Picker()
{
}

void Picker::Init()
{
    // compile picking shader program
    m_program = new QGLShaderProgram();
    GLHelper::CompileShaderProgram(m_program,":/pick.vert",":/pick.frag");
    if ( !m_program->bind() )
    {
        qDebug() << "Could not bind shader program to context";
        return;
    }

    // bind vertex attributes
    m_program->setAttributeBuffer( "vertex", GL_FLOAT, 0, 3,  sizeof( BufferData ) );
    m_program->enableAttributeArray( "vertex" );

    m_program->setAttributeBuffer( "id", GL_FLOAT,3 * sizeof( float ) ,1, sizeof( BufferData ));
    m_program->enableAttributeArray( "id" );
    glEnable(GL_DEPTH_TEST);

}
void Picker::Size(unsigned int WindowWidth, unsigned int WindowHeight)
{
    m_pickingTarget = new QGLFramebufferObject(WindowWidth,WindowHeight,m_pickingFormat);

}
void Picker::Start(QMatrix4x4 wvp)
{
    // start picking render
    m_pickingTarget->bind();

    if ( !m_program->bind() )
    {
        qDebug() << "Could not bind shader program to context";
        return;
    }
    m_program->enableAttributeArray( "vertex" );
    m_program->enableAttributeArray( "id" );
    m_program->setUniformValue("WVP",wvp);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void Picker::End()
{

    // end picking render, then get frame in more readable QImage format
   m_pickingTarget->release();
   m_pickingTexture = m_pickingTarget->toImage();
}

int Picker::PickAt(unsigned int x, unsigned int y)
{
    QColor pixel = m_pickingTexture.pixel(x,y);

    // bitshift RGBA pixel value into unique Entity ID (empty space returns 0)
    int id = pixel.red() + pixel.green()  * 256.0 + pixel.blue()  * 256.0 * 256.0 ;
    qDebug() <<id;
    return id;
}

