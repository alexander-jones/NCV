
#include "qglxbuffer.h"
#include <QDebug>
QGLXBuffer::QGLXBuffer(QGLXBuffer::Type type) :
    QGLBuffer((QGLBuffer::Type)type)
{
    if (type == QGLXBuffer::TextureBuffer)
        m_type = type;

}
bool QGLXBuffer::create()
{
    if (m_type == TextureBuffer)
    {
        glGenBuffers(1, &m_tbo);
        glGenTextures(1, &m_tex);
        return true;
    }
    else
        return QGLBuffer::create();

}
void QGLXBuffer::destroy()
{
    if (m_type == TextureBuffer)
    {
        glDeleteBuffers(1, &m_tbo);
        glDeleteTextures(1, &m_tex);
    }
    else
        QGLBuffer::destroy();

}
void QGLXBuffer::allocate(const void *data, int count,GLenum m_dataType )
{
    if (m_type == TextureBuffer)
    {
        glBindBuffer(GL_TEXTURE_BUFFER, m_tbo);
        glBufferData(GL_TEXTURE_BUFFER, count , data, usagePattern());
        //glBufferSubData(GL_TEXTURE_BUFFER,0,count,data);

        glBindTexture(GL_TEXTURE_BUFFER, m_tex);
        glTexBuffer(GL_TEXTURE_BUFFER, m_dataType, m_tbo);
        //glTexBufferRange(GL_TEXTURE_BUFFER,m_dataType,m_tbo,0,count);

    }
    else
        QGLBuffer::allocate(data,count);

}
GLint QGLXBuffer::textureID()
{
    if (m_type == TextureBuffer)
        return m_tex;
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
        glBindBuffer(GL_TEXTURE_BUFFER, m_tbo);
        glBindTexture(GL_TEXTURE_BUFFER, m_tex);
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
        glBindBuffer(GL_TEXTURE_BUFFER, m_tbo);
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
        glTexBuffer(GL_TEXTURE_BUFFER, m_dataType, m_tbo);
    }
    else
        QGLBuffer::write(offset,data,count);
}
