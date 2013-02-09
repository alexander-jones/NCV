#ifndef QGLXTEXTURE1D_H
#define QGLXTEXTURE1D_H

#include "qglxtexture.h"
class QGLXTexture1D:public QGLXTexture
{
public:
    QGLXTexture1D();

    /*!
        \param width The width of the texture to allocate.
        \param internalFormat The internal format of the texture to allocate.
        \brief This function allocates the texture in OpenGL memory.
    */
    void allocate(GLuint width, GLenum internalFormat, GLvoid * data = NULL);

    /*!
        \brief This function binds the texture to the current context.
    */
    void bind();

    /*!
        \param target The logical frame buffer target to use for this texture
        \param attachment The frame buffer attachment slot in the currently bound framebuffer.
        \brief This function binds the texture to the current context as a frame buffer texture.
    */
    void bind(FrameBufferTarget target, FrameBufferAttachment attachment) ;


    /*!
        \param unit The image texture unit to bind this texture to.
        \param access Defines how this texture will be accessed from within a shader program.
        \param level The level of this texture to bind to the assocaited image texture unit.
        \brief This function binds the texture to the current context as an image (atomic read/write access) texture;
    */
    void bind(ImageUnit unit,ImageTextureAccess access,GLuint level=0);


    /*!
        \param unit The texture unit to bind this texture to.
        \brief This function binds the texture to the current context in a shader texture unit.
    */
    void bind(GLuint unit);

    /*!
        \brief Creates the texture.
    */
    void create();

    /*!
        \brief This function deallocates the OpenGL memory associated with this texture.
    */
    void destroy();

    /*!
        \brief This function returns the id of this texture.
    */
    GLuint id();

    /*!
        \brief This function returns whether or not this texture has been sucessfully allocated.
    */
    bool isCreated();
    /*!
        \brief This function returns the internal format of this texture.
    */
    GLenum internalFormat();

    /*!
        \brief This function returns the pixel format of this texture.
    */
    GLenum pixelFormat();

    /*!
        \brief This function returns the pixel type of this texture.
    */
    GLenum pixelType();

    /*!
        \brief This function releases this texture from the current context.
    */
    void release();

    /*!
        \param width The new width of this texture.
        \brief This function sets the dimensions of this texture.
    */
    void setSize(GLuint width);

    void setMagFilter(Filter filter);
    void setMinFilter(Filter filter);
    void setWrapFunction(WrapFunction onWidth);

    /*!
        \brief This function returns the width of this texture.
    */
    GLuint width();

private:
    GLuint m_id;
    GLenum m_pixelType;
    GLenum m_pixelFormat;
    GLenum m_internalFormat;
    GLuint m_width;
};

#endif // QGLXTEXTURE1D_H
