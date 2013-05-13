#ifndef QGLXBUFFERTEXTURE_H
#define QGLXBUFFERTEXTURE_H
#include "qglxtexture.h"
#include "qglxbuffer.h"

/*!
    \class QGLXBufferTexture
    \author Alex Jones
    \brief A class for accessing buffer memory as a one dimensional texture.
*/
class QGLXBufferTexture : public QGLXTexture
{
public:

    /*!
        \brief Constructor. That is all.
    */
    QGLXBufferTexture();

    /*!
        \param buffer The buffer to use as the data source for this texture.
        \param internalFormat The internal format of the texture to allocate.
        \brief This function attaches the memory of a buffer object to this texture.
        Beyond buffer allocation, no additional allocation needs to be performed to use this texture.
    */
    void attach(QGLXBuffer buffer, GLenum internalFormat);

    /*!
        \brief This function binds the texture to the current context.
    */
    void bind();

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
        \brief This function releases this texture from the current context.
    */
    void release();

private:
    QGLXBuffer m_buffer;
    GLenum m_bindTarget;
    GLuint m_id;
    GLenum m_internalFormat;
    GLuint m_width;
};

#endif // QGLXBUFFERTEXTURE_H
