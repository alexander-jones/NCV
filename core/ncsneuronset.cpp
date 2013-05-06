#include "ncsneuronset.h"

NCSNeuronSet::NCSNeuronSet(QVector<QVector3D> positions,QObject *parent) :
    QObject(parent)
{
    m_positions = positions;
    m_count = m_positions.count();
}

QMap<QString,NCSAttribute *> NCSNeuronSet::attributes()
{
    return m_attributes;
}


QVector<QVector3D> NCSNeuronSet::positions()
{
    return m_positions;
}


void NCSNeuronSet::addAttribute(QString name, NCSAttribute * attribute)
{
    m_attributes[name] = attribute;
    attributeAdded(name,attribute);
}

void NCSNeuronSet::removeAttribute(QString name)
{
    m_attributes.remove(name);
    attributeRemoved(name);
}

int NCSNeuronSet::count()
{
    return m_count;
}

