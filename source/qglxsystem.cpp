
#include "qglxsystem.h"
#include "qglxtexture.h"


QGLXSystem::QGLXSystem()
{
    m_primType = Triangle;
    m_LayoutType = Array;
    m_numObjects = 0;
    m_vertsPerObject = 0;
    m_elementsPerObject = 0;
    m_instancesPerObject = 0;
}

QGLXSystem::~QGLXSystem()
{
    for (QMap<QString,AttributeArray>::iterator it =  m_attributes.begin(); it != m_attributes.end();it++)
    {
        AttributeArray attrib = it.value();
        attrib.buffer->destroy();
    }
}


void QGLXSystem::create(int numObjects, int vertsPerObject, int elementsPerObject,int instancesPerObject)
{
    m_numObjects = numObjects;
    m_vertsPerObject = vertsPerObject;
    m_elementsPerObject = elementsPerObject;
    m_instancesPerObject = instancesPerObject;

    if (elementsPerObject >0)
    {
        if (m_instancesPerObject > 0)
            m_LayoutType = InstancedElement;

        else
            m_LayoutType = Element;
    }
    else
    {
        if (m_instancesPerObject > 0)
            m_LayoutType = InstancedArray;

        else
            m_LayoutType = Array;
    }
}

void QGLXSystem::setPrimitiveType(PrimitiveType type)
{
    m_primType = type;
}


void QGLXSystem::setVertexAttributeArray(QString name,void * data, int stride, GLenum componentType, AttributeUsage usage )
{
    int componentSize = QGLXTexture::getComponentSize(componentType);

    QGLXBuffer * buffer = new QGLXBuffer();
    m_attributes[name] = AttributeArray(buffer,componentType,stride,componentSize);
    m_attributes[name].buffer->create();

    if (usage == Static)
        m_attributes[name].buffer->setUsagePattern( QGLXBuffer::StaticDraw );
    else
        m_attributes[name].buffer->setUsagePattern( QGLXBuffer::DynamicDraw );
    m_attributes[name].buffer->bind();
    if (m_LayoutType == Array|| m_LayoutType == Element )
        m_attributes[name].buffer->allocate( data, m_numObjects * m_vertsPerObject * stride);
    else
        m_attributes[name].buffer->allocate( data, m_vertsPerObject * stride);
    m_attributes[name].buffer->release();
    m_attributes[name].divisor = 0;
}



void QGLXSystem::setInstanceAttributeArray(QString name,void * data, int stride, GLenum componentType, int divisor, AttributeUsage usage )
{

    if (m_LayoutType == InstancedArray || m_LayoutType == InstancedElement)
    {
        int componentSize = QGLXTexture::getComponentSize(componentType);

        QGLXBuffer * buffer = new QGLXBuffer();
        m_attributes[name] = AttributeArray(buffer,componentType,stride,componentSize,1);
        m_attributes[name].buffer->create();

        if (usage == Static)
            m_attributes[name].buffer->setUsagePattern( QGLXBuffer::StaticDraw );
        else
            m_attributes[name].buffer->setUsagePattern( QGLXBuffer::DynamicDraw );

        m_attributes[name].buffer->bind();
        m_attributes[name].buffer->allocate( data, (m_numObjects/ divisor) * stride);
        m_attributes[name].buffer->release();

    }
}


void QGLXSystem::bind(QGLShaderProgram * program)
{
    m_currentProgram = program;
    if (m_LayoutType ==  Element || m_LayoutType == InstancedElement)
        m_indexBuffer.bind();
    for (QMap<QString,AttributeArray>::iterator it =  m_attributes.begin(); it != m_attributes.end();it++)
    {
        QString name = it.key();
        AttributeArray attrib = it.value();
        attrib.buffer->bind();
        int location = program->attributeLocation(name);
        program->setAttributeBuffer( location, attrib.componentType,  0 ,attrib.stride  / attrib.componentSize, attrib.stride);
        program->enableAttributeArray( location);
        glVertexAttribDivisor( location, attrib.divisor);
    }

}

void QGLXSystem::bindAttributeToShader(QString key, QGLShaderProgram * program,const QString & shaderAttributeName)
{
    if (m_attributes.contains(key))
    {
        AttributeArray attrib = m_attributes[key];
        attrib.buffer->bind();
        int location = program->attributeLocation(shaderAttributeName);
        program->setAttributeBuffer( location, attrib.componentType,  0 ,attrib.stride  / attrib.componentSize, attrib.stride);
        program->enableAttributeArray( location);
        glVertexAttribDivisor( location, attrib.divisor);
    }
}
void QGLXSystem::releaseAttributeFromShader(QString key, QGLShaderProgram * program,const QString & shaderAttributeName)
{
    if (m_attributes.contains(key))
    {
        AttributeArray attrib = m_attributes[key];
        attrib.buffer->bind();
        int location = program->attributeLocation(shaderAttributeName);
        program->disableAttributeArray( location );
        attrib.buffer->release();
    }
}

void QGLXSystem::release(QGLShaderProgram * program)
{

    for (QMap<QString,AttributeArray>::iterator it =  m_attributes.begin(); it != m_attributes.end();it++)
    {
        QString name = it.key();
        AttributeArray attrib = it.value();
        int location = program->attributeLocation(name);
        program->disableAttributeArray( location );
        attrib.buffer->release();
    }
    if (m_LayoutType ==  Element || m_LayoutType == InstancedElement)
        m_indexBuffer.release();
}

void QGLXSystem::drawSubset(GLuint start, GLuint count)
{
     if (m_LayoutType == Array)
         glDrawArrays(m_primType,start * m_vertsPerObject,count  * m_vertsPerObject);
     else if (m_LayoutType == Element)
         glDrawElementsBaseVertex(m_primType,count * m_elementsPerObject,GL_UNSIGNED_INT,0,start * m_elementsPerObject);
     else if (m_LayoutType == InstancedArray)
         glDrawArraysInstancedBaseInstance(m_primType,0,m_vertsPerObject,count* m_instancesPerObject, start * m_instancesPerObject);
     else if (m_LayoutType == InstancedElement)
         glDrawElementsInstancedBaseInstance(m_primType,m_elementsPerObject ,GL_UNSIGNED_INT,0,count * m_instancesPerObject, start * m_instancesPerObject );

}


void QGLXSystem::draw()
{
    drawSubset(0,m_numObjects);
}

void * QGLXSystem::mapAttributeArray(QString attribute, QGLXBuffer::Access access)
{
   return  m_attributes[attribute].buffer->map(access);
}

void QGLXSystem::unmapAttributeArray(QString attribute)
{
    m_attributes[attribute].buffer->unmap();
}

QGLXSystem::PrimitiveType QGLXSystem::primitiveType()
{
    return m_primType;
}
void QGLXSystem::setVertexAttributeIndices(GLuint * indices,AttributeUsage usage )
{
    if (m_LayoutType == Element || m_LayoutType == InstancedElement)
    {
        m_indexBuffer = QGLXBuffer(QGLXBuffer::IndexBuffer);
        m_indexBuffer.create();
        if (usage == Static)
            m_indexBuffer.setUsagePattern( QGLXBuffer::StaticDraw );
        else
            m_indexBuffer.setUsagePattern( QGLXBuffer::DynamicDraw );
        // allocate memory for instances
        m_indexBuffer.bind();
        if (m_LayoutType == Element )
            m_indexBuffer.allocate( indices, sizeof(GLuint) * m_numObjects * m_elementsPerObject );
        else
            m_indexBuffer.allocate( indices, sizeof(GLuint) * m_elementsPerObject );

        m_indexBuffer.release();
    }
}


int QGLXSystem::numObjects()
{
    return m_numObjects ;
}

