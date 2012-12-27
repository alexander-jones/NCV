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


class QGLXCore
{
public:

    static bool compileShaderProgram(QGLShaderProgram * program, const QString& vertexShaderPath,const QString& fragmentShaderPath )
    {
        // First we load and compile the vertex shader->..
        bool success = program->addShaderFromSourceFile( QGLShader::Vertex, vertexShaderPath );
        if ( !success )
            qDebug() << program->log();

        // ...now the fragment shader->..
        success = program->addShaderFromSourceFile( QGLShader::Fragment, fragmentShaderPath );
        if ( !success )
            qDebug() << program->log();

        // ...and finally we link them to resolve any references.
        success = program->link();
        if ( !success )
            qDebug() << "Could not link shader program:" << program->log();
        return success;
    }

    static bool compileShaderProgram(QGLShaderProgram * program, const QString& vertexShaderPath, const QString & geometryShaderPath, const QString& fragmentShaderPath )
    {
        // First we load and compile the vertex shader->..
        bool success = program->addShaderFromSourceFile( QGLShader::Vertex, vertexShaderPath );
        if ( !success )
            qDebug() << program->log();

        // ...now the geometry shader->..
        success = program->addShaderFromSourceFile( QGLShader::Geometry, geometryShaderPath );
        if ( !success )
            qDebug() << program->log();

        // ...now the fragment shader->..
        success = program->addShaderFromSourceFile( QGLShader::Fragment, fragmentShaderPath );
        if ( !success )
            qDebug() << program->log();


        // ...and finally we link them to resolve any references.
        success = program->link();
        if ( !success )
            qDebug() << "Could not link shader program:" << program->log();
        return success;
    }

};

#endif // QGLXCORE_H
