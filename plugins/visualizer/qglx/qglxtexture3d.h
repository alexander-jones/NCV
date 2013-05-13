#ifndef QGLXTEXTURE3D_H
#define QGLXTEXTURE3D_H

#include "qglxtexture.h"

/*!
    \class QGLXTexture3D
    \author Alex Jones
    \brief A class for manipulating three dimensional textures.
*/
class QGLXTexture3D : public QGLXTexture
{
public:
    QGLXTexture3D();


    /*!
        \param width The width of the texture to allocate.
        \param height The height of the texture to allocate.
        \param depth The depth of the texture to allocate.
        \param internalFormat The internal format of the texture to allocate.
        \brief This function allocates the texture in OpenGL memory.
    */
    void allocate(GLuint width,GLuint height, GLuint depth,GLenum internalFormat, GLvoid * data = NULL);


    /*!
        \brief This function binds the texture to the current context.
    */
    void bind();

    /*!
        \param target The logical frame buffer target to use for this texture
        \param attachment The frame buffer attachment slot in the currently bound framebuffer.
        \param offset The offset from the first attachment slot to use. Only applies to color attachments.
        \param layer The layer of this 3d texture to bind to the frame buffer.
        \brief This function binds the texture to the current context.
    */
    void bind(FrameBufferTarget target, FrameBufferAttachment attachment, GLuint offset = 0, GLuint layer = 0);

    /*!
        \param unit The image texture unit to bind this texture to.
        \param access Defines how this texture will be accessed from within a shader program.
        \param level The level of this texture to bind to the assocaited image texture unit.
        \param layer If layered is true, this specifies the layer to bind to.
        \brief Constructs a QGLXTexture object of type.
    */
    void bind(GLuint unit,  Access access,GLuint layer,GLuint level = 0);


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
        \brief This function returns the depth of this texture.
    */
    GLuint depth();

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
    void setSize(GLuint width,GLuint height, GLuint depth);

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
    void setWrapFunction(WrapFunction onWidth,WrapFunction onHeight, WrapFunction onDepth);
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
    GLuint m_height;
    GLuint m_depth;
};

#endif // QGLXTEXTURE3D_H
