#ifndef QGLXTEXTURE3D_H
#define QGLXTEXTURE3D_H

#include "qglxtexture.h"

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
        \param layer The layer of this 3d texture to bind to the frame buffer.
        \brief This function binds the texture to the current context.
    */
    void bind(FrameBufferTarget target, FrameBufferAttachment attachment, GLuint layer);

    /*!
        \param unit The image texture unit to bind this texture to.
        \param access Defines how this texture will be accessed from within a shader program.
        \param level The level of this texture to bind to the assocaited image texture unit.
        \param layer If layered is true, this specifies the layer to bind to.
        \brief Constructs a QGLXTexture object of type.
    */
    void bind(QGLXTexture3D::ImageUnit unit,  QGLXTexture3D::ImageTextureAccess access,GLuint layer,GLuint level = 0);


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

    void setMagFilter(Filter filter);
    void setMinFilter(Filter filter);
    void setWrapFunction(WrapFunction onWidth,WrapFunction onHeight, WrapFunction onDepth);
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
    GLuint m_depth;
};

#endif // QGLXTEXTURE3D_H
