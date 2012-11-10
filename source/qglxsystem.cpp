
#include "qglxsystem.h"


QGLXSystem::QGLXSystem()
{
    m_primType = Triangle;
    m_program = NULL;
    m_LayoutType == Array;
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

void QGLXSystem::attachShaderProgram(QGLShaderProgram * program)
{
    m_program = program;
}


void QGLXSystem::setPrimitiveType(PrimitiveType type)
{
    m_primType = type;
}

int QGLXSystem::getComponentSize(GLenum componentType)
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



}
void QGLXSystem::setVertexAttributeArray(QString name,void * data, int stride, GLenum componentType, AttributeUsage usage )
{
    int componentSize = getComponentSize(componentType);

    QGLXBuffer * buffer = new QGLXBuffer();
    m_attributes[name] = AttributeArray(buffer,componentType,m_program->attributeLocation(name),stride,componentSize);
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
}




void QGLXSystem::setInstanceAttributeArray(QString name,void * data, int stride, GLenum componentType, int divisor, AttributeUsage usage )
{

    if (m_LayoutType == InstancedArray || m_LayoutType == InstancedElement)
    {
        int componentSize = getComponentSize(componentType);

        QGLXBuffer * buffer = new QGLXBuffer();
        m_attributes[name] = AttributeArray(buffer,componentType,m_program->attributeLocation(name),stride,componentSize,1);
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


void QGLXSystem::draw()
{
    drawSubset(0,m_numObjects);
}


void QGLXSystem::drawSubset(int start, int count)
{
    for (QMap<QString,AttributeArray>::iterator it =  m_attributes.begin(); it != m_attributes.end();it++)
    {
        AttributeArray attrib = it.value();
        QString name = it.key();
        attrib.buffer->bind();
        int offset = 0;
        if (attrib.divisor > 0)
            offset = start * m_instancesPerObject;
        m_program->setAttributeBuffer( attrib.location, attrib.componentType,  offset ,attrib.stride  / attrib.componentSize, attrib.stride);
        m_program->enableAttributeArray( attrib.location);
        glVertexAttribDivisor( attrib.location, attrib.divisor);
    }
    if (m_LayoutType ==  Element || m_LayoutType == InstancedElement)
        m_indexBuffer.bind();

     if (m_LayoutType == Array)
         glDrawArrays(m_primType,start * m_vertsPerObject,count  * m_vertsPerObject);
     else if (m_LayoutType == Element)
         glDrawElements(m_primType,count * m_elementsPerObject,GL_UNSIGNED_INT,0);
     else if (m_LayoutType == InstancedArray)
         glDrawArraysInstanced(m_primType,0,m_vertsPerObject,count* m_instancesPerObject);
     else if (m_LayoutType == InstancedElement)
         glDrawElementsInstanced(m_primType,m_elementsPerObject ,GL_UNSIGNED_INT,0,count * m_instancesPerObject );

     if (m_LayoutType ==  Element || m_LayoutType == InstancedElement)
         m_indexBuffer.release();

     for (QMap<QString,AttributeArray>::iterator it =  m_attributes.begin(); it != m_attributes.end();it++)
     {
         AttributeArray attrib = it.value();
         m_program->disableAttributeArray( attrib.location );
         attrib.buffer->release();
     }
}


void QGLXSystem::editAttributeArray(QString attribute,void * data, int index, int count)
{
    m_attributes[attribute].buffer->bind();
    m_attributes[attribute].buffer->write(index * m_attributes[attribute].stride ,data,count * m_attributes[attribute].stride);
    m_attributes[attribute].buffer->release();
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

