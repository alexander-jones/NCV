#include "ncsdiscreteattribute.h"
#include "math.h"
NCSDiscreteAttribute::NCSDiscreteAttribute(NCSElementType type, QVector<QString> states,int bitsPerValue,QObject * parent)
    :NCSAttribute(parent)
{
    m_states = states;
    m_type =type;
    m_bitsPerValue = bitsPerValue;
    m_bitMask = 0;
    for (int i = 0;  i < m_bitsPerValue; i ++)
        m_bitMask += (int)pow((double)2,(double)i);

}

int NCSDiscreteAttribute::bitMask()
{
    return m_bitMask;
}

void NCSDiscreteAttribute::attachData(QVector<unsigned char> & data )
{
    m_data = data;
    dataChanged();
}

NCSElementType NCSDiscreteAttribute::elementType()
{
    return m_type;
}


int NCSDiscreteAttribute::bitsPerValue()
{
    return m_bitsPerValue;
}

QVector<QString> NCSDiscreteAttribute::states()
{
    return m_states;
}

QVector<QString> NCSDiscreteAttribute::stateValues()
{
    return stateValues(0,(m_data.count() *m_bitsPerValue) /8);
}

QVector<QString> NCSDiscreteAttribute::stateValues(int firstElement, int count)
{
    QVector<QString> unpacked;
    int endElement = firstElement + count-1;
    for (int i = firstElement; i <= endElement; i ++)
    {
        unsigned char val = m_data[(i *m_bitsPerValue) /8] & (unsigned char) (m_bitMask << (firstElement * m_bitsPerValue) % 8);
        val = qMin(val,(unsigned char)(m_states.count() -1));

        if ( (i+1)* (m_bitsPerValue /8) >= m_data.size() && (endElement % 8) < (firstElement % 8))
        {
            unsigned char addition = m_data[(i *m_bitsPerValue) /8] & (unsigned char) (m_bitMask >> (m_bitsPerValue - (endElement % 8)));
            val += qMin(addition,(unsigned char)(m_states.count() -1));

        }
        unpacked.append(m_states[val]);
    }
    return unpacked;
}

QVector<unsigned char> NCSDiscreteAttribute::data()
{
    return m_data;
}
