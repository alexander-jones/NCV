#ifndef NCVCORE_H
#define NCVCORE_H

#include "qglxbuffer.h"
#include "qglxcamera.h"


enum NCVElementType
{
    Neuron,
    Connection,
    Column,
    Layer
};
enum NCVAttributeType
{
    Discrete,
    Continuous
};


struct Range
{
    int start;
    int end;
};

class NCVAttribute
{
public:
    NCVAttribute(){}
    virtual QGLShaderProgram * program(){return NULL;}
    virtual void bind(QGLXCamera camera){Q_UNUSED(camera)}
    virtual void release(){}
    virtual NCVElementType elementType(){return Neuron;}
    virtual NCVAttributeType type(){return Continuous;}
    virtual void resolve(){}
};

class NCVElementSet: public QObject
{
    Q_OBJECT
public:
    NCVElementSet(){}

    virtual void bind(QGLXCamera camera){Q_UNUSED(camera)}
    virtual void bindSilhouettes(QGLXCamera camera){Q_UNUSED(camera)}
    virtual int count(){return 0;}
    virtual void draw(){}
    virtual void drawSubset(int, int){}
    virtual void release(){}
    virtual void releaseSilhouettes(){}
    virtual QMap<QString,NCVAttribute *> attributes(){return QMap<QString,NCVAttribute *>();}
public slots:
    virtual void addAttribute(QString , NCVAttribute * ){}
    virtual void setCurrentAttribute(QString, NCVAttribute * ){}
    virtual void removeAttribute(QString , NCVAttribute * ){}


};

#endif // NCVCORE_H
