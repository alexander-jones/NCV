#ifndef NCSATTRIBUTE_H
#define NCSATTRIBUTE_H

#include <QObject>

enum NCSElementType
{
    Neuron,
    Connection,
    Grouping
};

enum NCSAttributeType
{
    Discrete,
    Continuous
};

class NCSAttribute: public QObject
{
    Q_OBJECT
public:
    NCSAttribute(QObject * parent  =0);
    virtual NCSElementType elementType(){return Neuron;}
    virtual NCSAttributeType type(){return Continuous;}
    virtual bool reportable() const;
    virtual void setReportable(bool value);

signals:
    void dataChanged();

private:
    bool m_reportable;
};

#endif // NCSATTRIBUTE_H
