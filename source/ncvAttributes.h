#ifndef NCVATTRIBUTES_H
#define NCVATTRIBUTES_H
#include <QGLShaderProgram>
#include <QGLWidget>
#include "qglxtexture.h"
#include "qglxcore.h"

enum RangeInferenceFunction
{
    Max,
    Min,
    Subtract,
    Add,
    Interpolate
};

enum FlagInferenceFunction
{
    Or,
    And,
    Xor
};

enum AttributeOwner
{
    Neuron,
    Connection
};

class Attribute : public QGLXBuffer
{
public:
    Attribute()
        :QGLXBuffer(QGLXBuffer::TextureBuffer){}
    virtual void bindBuffer(GLuint numNeurons,QGLShaderProgram & neuronProgram, GLuint numConnections, QGLShaderProgram & connectionProgram,GLuint textureSlot){}
};

class PositionAttribute : public Attribute
{
public:

    PositionAttribute()
        :Attribute()
    {
        unboundData = NULL;
    }

    void bindBuffer(GLuint numNeurons,QGLShaderProgram & neuronProgram, GLuint numConnections, QGLShaderProgram & connectionProgram,GLuint textureSlot)
    {
        if (!isCreated())
            create();


        qDebug() << glGetError();
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        bind();

        if (unboundData != NULL)
        {
            GLuint componentSize = QGLXTexture::getComponentSize(GL_FLOAT);
            GLenum textureFormat  = QGLXTexture::bufferFormatToTextureFormat(GL_FLOAT,3,QGLXTexture::getComponentSize(GL_FLOAT));

            allocate(unboundData,3* componentSize * numNeurons,textureFormat);
            unboundData = NULL;
        }
        qDebug() << glGetError();
        // bind texture of texture buffer to neuron shader program
        neuronProgram.bind();

        int nLoc = neuronProgram.uniformLocation("Inst_Translation");
        neuronProgram.setUniformValue(nLoc, textureSlot);
        neuronProgram.release();

        // bind texture of texture buffer to neuron shader program
        connectionProgram.bind();

        int sLoc = connectionProgram.uniformLocation("Inst_Translation");
        connectionProgram.setUniformValue(sLoc, textureSlot);
        connectionProgram.release();
        qDebug() << glGetError();
    }


    QVector3D * unboundData;
};


class RangeAttribute : public Attribute
{
public:

    RangeAttribute()
        :Attribute()
    {
        minValue = 0.0f;
        maxValue = 1.0f;
        owner = Neuron;
        unboundData = NULL;
    }
    RangeAttribute(AttributeOwner owner, float minValue, float maxValue)
        :Attribute()
    {
        this->minValue = minValue;
        this->maxValue = maxValue;
        this->owner = owner;
        unboundData = NULL;
    }

    void bindBuffer(GLuint numNeurons,QGLShaderProgram & neuronProgram, GLuint numConnections, QGLShaderProgram & connectionProgram,GLuint textureSlot)
    {
        if (!isCreated())
            create();
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        bind();

        if (unboundData != NULL)
        {
            GLuint componentSize = QGLXTexture::getComponentSize(GL_FLOAT);
            GLenum textureFormat  = QGLXTexture::bufferFormatToTextureFormat(GL_FLOAT,1,QGLXTexture::getComponentSize(GL_FLOAT));

            if (owner == Neuron)
                allocate(unboundData, componentSize * numNeurons,textureFormat);
            else
                allocate(unboundData,componentSize * numConnections,textureFormat);
            unboundData = NULL;
        }
        // bind texture of texture buffer to neuron shader program
        neuronProgram.bind();

        int nLoc = neuronProgram.uniformLocation(name);
        neuronProgram.setUniformValue("MinimumValue", minValue);
        neuronProgram.setUniformValue("MaximumValue", maxValue);
        neuronProgram.setUniformValue(nLoc, textureSlot);
        neuronProgram.release();

        // bind texture of texture buffer to neuron shader program
        connectionProgram.bind();

        int sLoc = connectionProgram.uniformLocation(name);
        connectionProgram.setUniformValue("MinimumValue", minValue);
        connectionProgram.setUniformValue("MaximumValue", maxValue);
        connectionProgram.setUniformValue(sLoc, textureSlot);
        connectionProgram.release();
    }


    AttributeOwner owner;
    float minValue, maxValue;
    GLfloat * unboundData;
    QString name;
};

class FlagAttribute : public Attribute
{
public:
    FlagAttribute()
        :Attribute()
    {
        onColor = QVector3D(1,1,1);
        offColor = QVector3D(0,0,0);
        owner = Neuron;
        unboundData = NULL;
    }
    FlagAttribute(AttributeOwner owner, QVector3D onColor, QVector3D offColor)
        :Attribute()
    {
        this->onColor = onColor;
        this->offColor = offColor;
        this->owner = owner;
        unboundData = NULL;
    }

    void bindBuffer(GLuint numNeurons,QGLShaderProgram & neuronProgram, GLuint numConnections, QGLShaderProgram & connectionProgram,GLuint textureSlot)
    {
        if (!isCreated())
            create();

        glActiveTexture(GL_TEXTURE0 + textureSlot);
        bind();

        if (unboundData != NULL)
        {
            GLuint componentSize = QGLXTexture::getComponentSize(GL_UNSIGNED_BYTE);
            GLenum textureFormat  = QGLXTexture::bufferFormatToTextureFormat(GL_UNSIGNED_BYTE,1,QGLXTexture::getComponentSize(GL_UNSIGNED_BYTE));

            if (owner == Neuron)
                allocate(unboundData, componentSize * numNeurons,textureFormat);
            else
                allocate(unboundData,componentSize * numConnections,textureFormat);
            unboundData = NULL;
        }
        // bind texture of texture buffer to neuron shader program
        neuronProgram.bind();

        int nLoc = neuronProgram.uniformLocation(name);
        neuronProgram.setUniformValue("OnColor", onColor);
        neuronProgram.setUniformValue("OffColor", offColor);
        neuronProgram.setUniformValue(nLoc, textureSlot);
        neuronProgram.release();

        // bind texture of texture buffer to neuron shader program
        connectionProgram.bind();

        int sLoc = connectionProgram.uniformLocation(name);
        connectionProgram.setUniformValue("OnColor", onColor);
        connectionProgram.setUniformValue("OffColor", offColor);
        connectionProgram.setUniformValue(sLoc, textureSlot);
        connectionProgram.release();
    }


    AttributeOwner owner;
    QVector3D onColor,offColor;
    GLubyte * unboundData;
    QString name;
};


#endif // NCVATTRIBUTES_H
