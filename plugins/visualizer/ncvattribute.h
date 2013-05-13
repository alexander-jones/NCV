#ifndef NCVATTRIBUTE_H
#define NCVATTRIBUTE_H
#include "qglx/qglxcamera.h"
#include "core/ncsattribute.h"
#include <QGLShaderProgram>
#include <QObject>

class NCVAttribute: public QObject
{
    Q_OBJECT

public:
    NCVAttribute(NCSAttribute * parent):QObject() {m_parent = parent;}
    virtual void bind(QGLXCamera camera){Q_UNUSED(camera)}
    virtual void release(){}
    virtual bool dirty(){return false;}
    virtual NCSElementType elementType(){return m_parent->elementType();}
    virtual NCSAttributeType type(){return m_parent->type();}
    virtual void resolve(){}
    virtual void destroy() = 0;
    virtual bool reportable() const { return m_parent->reportable(); }

private:
    NCSAttribute * m_parent;
};
#endif // NCVATTRIBUTE_H
