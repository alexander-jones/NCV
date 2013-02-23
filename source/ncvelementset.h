#ifndef NCVELEMENT_H
#define NCVELEMENT_H
#include <QMatrix4x4>
#include <QGLShaderProgram>

class NCVElement
{
public:
    NCVElement();
    virtual int startID();
    virtual int endID();
    virtual QMatrix4x4 scale();
    virtual int bind(QGLShaderProgram program);
    virtual int release();

};

#endif // NCVELEMENTSET_H
