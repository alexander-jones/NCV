#ifndef QGLXTEXTURE_H
#define QGLXTEXTURE_H

#include "qglxcore.h"

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
        /*!
            \brief Clamps the texture coordinate in the [0,1] range. In the limits the color of texture is combined with the color of the edge.
        */
        Clamp = GL_CLAMP,
        /*!
            \brief Clamps the texture coordinate in the [0,1] range. In the limits the color of texture is combined with the color of the edge.
        */
        ClampToEdge = GL_CLAMP_TO_EDGE,
        /*!
            \brief Sets the texture coordinates to [1/2N] (where N is the size of the texture) texels within the texture. This prevents to draw the border color. Available since OpenGL 1.2.
        */
        ClampToBorder = GL_CLAMP_TO_BORDER,
        /*!
            \brief Sets the texture coordinates to [1/2N] (where N is the size of the texture) texels outside the texture. This allows to draw the border color without mixing with the texture color. Available since OpenGL 1.3.
        */
        Repeat = GL_REPEAT,
        /*!
            \brief Ignores the integer portion of the texture coordinate, only the fractional part is used, which creates a repeating pattern. This is the default value.
        */
        MirroredRepeat = GL_MIRRORED_REPEAT
        /*!
            \brief The coordinates are set to the fractional part of the texture coordinate if the integer part is even; if the integer part is odd, then the texture coordinate is set to 1 – the fractional part of the texture coordinate.
        */
    };

    enum FrameBufferTarget
    {
        Draw = GL_DRAW_FRAMEBUFFER,
        Read = GL_READ_FRAMEBUFFER
    };

    enum FrameBufferAttachment
    {
        Color0 = GL_COLOR_ATTACHMENT0,
        Color1 = GL_COLOR_ATTACHMENT1,
        Color2 = GL_COLOR_ATTACHMENT2,
        Color3 = GL_COLOR_ATTACHMENT3,
        Color4 = GL_COLOR_ATTACHMENT4,
        Color5 = GL_COLOR_ATTACHMENT5,
        Color6 = GL_COLOR_ATTACHMENT6,
        Color7 = GL_COLOR_ATTACHMENT7,
        Color8 = GL_COLOR_ATTACHMENT8,
        Color9 = GL_COLOR_ATTACHMENT9,
        Color10 = GL_COLOR_ATTACHMENT10,
        Color11 = GL_COLOR_ATTACHMENT11,
        Color12 = GL_COLOR_ATTACHMENT12,
        Color13 = GL_COLOR_ATTACHMENT13,
        Color14 = GL_COLOR_ATTACHMENT14,
        Color15 = GL_COLOR_ATTACHMENT15,
        Depth = GL_DEPTH_ATTACHMENT,
        DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT,
        Unset
    };

    /*!
        \brief This enum describes the available image unit slots.
    */
    enum ImageUnit
    {
        Zero = 0,
        One,
        Two,
        Three,
        Four,
        Five,
        Six,
        Seven
    };


    /*!
        \brief This enum defines how an QGLXTexure bound to an image unit will be acessed in a shader program.
    */
    enum ImageTextureAccess
    {
        /*!
            \brief This texture will only be read from in shader programs.
        */
        ReadOnly = GL_READ_ONLY,

        /*!
            \brief This texture will only be written to in shader programs.
        */
        WriteOnly= GL_WRITE_ONLY,

        /*!
            \brief This texture will be both read from and written to in shader programs.
        */
        ReadWrite= GL_READ_WRITE
    };


    QGLXTexture(){}
    virtual void bind(){}
    virtual void create(){}
    virtual void destroy(){}
    virtual GLuint id(){return 0;}
    virtual bool isCreated(){return false;}
    virtual GLenum internalFormat(){return 0;}
    virtual GLenum pixelFormat(){return 0;}
    virtual GLenum pixelType(){return 0;}
    virtual void release(){}

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

    /*!
        \param textureUnit The texture unit to set as active. Must be between 0 and getMaxTextureUnits() -1.
        \brief This function extrapolates size from a component type enum.
    */

};

#endif // QGLXTEXTURE_H
