#include "ncsdatasource.h"

#include "ncvcontinuousattribute.h"
#include "ncvdiscreteattribute.h"



NCSDataSource::NCSDataSource(QObject *parent) : QObject(parent), m_neurons(0), m_connections(0)
{
}



NCSDataSource::~NCSDataSource()
{
    closeConnection();
}



bool NCSDataSource::establishConnection(const std::string &host, int port)
{
    return m_client.connectToSimulator(host, port, 3);
}



void NCSDataSource::closeConnection()
{
    m_client.disconnectFromSimulator();
}



void NCSDataSource::replaceNeuronSet(NCVNeuronSet *neurons)
{
    QMap<QString, NCVAttribute*> attributes;

    if(m_neurons)
    {
        //Delete current reported attributes that match the ones being replaced.
        attributes = m_neurons->attributes();

        for(QMap<QString, NCVAttribute*>::iterator it = attributes.begin(); it != attributes.end(); it++)
        {
            delete m_reported[it.value()].report;
            m_reported.remove(it.value());
        }

        //delete m_neurons;
    }

    m_neurons = neurons;

    if(m_neurons)
    {
        attributes = m_neurons->attributes();

        for(QMap<QString, NCVAttribute*>::iterator it = attributes.begin(); it != attributes.end(); it++)
        {
            if(it.value()->reportable())
                addAttribute(it.key(), it.value(), neurons->count());
        }
    }
}



void NCSDataSource::replaceConnectionSet(NCVConnectionSet *connections)
{
    QMap<QString, NCVAttribute*> attributes;

    if(m_connections)
    {
        //Delete current reported attributes that match the ones being replaced.
        attributes = m_neurons->attributes();

        for(QMap<QString, NCVAttribute*>::iterator it = attributes.begin(); it != attributes.end(); it++)
        {
            delete m_reported[it.value()].report;
            m_reported.remove(it.value());
        }

        //delete m_connections;
    }

    m_connections = connections;

    if(m_connections)
    {
        attributes = m_connections->attributes();

        for(QMap<QString, NCVAttribute*>::iterator it = attributes.begin(); it != attributes.end(); it++)
        {
            if(it.value()->reportable())
                addAttribute(it.key(), it.value(), connections->count());
        }
    }
}



void NCSDataSource::updateAttribute(NCVAttribute *attribute)
{
    if(m_reported.count(attribute) < 1)
        return;

    ReportedAttribute reportedAttribute = m_reported[attribute];
    std::vector<char> data;

    reportedAttribute.report->pull(data, m_step);

    if(reportedAttribute.ncvAttribute->type() == Continuous)
    {
        std::vector<float> values;
        NCVContinuousAttribute *contAttr = dynamic_cast<NCVContinuousAttribute*>(reportedAttribute.ncvAttribute);
        reportedAttribute.report->permute(data, values);
        contAttr->attachData(QVector<GLfloat>::fromStdVector(values));
    }
    else if(reportedAttribute.ncvAttribute->type() == Discrete)
    {
        //Note: this will just handle bit attributes for now, since the mechanism for
        //more complex discrete attributes doesn't exist in NCS yet. At some point we
        //should probably convert to NCS's bit array format to speed this update process
        //up a little. Let's see how this performs first though.
        std::vector<unsigned int> values;
        QVector<GLubyte> byteValues;
        NCVDiscreteAttribute *discAttr = dynamic_cast<NCVDiscreteAttribute*>(reportedAttribute.ncvAttribute);
        reportedAttribute.report->permute(data, values);
        byteValues.resize(values.size() * sizeof(unsigned int));

        for(int i = 0; i < m_neurons->count(); i++)
        {
            if(values[i >> 5] & (0x80000000 >> (i & 0x1F)))
            {
                byteValues[i >> 3] |= 1 << (i & 0x07);
            }
        }

        discAttr->attachData(byteValues, 1);
    }
}//updateAttribute()



void NCSDataSource::updateCurrentAttributes()
{
    if(NULL != m_neurons)
        updateAttribute(m_neurons->getCurrentAttribute());

    if(NULL != m_connections)
        updateAttribute(m_connections->getCurrentAttribute());
}//updateCurrentAttributes()



void NCSDataSource::addAttribute(const QString& name, NCVAttribute *ncvAttribute, int elementCount)
{
    QReportClient::ReportRequest req;
    QReportClient::Report *report;
    ReportedAttribute ra;

    req.key = name.toStdString();
    req.dataspace = "device";
    req.frequency = 1;

    for(int i = 0; i < elementCount; i++)
        req.indices.push_back(i);

    if(ncvAttribute->elementType() == Neuron)
        req.elementType = "neuron";
    else if(ncvAttribute->elementType() == Connection)
        req.elementType = "synapse";

    if(ncvAttribute->type() == Continuous)
        req.datatype = "float";
    else if(ncvAttribute->type() == Discrete)
        req.datatype = "bit";

    report = m_client.report(req);
    if(NULL == report)
    {
        qDebug() << "Error setting up report for attribute " << name << ".\n";
        return;
    }

    ra.name = req.key;
    ra.ncvAttribute = ncvAttribute;
    ra.report = report;
    m_reported[ncvAttribute] = ra;
}//addAttribute()
