#ifndef QGLXSYSTEM_H
#define QGLXSYSTEM_H

#include "qglxcore.h"

#include "qglxbuffer.h"
#include <QGLShaderProgram>
#include <map>


struct AttributeArray
{
    AttributeArray( )
    {
        this->componentType = 0;
        this->location = 0;
        this->buffer = NULL;
    }

    AttributeArray( QGLXBuffer * buffer, GLenum componentType, GLuint location,int stride, int componentSize, int divisor = 0)
    {
        this->buffer = buffer;
        this->componentType = componentType;
        this->location = location;
        this ->componentSize =componentSize;
        this->stride = stride;
        this->divisor = divisor;
    }

    QGLXBuffer * buffer;
    GLenum componentType;
    GLuint location;
    int stride,componentSize, divisor;

};


/*!
    \class QGLXSystem
    \author Alex Jones
    \brief Container object best used for manipulating a system of objects.
*/
class QGLXSystem
{
public:
    /*!
        \enum PrimitiveType
        \brief Type of primitive used by system for rendering.
    */
    enum PrimitiveType
    {
        Point = GL_POINTS,
        Line= GL_LINES,
        LineStrip= GL_LINE_STRIP,
        Triangle = GL_TRIANGLES,
        TriangleStrip = GL_TRIANGLE_STRIP,
        Quad = GL_QUADS
    };

    /*!
        \enum AttributeUsage
        \brief Describes whether an attribute of the system is going to be updated or static.
    */
    enum AttributeUsage
    {
        Dynamic,
        Static
    };

    enum LayoutType
    {
        Array,
        Element,
        InstancedArray,
        InstancedElement
    };

    /*!
        \brief Constructor. That is all.
    */
    QGLXSystem();


    /*!
        \brief Destructor. That is all.
    */
    ~QGLXSystem();

    /*!
        \param program A pointer to the program used to render this system.
        \brief This function attaches a shader to this system for rendering.
    */
    void attachShaderProgram(QGLShaderProgram * program);

    /*!
        \param numObjects The number of objects in this system.
        \param vertsPerObject How many vertices are described per object.
        \param elementsPerObject How many indices are described per object.
        \param instancesPerObject How many instances are described per object.
        \brief This function initializes the system by setting up object information.
    */
    void create(int numObjects, int vertsPerObject, int elementsPerObject,int instancesPerObject);


    /*!
        \brief This function draws all members of the system with all the currently created attributes.
    */
    void draw();


    /*!
        \param start The first object in the system to draw.
        \param count The number of objects to draw after start.
        \brief This function draws a subset of the members of the system.
    */
    void drawSubset(int start, int count);

    /*!
        \param attribute The name of the attribute array to edit.
        \param start What member to starting writing at.
        \param data The data to write.
        \param count The total number of elements to write.
        \brief This function edits an attribute array at a given location.
    */
    void editAttributeArray(QString attribute, void * data,int start, int count);

    /*!
        \brief This returns the number of objects.
    */
    int numObjects();

    /*!
        \brief This returns the type of primitive used for rendering.
    */
    PrimitiveType primitiveType();

    /*!
        \param type A base rendering primitive. (Triangle, Line, Point, Quad, etc..)
        \brief This function sets the primitive type used for rendering this system.
    */
    void setPrimitiveType(PrimitiveType type);

    /*!
        \param name The name to associate this attribute array with.
        \param data The data of this attribute array.
        \param stride The size in bytes of each element.
        \param componentType The opengl component type. (GL_INT, GL_FLOAT, GL_UNSIGNED_BYTE, ...)
        \param divisor  The number of instances that will pass between updates of this attribute.
        \param usage Whether or not the buffer will be dynamically or staticly used.
        \brief This function enables the information in data to be bound to a shader attribute named name.
        The elements of this data will be advanced per every divisor instances.
    */
    void setInstanceAttributeArray(QString name,void * data, int stride, GLenum componentType, int divisor, AttributeUsage usage = Static);


    /*!
        \param names The names to associate the attribute arrays with.
        \param data The data of these attribute arrays.
        \param strides The size in bytes of each element.
        \param componentTypes The opengl component type. (GL_INT, GL_FLOAT, GL_UNSIGNED_BYTE, ...)
        \param divisor  The number of instances that will pass between updates of this attribute.
        \param usage Whether or not the buffer will be dynamically or staticly used.
        \brief This function enables binding of multiple attributes stored in a single buffer.
        The elements of this data will be advanced per every divisor instances.
    */
    void setPackedInstanceAttributeArray(int num, QString * names,void * data, int * strides, GLenum * componentTypes, int divisor, AttributeUsage usage = Static);

    /*!
        \param name The name to associate this attribute array with.
        \param data The data of this attribute array.
        \param stride The size in bytes of each element.
        \param componentType The opengl component type. (GL_INT, GL_FLOAT, GL_UNSIGNED_BYTE, ...)
        \param usage Whether or not the buffer will be dynamically or staticly used.
        \brief This function enables the information in data to be bound to a shader attribute named name.
        The elements of data will be advanced per-vertex.
    */
    void setVertexAttributeArray(QString name,void * data, int stride, GLenum componentType, AttributeUsage usage = Static);

    /*!
        \param names The names to associate the attribute arrays with.
        \param data The data of these attribute arrays.
        \param strides The size in bytes of each element.
        \param componentTypes The opengl component type. (GL_INT, GL_FLOAT, GL_UNSIGNED_BYTE, ...)
        \param usage Whether or not the buffer will be dynamically or staticly used.
        \brief This function enables binding of multiple attributes stored in a single buffer.
        The elements of data will be advanced per-vertex.
    */
    void setPackedVertexAttributeArray(int num, QString * names,void * data, int * strides, GLenum * componentTypes, AttributeUsage usage = Static);


    /*!
        \param data The data to write.
        \param usage Whether or not the buffer will be dynamically or staticly used.
        \brief This function sets the index buffer for this system.
    */
    void setVertexAttributeIndices(GLuint * indices,AttributeUsage usage = Static);



private:
    int getComponentSize(GLenum componentType);
    LayoutType m_LayoutType;
    PrimitiveType m_primType;
    QGLShaderProgram * m_program;
    QGLXBuffer m_indexBuffer;
    int  m_numObjects, m_vertsPerObject,m_elementsPerObject,m_instancesPerObject;
    QMap<QString,AttributeArray> m_attributes;
};

#endif // QGLXSYSTEM_H
