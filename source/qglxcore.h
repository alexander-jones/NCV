#ifndef QGLXCORE_H
#define QGLXCORE_H
#define _USE_MATH_DEFINES 
#include <GL/glew.h>
#include <QGLShaderProgram>

#include <QVector3D>


class QGLXCore
{

public:

    enum PrimitiveType
    {
        Point,
        Line,
        LineStrip,
        Triangle,
        TriangleStrip,
        Quad
    };
    QGLXCore(){}
    static char * getErrorString(GLenum error) { return (char *)gluErrorString(error);}
};




#endif // QGLXCORE_H
