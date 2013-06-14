#ifndef NCSDISCRETEATTRIBUTE_H
#define NCSDISCRETEATTRIBUTE_H
#include "ncsattribute.h"
#include <QObject>
#include <QVector>

class NCSDiscreteAttribute:public NCSAttribute
{
    Q_OBJECT
public:
    NCSDiscreteAttribute(NCSElementType type, QVector<QString> states,int bitsPerValue,QObject *parent = 0);
    void attachData(QVector<unsigned char> & data);
    QVector<unsigned char> data();
    QVector<QString> states();
    QVector<QString> stateValues();
    QVector<QString> stateValues(int startElement, int count);
    NCSElementType elementType();
    NCSAttributeType type(){return Discrete;}
    int bitsPerValue();
    int bitMask();


private:
    QVector<unsigned char> m_data;
    NCSElementType m_type;
    int m_bitsPerValue,m_bitMask;
    QVector<QString> m_states;
};

#endif // NCSDISCRETEATTRIBUTE_H
