#ifndef NCSCONTINUOUSATTRIBUTE_H
#define NCSCONTINUOUSATTRIBUTE_H
#include "ncsattribute.h"
#include <QVector>

class NCSContinuousAttribute: public NCSAttribute
{
    Q_OBJECT
public:
    NCSContinuousAttribute( NCSElementType type, float minValue , float maxValue,QObject *parent = 0);
    void attachData(QVector<float> & data);
    QVector<float> data();
    NCSElementType elementType();
    float minValue();
    float maxValue();
    NCSAttributeType type(){return Continuous;}


private:
    NCSElementType m_type;
    float m_minValue, m_maxValue;
    QVector<float> m_data;
};
#endif // NCSCONTINUOUSATTRIBUTE_H
