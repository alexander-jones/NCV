#ifndef QGLXCORE_H
#define QGLXCORE_H

#include <GL/glew.h>
#include <QGLShaderProgram>

#include <QVector3D>

class QGLXCore
{
public:
    QGLXCore(){}
    static char * getErrorString(GLenum error) { return (char *)gluErrorString(error);}
};


class QGLXObject3D
{
public:
    QGLXObject3D(){}
    virtual void update(float dTime){}
    virtual QVector3D position(){}
    virtual QVector3D forward(){}
    virtual void setForward(QVector3D forward){}
    virtual void setPosition(QVector3D position){}

};


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
