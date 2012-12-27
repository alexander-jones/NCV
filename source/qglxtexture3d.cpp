#include "qglxtexture3d.h"
const unsigned int NOT_CREATED = 99999;

QGLXTexture3D::QGLXTexture3D()
    :QGLXTexture()
{
    m_id = NOT_CREATED;
}
void QGLXTexture3D::bind()
{
    glBindTexture(GL_TEXTURE_3D,m_id);
}

void QGLXTexture3D::bindAsFrameBufferTexture(FrameBufferTarget target, FrameBufferAttachment attachment, GLuint layer)
{
    glBindTexture(GL_TEXTURE_3D,m_id);
    glFramebufferTexture3D(target, attachment, GL_TEXTURE_3D, m_id, 0,layer);
}


void QGLXTexture3D::bindAsImageTexture(QGLXTexture3D::ImageUnit unit,  QGLXTexture3D::ImageTextureAccess access,GLuint layer,GLuint level)
{
    glBindTexture(GL_TEXTURE_3D,m_id);
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

    glTexImage3D(GL_TEXTURE_3D, 0, m_internalFormat, m_width,m_height,m_depth, 0, m_pixelFormat, GL_FLOAT, 0);



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
    glBindTexture(GL_TEXTURE_3D,0);
}

void QGLXTexture3D::setSize(GLuint width,GLuint height,GLuint depth)
{
    m_width = width;
    m_height = height;
    m_depth = depth;

    glBindTexture(GL_TEXTURE_3D,m_id);
    glTexImage3D(GL_TEXTURE_3D, 0, m_internalFormat, m_width,m_height,m_depth, 0, m_pixelFormat, GL_UNSIGNED_BYTE, 0);
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
