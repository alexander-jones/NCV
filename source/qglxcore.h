#ifndef QGLXCORE_H
#define QGLXCORE_H

#ifdef WIN32
    #pragma comment(lib,"glew32.lib")
#endif
#include <GL/glew.h>
class QGLXCore
{
public:

    enum TexturePrecision
    {
        Low,
        Mid,
        High
    };

    static int getComponentSize(GLenum componentType)
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
    static GLenum bufferFormatToTextureFormat(GLenum componentType,  int tupleSize,TexturePrecision precision)
    {
        switch(componentType)
        {
            case GL_INT:
            if (precision == Low)
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
            if (precision == Mid)
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
            if (precision == High)
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
            if (precision == Low || precision == Mid)
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
            if (precision == High)
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
            if (precision == Low)
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
            if (precision == Mid)
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
            if (precision == High)
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
            if (precision == Low)
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
            if (precision == Mid || precision == High)
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
    }
};

#endif // QGLXCORE_H
