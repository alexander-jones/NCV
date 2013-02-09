#include "qglxbuffer.h"
#include <QDebug>

const unsigned int NOT_CREATED = 10000;
QGLXBuffer::QGLXBuffer() :
    QGLBuffer()
{
    m_type = QGLXBuffer::VertexBuffer;
    m_isQGLX= false;
    m_bufferID = NOT_CREATED;
    m_textureID = NOT_CREATED;

}



QGLXBuffer::QGLXBuffer(QGLXBuffer::Type type) :
    QGLBuffer((QGLBuffer::Type)type)
{
    m_type = type;
    if (m_type == QGLXBuffer::TextureBuffer || m_type == QGLXBuffer::AtomicCounterBuffer || m_type == QGLXBuffer::TransformFeedbackBuffer
        || m_type == QGLXBuffer::UniformBuffer  || m_type == QGLXBuffer::ShaderStorageBuffer  || m_type == QGLXBuffer::DispatchIndirectBuffer)
       m_isQGLX= true;
    else
       m_isQGLX= false;
    m_bufferID = NOT_CREATED;
    m_textureID = NOT_CREATED;

}
bool QGLXBuffer::create()
{
    if (m_isQGLX)
    {
        glGenBuffers(1, &m_bufferID);
        if (m_type == TextureBuffer)
            glGenTextures(1, &m_textureID);
        return true;
    }
    else
        return QGLBuffer::create();

}
void QGLXBuffer::destroy()
{
    if (m_isQGLX)
    {
        glDeleteBuffers(1, &m_bufferID);
        if (m_type == TextureBuffer)
            glDeleteTextures(1, &m_textureID);
    }
    else
        QGLBuffer::destroy();

}
void QGLXBuffer::allocate(const void *data, int count,GLenum dataType )
{
    if (m_isQGLX)
    {
        glBufferData(m_type, count , data, usagePattern());
        if (m_type == TextureBuffer)
            glTexBuffer(m_type, dataType, m_bufferID);

    }

    else
        QGLBuffer::allocate(data,count);

}
GLint QGLXBuffer::textureID()
{
    if (m_type == TextureBuffer)
        return m_textureID;
    else
        return -1;
}


bool QGLXBuffer::bind()
{
    if (m_isQGLX)
    {
        glGetError();
        glBindBuffer(m_type, m_bufferID);
        if (m_type == TextureBuffer)
            glBindTexture(m_type, m_textureID);
        GLenum after = glGetError();
        return after == 0;
    }
    else
        return QGLBuffer::bind();
}

void * QGLXBuffer::map(QGLXBuffer::Access access)
{
    if (m_isQGLX)
    {
        glBindBuffer(m_type, m_bufferID);
        return glMapBuffer(m_type,access);
    }
    else
        return QGLBuffer::map(access);
}

bool QGLXBuffer::isCreated()
{
    if (m_isQGLX)
    {
        return m_bufferID != NOT_CREATED;
    }
    else
        return QGLBuffer::isCreated();
}

void  QGLXBuffer::unmap()
{
    if (m_isQGLX)
    {
        glUnmapBuffer(m_type);
        glBindBuffer(m_type, 0);
    }
    else
        QGLBuffer::unmap();
}
void QGLXBuffer::release()
{
    if (m_isQGLX)
    {
        glBindBuffer(m_type, 0);
        if (m_type == TextureBuffer)
            glBindTexture(m_type, 0);
    }
    else
        QGLBuffer::release();
}

bool QGLXBuffer::read(int offset, void *data, int count)
{
    if (m_isQGLX)
    {
        GLenum before = glGetError();
        glBindBuffer(m_type, m_bufferID);
        glGetBufferSubData(m_type,offset,count,data);
        glBindBuffer(m_type, 0);
        GLenum after = glGetError();
        return after == 0 && before != 0;
    }
    else
        return QGLBuffer::read(offset,data,count);
}

void QGLXBuffer::write(int offset, void *data, int count)
{
    if (m_type == TextureBuffer)
    {
        glBufferSubData(m_type,offset,count,data);
        glTexBuffer(m_type, m_dataType, m_bufferID);
    }
    else
        QGLBuffer::write(offset,data,count);
}
