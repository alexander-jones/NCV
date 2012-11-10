#ifndef QGLXBUFFER_H
#define QGLXBUFFER_H

#include <GL/glew.h>
#include <QGLWidget>
#include <QGLBuffer>
/*!
    \class QGLXBuffer
    \author Alex Jones
    \brief An QGL extension class for managing OpenGL buffers. Extends functionality of \htmlonly<a href="http://qt-project.org/doc/qt-4.8/qglbuffer.html"> QGLBuffer</a>  \endhtmlonly to include Texture Buffers.
*/
class QGLXBuffer : public QGLBuffer
{
public:

    /*!
        \brief This enum defines the type of GL buffer object to create with QGLXBuffer.
    */
    enum Type
    {
        /*! \brief Vertex buffer object for use when specifing vertex arrays. */
        VertexBuffer = QGLBuffer::VertexBuffer,
        /*! \brief Index buffer object for use with glDrawElements / glDrawElementsInstanced */
        IndexBuffer = QGLBuffer::IndexBuffer,
        /*! \brief Pixel pack buffer object for reading pixel data from the GL server (for example, with glReadPixels()). Not supported under OpenGL/ES. */
        PixelPackBuffer = QGLBuffer::PixelPackBuffer,
        /*! \brief Pixel unpack buffer object for writing pixel data to the GL server (for example, with glTexImage2D()). Not supported under OpenGL/ES. */
        PixelUnpackBuffer = QGLBuffer::PixelUnpackBuffer,
        /*! \brief Texture buffer object used for specifing buffer data as a texture. */
        TextureBuffer
    };


    /*!
        \brief Constructs a new buffer object of Type VertexBuffer.
    */
    QGLXBuffer():QGLBuffer(){}
    /*!
        \brief Constructs a new buffer object of Type specified by type.
    */
    QGLXBuffer(QGLXBuffer::Type type) ;


    /*!
        \param data A pointer to the data to use in the buffer;
        \param count The size (in bytes) of data;
        \param dataType The OpenGL data type used by this buffer.
        \brief This function allocates memory for this buffer. It is assumed that create() has been called on this buffer and that it has
        been bound to the current context.
    */
    void allocate(const void *data, int ,GLenum  dataType = GL_UNSIGNED_INT);

    /*!
        \brief This function binds this buffer to the current context.
    */
    bool bind();

    /*!
        \brief This function creates this buffer in OpenGL controlled memory.
    */
    bool create();

    /*!
        \brief This function destroys this buffer in OpenGL controlled memory..
    */
    void destroy();

    /*!
        \brief This function returns the texture slot associated with this buffer.
        If this buffer is not a Texture Buffer or has not had setTextureSlot called on it, -1 will be returned.
    */
    GLint textureSlot();

    /*!
        \brief This function releases this buffer from the current context.
    */
    void release();
    /*!
        \param offset The offset (in bytes) to start reading from.
        \param data A pointer to the array to read into.
        \param count The size (in bytes) of data to read;
        \brief This function allows the user to read data stored in this buffer.
    */
    bool read(int offset, void *data, int count);
    /*!
        \param slot The texture slot to associate with this buffer.
        \brief This function associates a Texture Buffer with a texture slot.
    */
    void setTextureSlot(GLuint slot);

    /*!
        \param offset The offset (in bytes) to start write from.
        \param data A pointer to the array to write from.
        \param count The size (in bytes) of data to write;
        \brief This function allows the user to write data stored in this buffer.
    */
    void write(int offset, void *data, int count);

    /*!
        \brief This function returns the texture id for TextureBuffer objects. This function will return -1 if the buffer is not a Texture Buffer,
        or is a TextureBuffer that hasn't had create() called on it.
    */
    GLint textureID();


private:
    GLuint m_tbo,m_tex;
    GLenum m_dataType, m_textureSlot;
    Type m_type;
};

#endif // QGLXBUFFER_H
