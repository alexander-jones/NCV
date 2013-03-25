#ifndef QGLXBUFFER_H
#define QGLXBUFFER_H

#include "qglxcore.h"
#include <QGLWidget>
#include "qglxtexture1d.h"
/*!
    \class QGLXBuffer
    \author Alex Jones
    \brief An QGL extension class for managing OpenGL buffers. Extends functionality of \htmlonly<a href="http://qt-project.org/doc/qt-4.8/qglbuffer.html"> QGLBuffer</a>  \endhtmlonly to include Texture Buffers.
*/
class QGLXBuffer
{
public:

    enum UsagePattern
    {
        StreamDraw = GL_STREAM_DRAW,
        StreamRead = GL_STREAM_READ,
        StreamCopy	= GL_STREAM_COPY,
        StaticDraw	= GL_STATIC_DRAW,
        StaticRead	= GL_STATIC_READ,
        StaticCopy	= GL_STATIC_COPY,
        DynamicDraw	= GL_DYNAMIC_DRAW,
        DynamicRead	= GL_DYNAMIC_READ,
        DynamicCopy = GL_DYNAMIC_COPY
    };

    enum Access
    {
        ReadOnly = GL_READ_ONLY,
        WriteOnly = GL_WRITE_ONLY,
        ReadWrite = GL_READ_WRITE
    };

    /*!
        \brief This enum defines the type of GL buffer object to create with QGLXBuffer.
    */
    enum TargetBinding
    {
        Unbound = -1,
        /*! \brief A buffer used for specifing vertex arrays. */
        ArrayBuffer  = GL_ARRAY_BUFFER,
        /*! \brief A buffer used to specify vertex indices for use with glDrawElements / glDrawElementsInstanced */
        IndexBuffer = GL_ELEMENT_ARRAY_BUFFER ,
        /*! \brief A buffer for reading pixel data from the GL server (for example, with glReadPixels()). Not supported under OpenGL/ES. */
        PixelPackBuffer = GL_PIXEL_PACK_BUFFER,
        /*! \brief A buffer for writing pixel data to the GL server (for example, with glTexImage2D()). Not supported under OpenGL/ES. */
        PixelUnpackBuffer  = GL_PIXEL_UNPACK_BUFFER,
        /*! \brief A one dimensional buffer used for specifing buffer data as a texture. */
        TextureBuffer  = GL_TEXTURE_BUFFER,
        /*! \brief A buffer that can be incremented / decrimented in any shader stage by any GPU core. */
        AtomicCounterBuffer = GL_ATOMIC_COUNTER_BUFFER,
        /*! \brief A buffer used to capture an primitive output stream from a geometry shader. */
        TransformFeedbackBuffer = GL_TRANSFORM_FEEDBACK_BUFFER,
        /*! \brief A buffer used to store uniforms for a shader program. */
        UniformBuffer = GL_UNIFORM_BUFFER,
        /*! \brief A buffer close in functionality to a uniform buffer, but with atomic read / write capabilities. */
        ShaderStorageBuffer = GL_SHADER_STORAGE_BUFFER,
        /*! \brief A buffer that stores parameters for indirect compute shader dispatches. */
        DispatchIndirectBuffer = GL_DISPATCH_INDIRECT_BUFFER


    };


    /*!
        \brief Constructs a new buffer object.
    */
    QGLXBuffer();


    /*!
        \param data A pointer to the data to use in the buffer;
        \param count The size (in bytes) of data;
        \param dataType The OpenGL data type used by this buffer.
        \brief This function allocates memory for this buffer. It is assumed that create() has been called on this buffer and that it has
        been bound to the current context.
    */
    void allocate(const void *data, int count,UsagePattern usage = StaticDraw);


    void allocate(const void *data, int count,GLenum textureFormat,UsagePattern usage = StaticDraw);

    /*!
        \brief This function binds this buffer to the current context.
    */
    bool bind(TargetBinding binding);

    /*!
        \brief This function creates this buffer in OpenGL controlled memory.
    */
    bool create();

    /*!
        \brief This function destroys this buffer in OpenGL controlled memory..
    */
    void destroy();


    GLuint id();

    /*!
        \brief This function indicates whether or not this buffer has been created.
    */
    bool isCreated();

    /*!
        \param access The Read / Write permissions of this mapping.
        \brief This function maps the content of this buffer to a pointer.
    */
    void * map(TargetBinding binding, Access access);

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

    /*!.
        \brief This function unmaps the content of this buffer to a pointer.
    */
    void unmap();

private:
    GLuint m_bufferID,m_textureID;
    GLenum m_dataType;
    TargetBinding m_targetBinding;
};

#endif // QGLXBUFFER_H
