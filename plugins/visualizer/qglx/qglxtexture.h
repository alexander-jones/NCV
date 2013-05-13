#ifndef QGLXTEXTURE_H
#define QGLXTEXTURE_H
#include <qmath.h>
#include <GL/glew.h>
#include <QSize>
#include <QRect>

/*!
    \class QGLXTexture
    \author Alex Jones
    \brief This class faciliates management of textures stored in OpenGL memory.
*/
class QGLXTexture
{
public:

    enum Filter
    {
        Nearest = GL_NEAREST,
        Linear = GL_LINEAR,
        NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
        LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
        NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
        LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR
    };

    enum WrapFunction
    {
        /*! \brief Clamps the texture coordinate in the [0,1] range. In the limits the color of texture is combined with the color of the edge. */
        Clamp = GL_CLAMP,
        /*! \brief Clamps the texture coordinate in the [0,1] range. In the limits the color of texture is combined with the color of the edge. */
        ClampToEdge = GL_CLAMP_TO_EDGE,
        /*! \brief Sets the texture coordinates to [1/2N] (where N is the size of the texture) texels within the texture. This prevents to draw the border color. Available since OpenGL 1.2. */
        ClampToBorder = GL_CLAMP_TO_BORDER,
        /*! \brief Ignores the integer portion of the texture coordinate, only the fractional part is used, which creates a repeating pattern. This is the default value. */
        Repeat = GL_REPEAT,
        /*! \brief The coordinates are set to the fractional part of the texture coordinate if the integer part is even; if the integer part is odd, then the texture coordinate is set to 1 – the fractional part of the texture coordinate. */
        MirroredRepeat = GL_MIRRORED_REPEAT
    };

    enum FrameBufferTarget
    {
        Draw = GL_DRAW_FRAMEBUFFER,
        Read = GL_READ_FRAMEBUFFER
    };
    enum FrameBufferAttachment
    {
        Color = GL_COLOR_ATTACHMENT0,
        Depth = GL_DEPTH_ATTACHMENT,
        DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT
    };
    /*!
        \brief This enum defines access given to a resource utilizing an OpenGL object.
    */
    enum Access
    {
        /*! \brief A resource is given read only access to the underlying OpenGL object data storage. */
        ReadOnly = GL_READ_ONLY,
        /*! \brief A resource is given write only access to the underlying OpenGL object data storage. */
        WriteOnly = GL_WRITE_ONLY,
        /*! \brief A resource is given read and write access to the underlying OpenGL object data storage. */
        ReadWrite = GL_READ_WRITE
    };

    /*!
        \param componentType A component type value.
        \brief This function extrapolates size from a component type enum.
    */
    static GLuint getComponentSize(GLenum componentType);

    /*!
        \brief This function returns the maximum amount of texture units available on this system.
    */
    static GLuint getMaxTextureUnits();

    /*!
        \param componentType The component type of the buffer format.
        \param tupleSize The number of componentType items per element.
        \param componentSize The amount of bytes per componentType.
        \brief This function extrapolates a texture format from a buffer format.
    */
    static GLenum bufferFormatToTextureFormat(GLenum componentType,  int tupleSize,int  componentSize);

    /*!
        \param internalFormat The internal format to convert from.
        \brief This function extrapolates a pixel format from a internal format.
    */
    static GLenum internalFormatToPixelFormat(GLenum internalFormat);


    /*!
        \param internalFormat The internal format to convert from.
        \brief This function extrapolates a pixel type from a internal format.
    */
    static GLenum internalFormatToPixelType(GLenum internalFormat);

};

#endif // QGLXTEXTURE_H
