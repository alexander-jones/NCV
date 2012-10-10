#ifndef GLHELPER_H
#define GLHELPER_H
#ifdef WIN32
    #pragma comment(lib,"assimp.lib")
    #include <assimp/scene.h>
    #include <assimp/Importer.hpp>
    #include <assimp/postprocess.h>
#else
    #include <assimp/aiPostProcess.h>
    #include <assimp/assimp.hpp>
    #include <assimp/aiScene.h>

#endif
#include <QGLShaderProgram>


struct VertexAtrtributes
{
    aiVector3D position;
    aiVector3D normal;
    VertexAtrtributes (aiVector3D pos ,aiVector3D norm)
    {
        position = pos;
        normal = norm;
    }
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
