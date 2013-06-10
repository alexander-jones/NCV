#include "ncscontinuousattribute.h"

NCSContinuousAttribute::NCSContinuousAttribute( NCSElementType type, float minValue , float maxValue,QObject * parent)
    :NCSAttribute(parent)
{
    m_minValue = minValue;
    m_maxValue = maxValue;
    m_type = type;

}
void NCSContinuousAttribute::attachData(QVector<float> & data)
{
    m_data = data;
    dataChanged();
}

NCSElementType NCSContinuousAttribute::elementType()
{
    return m_type;
}


float NCSContinuousAttribute::minValue()
{
    return m_minValue;
}

float NCSContinuousAttribute::maxValue()
{
    return m_maxValue;
}

QVector<float> NCSContinuousAttribute::data()
{
    return m_data;
}

