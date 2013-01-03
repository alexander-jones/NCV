#ifndef QGLXCORE_H
#define QGLXCORE_H

#ifdef WIN32
    #pragma comment(lib,"glew32.lib")
#endif
#include <GL/glew.h>
#include <QGLShaderProgram>

#include <QVector3D>
struct QGLXLight
{
    QGLXLight(){}
    QVector3D position, color;
    GLfloat specularPower;
    GLfloat constantAttenuation,linearAttenuation,quadtraticAttenuation,radius;
    bool enabled;
    GLuint index;

};
struct QGLXMaterial
{
    float AmbientIntensity, SpecularIntensity, DiffuseIntensity;
};



#endif // QGLXCORE_H
