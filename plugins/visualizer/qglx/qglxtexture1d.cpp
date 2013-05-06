#include "qglxtexture1d.h"
const unsigned int NOT_CREATED = 99999;

QGLXTexture1D::QGLXTexture1D()
    :QGLXTexture()
{
    m_id = NOT_CREATED;
}
void QGLXTexture1D::bind()
{
    glBindTexture(GL_TEXTURE_1D,m_id);
}


void QGLXTexture1D::bind(FrameBufferTarget target, FrameBufferAttachment attachment)
{
    glBindTexture(GL_TEXTURE_1D,m_id);
    glFramebufferTexture1D(target, attachment, GL_TEXTURE_1D, m_id, 0);
}
void QGLXTexture1D::bind(QGLXTexture1D::ImageUnit unit,  QGLXTexture1D::ImageTextureAccess access,GLuint level)
{

    glBindTexture(GL_TEXTURE_1D,m_id);
    glBindImageTexture(unit, m_id, level, false, 0, access,m_internalFormat);

}

void QGLXTexture1D::create()
{
    glGenTextures(1, &m_id);
    bind();
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_CLAMP);
    release();
}


bool QGLXTexture1D::isCreated()
{
    return m_id != NOT_CREATED;
}

void QGLXTexture1D::setMagFilter(Filter filter)
{
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, filter);
}
void QGLXTexture1D::setMinFilter(Filter filter)
{
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, filter);
}

void QGLXTexture1D::setWrapFunction(WrapFunction onWidth)
{
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, onWidth);
}

void QGLXTexture1D::allocate(GLuint width, GLenum internalFormat, GLvoid * data)
{
    m_width = width;
    m_internalFormat = internalFormat;
    m_pixelType = internalFormatToPixelType(m_internalFormat);
    m_pixelFormat = internalFormatToPixelFormat(m_internalFormat);

    glBindTexture(GL_TEXTURE_1D,m_id);
    glTexImage1D(GL_TEXTURE_1D, 0, m_internalFormat, m_width, 0, m_pixelFormat, GL_FLOAT, data);
    glBindTexture(GL_TEXTURE_1D,0);

}
void QGLXTexture1D::destroy()
{
    glDeleteTextures(1,&m_id);
}

GLuint QGLXTexture1D::id()
{
    return m_id;
}
GLenum QGLXTexture1D::internalFormat()
{
    return m_internalFormat;
}
GLenum QGLXTexture1D::pixelFormat()
{
    return m_pixelFormat;
}
GLenum QGLXTexture1D::pixelType()
{
    return m_pixelType;
}
void QGLXTexture1D::release()
{
    glBindTexture(GL_TEXTURE_1D,0);
}




void QGLXTexture1D::setSize(GLuint width)
{
    m_width = width;

    glBindTexture(GL_TEXTURE_1D,m_id);
    glTexImage1D(GL_TEXTURE_1D, 0, m_internalFormat, m_width, 0, m_pixelFormat, m_pixelType, 0);
    glBindTexture(GL_TEXTURE_1D,0);

}
GLuint QGLXTexture1D::width()
{
    return m_width;
}

void QGLXTexture1D::bind(GLuint textureUnit)
{
    glActiveTexture(GL_TEXTURE0 +textureUnit);
    bind();
}
