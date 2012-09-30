#ifndef GLHELPER_H
#define GLHELPER_H
#include <QGLShaderProgram>


struct BufferData
{
    float vertex[3];
    float id;
};

class GLHelper
{
public :
    static bool  CompileShaderProgram(QGLShaderProgram * program, const QString& vertexShaderPath,const QString& fragmentShaderPath )
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
};

#endif // GLHELPER_H
