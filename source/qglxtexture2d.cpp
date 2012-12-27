#include "qglxtexture2d.h"
const unsigned int NOT_CREATED = 99999;

QGLXTexture2D::QGLXTexture2D()
    :QGLXTexture()
{
    m_id = NOT_CREATED;
}
void QGLXTexture2D::bind()
{
    glBindTexture(GL_TEXTURE_2D,m_id);
}


void QGLXTexture2D::bindAsFrameBufferTexture(FrameBufferTarget target, FrameBufferAttachment attachment)
{
    bind();
    glFramebufferTexture2D(target, attachment, GL_TEXTURE_2D, m_id, 0);
}

void QGLXTexture2D::bindAsImageTexture(QGLXTexture2D::ImageUnit unit,  QGLXTexture2D::ImageTextureAccess access,GLuint level)
{
    glBindTexture(GL_TEXTURE_2D,m_id);
    glBindImageTexture(unit, m_id, level, false, 0, access,m_internalFormat);

}

void QGLXTexture2D::create()
{
    glGenTextures(1, &m_id);
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    release();
}

bool QGLXTexture2D::isCreated()
{
    return m_id != NOT_CREATED;
}

void QGLXTexture2D::setMagFilter(Filter filter)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
}
void QGLXTexture2D::setMinFilter(Filter filter)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
}

void QGLXTexture2D::setWrapFunction(WrapFunction onWidth, WrapFunction onHeight)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, onWidth);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, onHeight);
}

void QGLXTexture2D::allocate(GLuint width, GLuint height,GLenum internalFormat, GLvoid * data)
{
    m_width = width;
    m_height = height;
    m_internalFormat = internalFormat;
    m_pixelType = internalFormatToPixelType(m_internalFormat);
    m_pixelFormat = internalFormatToPixelFormat(m_internalFormat);

    glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width,m_height,0, m_pixelFormat, GL_FLOAT, 0);


}
void QGLXTexture2D::destroy()
{
    glDeleteTextures(1,&m_id);
}

GLuint QGLXTexture2D::id()
{
    return m_id;
}
GLenum QGLXTexture2D::internalFormat()
{
    return m_internalFormat;
}
GLenum QGLXTexture2D::pixelFormat()
{
    return m_pixelFormat;
}
GLenum QGLXTexture2D::pixelType()
{
    return m_pixelType;
}
void QGLXTexture2D::release()
{
    glBindTexture(GL_TEXTURE_2D,0);
}


void QGLXTexture2D::setSize(GLuint width,GLuint height )
{
    m_width = width;
    m_height = height;

    glBindTexture(GL_TEXTURE_2D,m_id);
    glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width,m_height, 0, m_pixelFormat, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D,0);

}

GLuint QGLXTexture2D::width()
{
    return m_width;
}


GLuint QGLXTexture2D::height()
{
    return m_height;
}
