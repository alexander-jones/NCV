#include "ncsconnectionset.h"


NCSConnectionSet::NCSConnectionSet(NCSNeuronSet *neurons,QVector<NCSConnection> connections,QObject *parent )
    :QObject(parent)
{
    m_neurons = neurons;
    m_connections = connections;
    m_count = connections.count();
}

void NCSConnectionSet::addAttribute(QString name,NCSAttribute * attribute)
{
    m_attributes[name] = attribute;
    attributeAdded(name,attribute);
}


void NCSConnectionSet::removeAttribute(QString name)
{
    m_attributes.remove(name);
    attributeRemoved(name);
}


QMap<QString, NCSAttribute *> NCSConnectionSet::attributes()
{
    return m_attributes;
}

int NCSConnectionSet::count()
{
    return m_count;
}

QVector<NCSConnection> NCSConnectionSet::connections()
{
    return m_connections;
}

NCSNeuronSet* NCSConnectionSet::neurons()
{
    return m_neurons;
}

