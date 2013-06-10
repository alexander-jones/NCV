#include "qglxtexture2d.h"
const unsigned int NOT_CREATED = 0;
const GLuint INVALID_IMAGE_UNIT = -1;

QGLXTexture2D::QGLXTexture2D()
{
    m_id = NOT_CREATED;
    m_imageUnit = INVALID_IMAGE_UNIT;
}
void QGLXTexture2D::bind()
{
    glBindTexture(GL_TEXTURE_2D,m_id);
}


void QGLXTexture2D::bind(FrameBufferTarget target, FrameBufferAttachment attachment, GLuint offset)
{
    if (attachment != Color)
        offset = 0;

    bind();
    glFramebufferTexture2D(target, (GLenum)attachment + offset, GL_TEXTURE_2D, m_id, 0);
}

GLuint QGLXTexture2D::imageUnit()
{
    return m_imageUnit;
}
void QGLXTexture2D::bind(GLuint unit,  Access access,GLuint level)
{
    m_imageUnit = unit;
    glBindImageTexture(unit, m_id, level, false, 0, access,m_internalFormat);

}

void QGLXTexture2D::create( )
{
    glGenTextures(1, &m_id);
    bind();
    m_multisampled = false;
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
    if (filter == NearestMipmapNearest || filter == NearestMipmapLinear)
        filter = Nearest;
    else if (filter == LinearMipmapNearest || filter == LinearMipmapLinear)
        filter = Linear;

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

void QGLXTexture2D::allocate(GLuint width, GLuint height,GLenum internalFormat,int samplesPerPixel , GLvoid * data)
{

    m_width = width;
    m_height = height;
    m_internalFormat = internalFormat;
    m_pixelType = internalFormatToPixelType(m_internalFormat);
    m_pixelFormat = internalFormatToPixelFormat(m_internalFormat);


    m_samples = samplesPerPixel;
    if (m_samples == 1)
        m_multisampled = false;
    else
        m_multisampled = true;


    glBindTexture(GL_TEXTURE_2D,m_id);
    if (m_multisampled)
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,m_samples,m_internalFormat,m_width,m_height,GL_FALSE);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width,m_height,0, m_pixelFormat, m_pixelType, data);

    glBindTexture(GL_TEXTURE_2D,0);

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

    if (m_imageUnit != INVALID_IMAGE_UNIT)
        glBindImageTexture(m_imageUnit, 0, 0, false, 0, GL_READ_ONLY,m_internalFormat);
    else
        glBindTexture(GL_TEXTURE_2D,0);
}


void QGLXTexture2D::setSize(GLuint width,GLuint height )
{
    m_width = width;
    m_height = height;

    glBindTexture(GL_TEXTURE_2D,m_id);

    if (m_multisampled)
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,m_samples,m_internalFormat,m_width,m_height,GL_FALSE);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width,m_height, 0, m_pixelFormat, m_pixelType, 0);
    glBindTexture(GL_TEXTURE_2D,0);

}

QSize QGLXTexture2D::size()
{
    return QSize(m_width,m_height);
}

GLuint QGLXTexture2D::width()
{
    return m_width;
}


GLuint QGLXTexture2D::height()
{
    return m_height;
}

void QGLXTexture2D::bind(GLuint textureUnit)
{
    glActiveTexture(GL_TEXTURE0 +textureUnit);
    bind();
}
