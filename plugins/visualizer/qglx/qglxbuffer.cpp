#include "qglxbuffer.h"
#include <QDebug>

const unsigned int NOT_CREATED = 10000;
QGLXBuffer::QGLXBuffer()
{
    m_bufferID = NOT_CREATED;
    m_textureID = NOT_CREATED;

}


bool QGLXBuffer::create()
{
    glGenBuffers(1, &m_bufferID);
    glGenTextures(1, &m_textureID);
    return true;

}
void QGLXBuffer::destroy()
{
    glDeleteBuffers(1, &m_bufferID);
    glDeleteTextures(1, &m_textureID);

}
void QGLXBuffer::allocate(const void *data, int count, UsagePattern usage )
{
    glBindBuffer(ArrayBuffer,m_bufferID);
    glBufferData(ArrayBuffer, count , data, usage);
    glBindBuffer(ArrayBuffer,0);
}

void QGLXBuffer::allocate(const void *data, int count,GLenum textureFormat , UsagePattern usage)
{
    m_textureFormat = textureFormat;
    glBindBuffer(TextureBuffer,m_bufferID);
    glBufferData(TextureBuffer, count , data, usage);
    glBindBuffer(TextureBuffer,0);

    glBindTexture(TextureBuffer,m_textureID);
    glTexBuffer(TextureBuffer, textureFormat, m_bufferID);
    glBindTexture(TextureBuffer,0);
}

GLuint QGLXBuffer::id()
{
    return m_bufferID;
}

GLint QGLXBuffer::textureID()
{

    if (m_targetBinding == TextureBuffer)
        return m_textureID;
    else
        return -1;
}


bool QGLXBuffer::bind(TargetBinding binding)
{
    m_targetBinding = binding;
    if (m_targetBinding == TextureBuffer)
        glBindTexture(m_targetBinding,m_textureID);

    else
        glBindBuffer(m_targetBinding, m_bufferID);


    return true;
}

void * QGLXBuffer::map(TargetBinding binding, QGLXBuffer::Access access)
{
    m_targetBinding = binding;
    glBindBuffer(m_targetBinding, m_bufferID);
    return glMapBuffer(m_targetBinding,access);
}

bool QGLXBuffer::isCreated()
{
    return m_bufferID != NOT_CREATED;
}

void  QGLXBuffer::unmap()
{
    if (m_targetBinding == Unbound)
        return;
    glUnmapBuffer(m_targetBinding);
    glBindBuffer(m_targetBinding, 0);

}
void QGLXBuffer::release()
{
    if (m_targetBinding == Unbound)
        return;
    glBindBuffer(m_targetBinding, 0);
    if (m_targetBinding == TextureBuffer)
        glBindTexture(m_targetBinding,0);
    m_targetBinding = Unbound;

}

bool QGLXBuffer::read(int offset, void *data, int count)
{
    GLenum before = glGetError();
    glBindBuffer(m_targetBinding, m_bufferID);
    glGetBufferSubData(m_targetBinding,offset,count,data);
    glBindBuffer(m_targetBinding, 0);
    GLenum after = glGetError();
    return after == 0 && before != 0;
}

void QGLXBuffer::write(int offset, void *data, int count)
{
    if (m_targetBinding == Unbound)
    glBufferSubData(m_targetBinding,offset,count,data);
    if (m_targetBinding == TextureBuffer)
        glTexBuffer(m_targetBinding, m_dataType, m_bufferID);
}
