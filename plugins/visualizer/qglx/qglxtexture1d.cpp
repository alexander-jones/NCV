#include "qglxtexture1d.h"
const unsigned int NOT_CREATED = 0;
const GLuint INVALID_IMAGE_UNIT = -1;

QGLXTexture1D::QGLXTexture1D()
{
    m_id = NOT_CREATED;
    m_imageUnit = INVALID_IMAGE_UNIT;
}
void QGLXTexture1D::bind()
{
    glBindTexture(GL_TEXTURE_1D,m_id);
}


void QGLXTexture1D::bind(FrameBufferTarget target, FrameBufferAttachment attachment, GLuint offset)
{
    if (attachment != Color)
        offset = 0;

    glBindTexture(GL_TEXTURE_1D,m_id);
    glFramebufferTexture1D(target, attachment + offset, GL_TEXTURE_1D, m_id, 0);
}
void QGLXTexture1D::bind(GLuint unit,  Access access,GLuint level)
{
    glBindImageTexture(unit, m_id, level, false, 0, access,m_internalFormat);
    m_imageUnit = unit;

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
    if (filter == NearestMipmapNearest || filter == NearestMipmapLinear)
        filter = Nearest;
    else if (filter == LinearMipmapNearest || filter == LinearMipmapLinear)
        filter = Linear;

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
    if (m_imageUnit != INVALID_IMAGE_UNIT)
        glBindImageTexture(m_imageUnit, 0, 0, false, 0, GL_READ_ONLY,m_internalFormat);
    else
        glBindTexture(GL_TEXTURE_1D,0);

    m_imageUnit = INVALID_IMAGE_UNIT;
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
