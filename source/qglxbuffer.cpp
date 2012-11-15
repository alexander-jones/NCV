
#include "qglxbuffer.h"
#include <QDebug>

QGLXBuffer::QGLXBuffer() :
    QGLBuffer()
{
    m_type = QGLXBuffer::VertexBuffer;
    m_isQGLX= false;

}

QGLXBuffer::QGLXBuffer(QGLXBuffer::Type type) :
    QGLBuffer((QGLBuffer::Type)type)
{
    m_type = type;
    if (type == QGLXBuffer::TextureBuffer || type == QGLXBuffer::AtomicCounterBuffer || type == QGLXBuffer::TransformFeedbackBuffer)
       m_isQGLX= true;
    else
       m_isQGLX= false;

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
void QGLXBuffer::allocate(const void *data, int count,GLenum m_dataType )
{
    if (m_type == TextureBuffer)
    {
        glBindBuffer(GL_TEXTURE_BUFFER, m_bufferID);
        glBufferData(GL_TEXTURE_BUFFER, count , data, usagePattern());

        glBindTexture(GL_TEXTURE_BUFFER, m_textureID);
        glTexBuffer(GL_TEXTURE_BUFFER, m_dataType, m_bufferID);

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

GLint QGLXBuffer::textureSlot()
{
    if (m_type == TextureBuffer)
        return m_textureSlot -  GL_TEXTURE0;
    else
        return -1;
}

void QGLXBuffer::setTextureSlot(GLuint slot)
{
    if (m_type == TextureBuffer)
        m_textureSlot =  GL_TEXTURE0 + slot;

}

bool QGLXBuffer::bind()
{
    if (m_type == TextureBuffer)
    {
        glActiveTexture(m_textureSlot);
        glBindBuffer(GL_TEXTURE_BUFFER, m_bufferID);
        glBindTexture(GL_TEXTURE_BUFFER, m_textureID);
        return true;
    }
    else
        return QGLBuffer::bind();
}
void QGLXBuffer::release()
{
    if (m_type == TextureBuffer)
    {
        glBindBuffer(GL_TEXTURE_BUFFER, 0);
        glBindTexture(GL_TEXTURE_BUFFER, 0);
    }
    else
        QGLBuffer::release();
}

bool QGLXBuffer::read(int offset, void *data, int count)
{
    if (m_type == TextureBuffer)
    {
        glBindBuffer(GL_TEXTURE_BUFFER, m_bufferID);
        glGetBufferSubData(GL_TEXTURE_BUFFER,offset,count,data);
        glBindBuffer(GL_TEXTURE_BUFFER, 0);
    }
    else
        return QGLBuffer::read(offset,data,count);
}

void QGLXBuffer::write(int offset, void *data, int count)
{
    if (m_type == TextureBuffer)
    {
        glBufferSubData(GL_TEXTURE_BUFFER,offset,count,data);
        glTexBuffer(GL_TEXTURE_BUFFER, m_dataType, m_bufferID);
    }
    else
        QGLBuffer::write(offset,data,count);
}
