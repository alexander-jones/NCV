#ifndef QGLXTEXTURE1D_H
#define QGLXTEXTURE1D_H

#include "qglxtexture.h"


/*!
    \class QGLXTexture1D
    \author Alex Jones
    \brief A class for manipulating one dimensional textures.
*/
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
        \param attachment The frame buffer attachment slot to use in the currently bound framebuffer.
        \param offset The offset from the first attachment slot to use. Only applies to color attachments.
        \brief This function binds the texture to the current context as a frame buffer texture.
    */
    void bind(FrameBufferTarget target, FrameBufferAttachment attachment, GLuint offset = 0) ;


    /*!
        \param unit The image texture unit to bind this texture to.
        \param access Defines how this texture will be accessed from within a shader program.
        \param level The level of this texture to bind to the assocaited image texture unit.
        \brief This function binds the texture to the current context as an image (atomic read/write access) texture;
    */
    void bind(GLuint unit, Access access,GLuint level=0);


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

    /*!
        \param filter The texture filtering option to use.
        \brief This function sets the mag filter for this texture.
        When a surface is rendered with greater dimensions than the uv mapping of this texture, the mag filter controls how pixel values are interpolated.
        It is assumed this texture has been allocated and is bound to the current context.
    */
    void setMagFilter(Filter filter);

    /*!
        \param filter The texture filtering option to use.
        \brief This function sets the mag filter for this texture.
        When a surface is rendered with greater dimensions than the uv mapping of this texture, the mag filter controls how pixel values are interpolated.
        It is assumed this texture has been allocated and is bound to the current context.
    */
    void setMinFilter(Filter filter);

    /*!
        \param onWidth The wrap function for the x axis of the texture.
        \param onHeight The wrap function for the y axis of the texture.
        \param onDepth The wrap function for the z axis of the texture.
        \brief This function describes how texture coordinates outside of the 0.0 -1.0 range access this texture.
        It is assumed this texture has been allocated and is bound to the current context.
    */
    void setWrapFunction(WrapFunction onWidth);

    /*!
        \brief This function returns the width of this texture.
    */
    GLuint width();

private:
    GLuint m_imageUnit;
    GLuint m_id;
    GLenum m_pixelType;
    GLenum m_pixelFormat;
    GLenum m_internalFormat;
    GLuint m_width;
};

#endif // QGLXTEXTURE1D_H
