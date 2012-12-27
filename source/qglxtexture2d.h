#ifndef QGLXTEXTURE2D_H
#define QGLXTEXTURE2D_H

#include "qglxtexture.h"

class QGLXTexture2D : public QGLXTexture
{
public:
    QGLXTexture2D();


    /*!
        \param width The width of the texture to allocate.
        \param height The height of the texture to allocate.
        \param internalFormat The internal format of the texture to allocate.
        \brief This function allocates the texture in OpenGL memory.
    */
    void allocate(GLuint width,GLuint height, GLenum internalFormat, GLvoid * data = NULL);

    /*!
        \brief This function binds the texture to the current context.
    */
    void bind();


    /*!
        \param target The logical frame buffer target to use for this texture
        \param attachment The frame buffer attachment slot in the currently bound framebuffer.
        \brief This function binds the texture to the current context.
    */
    void bindAsFrameBufferTexture(FrameBufferTarget target, FrameBufferAttachment attachment) ;

    /*!
        \param unit The image texture unit to bind this texture to.
        \param access Defines how this texture will be accessed from within a shader program.
        \param level The level of this texture to bind to the assocaited image texture unit.
        \brief Constructs a QGLXTexture object of type.
    */
    void bindAsImageTexture(ImageUnit unit,ImageTextureAccess access,GLuint level=0);

    /*!
        \brief Creates the texture.
    */
    void create();


    /*!
        \brief This function deallocates the OpenGL memory associated with this texture.
    */
    void destroy();


    /*!
        \brief This function returns the height of this texture.
    */
    GLuint height();

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
        \param height The new height of this texture.
        \param depth The new depth of this texture.
        \brief This function sets the dimensions of this texture.
    */
    void setSize(GLuint width,GLuint height);

    /*!
        \param filter The mag filter to use when sampling this texture.
        \brief This function sets the mag filter setting for sampling.
        \note Nearest and Linear are the only valid mag filters.
        It is assumed this texture has been allocated and is bound to the current context.
    */

    void setMagFilter(Filter filter);


    /*!
        \param filter The min filter to use when sampling this texture.
        \brief This function sets the min filter setting for sampling.
        It is assumed this texture has been allocated and is bound to the current context.
    */
    void setMinFilter(Filter filter);


    /*!
        \param onWidth The wrap function for the x axis of the texture;
        \param onHeight The wrap function for the y axis of the texture;
        \brief This function describes how texture coordinates outside of the 0.0 -1.0 range access this texture.
        It is assumed this texture has been allocated and is bound to the current context.
    */
    void setWrapFunction(WrapFunction onWidth, WrapFunction onHeight);


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
    GLuint m_height;
};

#endif // QGLXTEXTURE2D_H
