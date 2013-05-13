#include "qglxtexture3d.h"
const unsigned int NOT_CREATED = 0;
const GLuint INVALID_IMAGE_UNIT = -1;

QGLXTexture3D::QGLXTexture3D()
{
    m_id = NOT_CREATED;
    m_imageUnit = INVALID_IMAGE_UNIT;
}
void QGLXTexture3D::bind()
{
    glBindTexture(GL_TEXTURE_3D,m_id);
}

void QGLXTexture3D::bind(FrameBufferTarget target, FrameBufferAttachment attachment, GLuint offset, GLuint layer)
{
    if (attachment != Color)
        offset = 0;

    glBindTexture(GL_TEXTURE_3D,m_id);
    glFramebufferTexture3D(target, attachment + offset, GL_TEXTURE_3D, m_id, 0,layer);
}


void QGLXTexture3D::bind(GLuint unit,  Access access,GLuint layer,GLuint level)
{
    m_imageUnit = unit;
    glBindImageTexture(unit, m_id, level, true, layer, access,m_internalFormat);

}

void QGLXTexture3D::create()
{
    glGenTextures(1, &m_id);
    bind();
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    release();
}


bool QGLXTexture3D::isCreated()
{
    return m_id != NOT_CREATED;
}

void QGLXTexture3D::setMagFilter(Filter filter)
{
    if (filter == NearestMipmapNearest || filter == NearestMipmapLinear)
        filter = Nearest;
    else if (filter == LinearMipmapNearest || filter == LinearMipmapLinear)
        filter = Linear;

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filter);
}
void QGLXTexture3D::setMinFilter(Filter filter)
{
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filter);
}

void QGLXTexture3D::setWrapFunction(WrapFunction onWidth,WrapFunction onHeight, WrapFunction onDepth)
{
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, onWidth);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, onHeight);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, onDepth);
}

void QGLXTexture3D::allocate(GLuint width, GLuint height, GLuint depth,GLenum internalFormat, GLvoid * data)
{
    m_width = width;
    m_height = height;
    m_depth = depth;
    m_internalFormat = internalFormat;
    m_pixelType = internalFormatToPixelType(m_internalFormat);
    m_pixelFormat = internalFormatToPixelFormat(m_internalFormat);


    glBindTexture(GL_TEXTURE_3D,m_id);
    glTexImage3D(GL_TEXTURE_3D, 0, m_internalFormat, m_width,m_height,m_depth, 0, m_pixelFormat, m_pixelType, data);

    glBindTexture(GL_TEXTURE_3D,0);


}
void QGLXTexture3D::destroy()
{
    glDeleteTextures(1,&m_id);
}

GLuint QGLXTexture3D::id()
{
    return m_id;
}
GLenum QGLXTexture3D::internalFormat()
{
    return m_internalFormat;
}
GLenum QGLXTexture3D::pixelFormat()
{
    return m_pixelFormat;
}
GLenum QGLXTexture3D::pixelType()
{
    return m_pixelType;
}
void QGLXTexture3D::release()
{
    if (m_imageUnit != INVALID_IMAGE_UNIT)
        glBindImageTexture(m_imageUnit, 0, 0, false, 0, GL_READ_ONLY,m_internalFormat);
    else
        glBindTexture(GL_TEXTURE_3D,0);
    m_imageUnit = INVALID_IMAGE_UNIT;
}

void QGLXTexture3D::setSize(GLuint width,GLuint height,GLuint depth)
{
    m_width = width;
    m_height = height;
    m_depth = depth;

    glBindTexture(GL_TEXTURE_3D,m_id);
    glTexImage3D(GL_TEXTURE_3D, 0, m_internalFormat, m_width,m_height,m_depth, 0, m_pixelFormat, m_pixelType, 0);
    glBindTexture(GL_TEXTURE_3D,0);

}
GLuint QGLXTexture3D::width()
{
    return m_width;
}


GLuint QGLXTexture3D::height()
{
    return m_height;
}

GLuint QGLXTexture3D::depth()
{
    return m_depth;
}

void QGLXTexture3D::bind(GLuint textureUnit)
{
    glActiveTexture(GL_TEXTURE0 +textureUnit);
    bind();
}
