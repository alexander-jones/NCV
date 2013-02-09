#include "qglxtexture.h"

GLuint QGLXTexture::getMaxTextureUnits()
{
    GLint max = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,&max);
    return  max;
}
GLuint QGLXTexture::getComponentSize(GLenum componentType)
{
    switch(componentType)
    {
        case GL_INT:
        return sizeof(GLint);
        break;

        case GL_FLOAT:
        return sizeof(GLfloat);
        break;

        case GL_UNSIGNED_INT:
        return sizeof(GLuint);
        break;

        case GL_SHORT:
        return sizeof(GLshort);
        break;

        case GL_BOOL:
        return sizeof(GLboolean);
        break;

        case GL_UNSIGNED_BYTE:
        return sizeof(GLubyte);
        break;

        case GL_BYTE:
        return  sizeof(GLbyte);
        break;
    }

    return 0;

}


GLenum QGLXTexture::internalFormatToPixelFormat(GLenum internalFormat)
{
    GLenum pixelFormat = -1;
    // determine generic pixel format based on internalFormat setting
    switch (internalFormat)
    {
        case GL_RGB4:
        case GL_RGB5:
        case GL_RGB8:
        case GL_RGB10:
        case GL_RGB12:
        case GL_RGB16:
        case GL_RGB8UI:
        case GL_RGB16UI:
        case GL_RGB32UI:
        case GL_RGB8I:
        case GL_RGB16I:
        case GL_RGB32I:
        case GL_RGB16F:
        case GL_RGB32F:
            pixelFormat = GL_RGB;
            break;

        case GL_RGBA4:
        case GL_RGBA8:
        case GL_RGBA12:
        case GL_RGBA16:
        case GL_RGBA8UI:
        case GL_RGBA16UI:
        case GL_RGBA32UI:
        case GL_RGBA8I:
        case GL_RGBA16I:
        case GL_RGBA32I:
        case GL_RGBA16F:
        case GL_RGBA32F:
            pixelFormat = GL_RGBA;
            break;

        case GL_DEPTH_COMPONENT16:
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT32:
        case GL_DEPTH_COMPONENT32F:
        case GL_DEPTH_COMPONENT32F_NV:
            pixelFormat = GL_DEPTH_COMPONENT;
            break;


        case GL_R8UI:
        case GL_R16UI:
        case GL_R32UI:
        case GL_R8I:
        case GL_R16I:
        case GL_R32I:
            pixelFormat = GL_RED_INTEGER;
            break;
    }
    return pixelFormat;
}

GLenum QGLXTexture::internalFormatToPixelType(GLenum internalFormat)
{
    GLenum pixelType = -1;

    switch(internalFormat)
    {
    case GL_RGB8UI:
    case GL_RGB16UI:
    case GL_RGB32UI:
    case GL_R8UI:
    case GL_R16UI:
    case GL_R32UI:
    case GL_RGBA8UI:
    case GL_RGBA16UI:
    case GL_RGBA32UI:
        pixelType = GL_UNSIGNED_INT;
        break;

    case GL_RGBA8I:
    case GL_RGBA16I:
    case GL_RGBA32I:
    case GL_RGB8I:
    case GL_RGB16I:
    case GL_RGB32I:
    case GL_R8I:
    case GL_R16I:
    case GL_R32I:
        pixelType = GL_INT;
        break;

    case GL_RGB4:
    case GL_RGB5:
    case GL_RGB8:
    case GL_RGB10:
    case GL_RGB12:
    case GL_RGB16:
    case GL_RGB16F:
    case GL_RGB32F:
    case GL_RGBA4:
    case GL_RGBA8:
    case GL_RGBA12:
    case GL_RGBA16:
    case GL_RGBA16F:
    case GL_RGBA32F:
    case GL_DEPTH_COMPONENT16:
    case GL_DEPTH_COMPONENT24:
    case GL_DEPTH_COMPONENT32:
    case GL_DEPTH_COMPONENT32F:
    case GL_DEPTH_COMPONENT32F_NV:
        pixelType = GL_FLOAT;
        break;
    }
    return pixelType;
}

GLenum QGLXTexture::bufferFormatToTextureFormat(GLenum componentType,  int tupleSize,int  componentSize)
{
    switch(componentType)
    {
        case GL_INT:
        if (componentSize == 1)
        {
            if (tupleSize == 1)
                return GL_R8I;
            else if (tupleSize ==2)
                return GL_RG8I;
            else if (tupleSize ==3)
                return GL_RGB8I;
            else if (tupleSize ==4)
                return GL_RGBA8I;
        }
        if (componentSize == 2)
        {
            if (tupleSize == 1)
                return GL_R16I;
            else if (tupleSize ==2)
                return GL_RG16I;
            else if (tupleSize ==3)
                return GL_RGB16I;
            else if (tupleSize ==4)
                return GL_RGBA16I;
        }
        if (componentSize == 4)
        {
            if (tupleSize == 1)
                return GL_R32I;
            else if (tupleSize ==2)
                return GL_RG32I;
            else if (tupleSize ==3)
                return GL_RGB32I;
            else if (tupleSize ==4)
                return GL_RGBA32I;
        }
        break;

        case GL_FLOAT:
        if (componentSize == 2)
        {
            if (tupleSize == 1)
                return GL_R16F;
            else if (tupleSize ==2)
                return GL_RG16F;
            else if (tupleSize ==3)
                return GL_RGB16F;
            else if (tupleSize ==4)
                return GL_RGBA16F;
        }
        if (componentSize == 4)
        {
            if (tupleSize == 1)
                return GL_R32F;
            else if (tupleSize ==2)
                return GL_RG32F;
            else if (tupleSize ==3)
                return GL_RGB32F;
            else if (tupleSize ==4)
                return GL_RGBA32F;
        }
        break;

        case GL_UNSIGNED_INT:
        if (componentSize == 1)
        {
            if (tupleSize == 1)
                return GL_R8UI;
            else if (tupleSize ==2)
                return GL_RG8UI;
            else if (tupleSize ==3)
                return GL_RGB8UI;
            else if (tupleSize ==4)
                return GL_RGBA8UI;
        }
        if (componentSize == 2)
        {
            if (tupleSize == 1)
                return GL_R16UI;
            else if (tupleSize ==2)
                return GL_RG16UI;
            else if (tupleSize ==3)
                return GL_RGB16UI;
            else if (tupleSize ==4)
                return GL_RGBA16UI;
        }
        if (componentSize == 4)
        {
            if (tupleSize == 1)
                return GL_R32UI;
            else if (tupleSize ==2)
                return GL_RG32UI;
            else if (tupleSize ==3)
                return GL_RGB32UI;
            else if (tupleSize ==4)
                return GL_RGBA32UI;
        }
        break;

        case GL_SHORT:
        case GL_UNSIGNED_BYTE:
        if (componentSize == 1)
        {
            if (tupleSize == 1)
                return GL_R8;
            else if (tupleSize ==2)
                return GL_RG8;
            else if (tupleSize ==3)
                return GL_RGB8;
            else if (tupleSize ==4)
                return GL_RGBA8;
        }
        if (componentSize == 2)
        {
            if (tupleSize == 1)
                return GL_R16;
            else if (tupleSize ==2)
                return GL_RG16;
            else if (tupleSize ==3)
                return GL_RGB16;
            else if (tupleSize ==4)
                return GL_RGBA16;
        }
        break;

    }
    return GL_INVALID_ENUM;
}

