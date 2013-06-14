#include "qglxbuffertexture.h"
#include <QDebug>

const unsigned int NOT_CREATED = 99999;

QGLXBufferTexture::QGLXBufferTexture()
    :QGLXTexture()
{
    m_id = NOT_CREATED;
}

void QGLXBufferTexture::attach(QGLXBuffer buffer, GLenum internalFormat)
{
    if (buffer.isCreated())
    {
        m_buffer = buffer;
        glBindTexture(GL_TEXTURE_BUFFER,m_id);
        glTexBuffer(GL_TEXTURE_BUFFER, internalFormat, buffer.id());
        glBindTexture(GL_TEXTURE_BUFFER,0);
    }
}


void QGLXBufferTexture::bind()
{
    glBindTexture(GL_TEXTURE_BUFFER,m_id);
}


void QGLXBufferTexture::create()
{
    glGenTextures(1, &m_id);
}


bool QGLXBufferTexture::isCreated()
{
    return m_id != NOT_CREATED;
}

void QGLXBufferTexture::destroy()
{
    glDeleteTextures(1,&m_id);
}

GLuint QGLXBufferTexture::id()
{
    return m_id;
}
GLenum QGLXBufferTexture::internalFormat()
{
    return m_internalFormat;
}

void QGLXBufferTexture::release()
{
    glBindTexture(GL_TEXTURE_BUFFER,0);
}

void QGLXBufferTexture::bind(GLuint textureUnit)
{
    glActiveTexture(GL_TEXTURE0 +textureUnit);
    bind();
}
