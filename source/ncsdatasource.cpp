#include "ncsdatasource.h"

#include "ncvcontinuousattribute.h"
#include "ncvdiscreteattribute.h"



NCSDataSource::NCSDataSource() : QObject(0), m_neurons(0), m_connections(0)
{
}



NCSDataSource::~NCSDataSource()
{
    closeConnection();
}



bool NCSDataSource::establishConnection(const std::string &host, int port)
{
    return m_client.connect(host, port, 3);
}



void NCSDataSource::closeConnection()
{
    m_client.disconnect();
}



void NCSDataSource::replaceNeuronSet(NCVNeuronSet *neurons)
{
    QMap<QString, NCVAttribute*> attributes;

    if(m_neurons)
    {
        //In the future, stop any update threads that might be running.

        //Delete current reported attributes.
        for(QMap<NCVAttribute*, ReportedAttribute>::iterator it = m_reported.begin(); it != m_reported.end(); ++it)
        {
            delete it.value().report;
        }

        m_reported.clear();
        delete[] m_neurons;
    }

    m_neurons = neurons;

    if(m_neurons)
    {
        attributes = m_neurons->attributes();

        for(QMap<QString, NCVAttribute*>::iterator it = attributes.begin(); it != attributes.end(); it++)
        {
            addAttribute(it.key(), it.value());
        }
    }
}



void NCSDataSource::replaceConnectionSet(NCVConnectionSet *connections)
{
    QMap<QString, NCVAttribute*> attributes;

    if(m_connections)
        delete[] m_connections;

    m_connections = connections;

    if(m_connections)
    {
        attributes = m_connections->attributes();
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
    //How will discrete attributes work here?
}



void NCSDataSource::updateCurrentAttributes()
{
    if(NULL != m_neurons)
        updateAttribute(m_neurons->getCurrentAttribute());

    if(NULL != m_connections)
        updateAttribute(m_connections->getCurrentAttribute());
}



void NCSDataSource::addAttribute(const QString& name, NCVAttribute *ncvAttribute)
{
    slug::util::ReportRequest req;
    slug::sim::ReportClient::Report *report;
    ReportedAttribute ra;

    req.key = name.toStdString();
    req.dataspace = "device";
    req.frequency = 1;
    req.indices.push_back(1);
    req.indices.push_back(0);

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
}
