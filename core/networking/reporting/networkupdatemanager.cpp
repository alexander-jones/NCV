#include "networkupdatemanager.h"
#include "core/ncscontinuousattribute.h"
#include "core/ncsdiscreteattribute.h"


NetworkUpdateManager::NetworkUpdateManager(QObject *parent) : QObject(parent)
{

    m_neurons = NULL;
    m_connections = NULL;
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(m_updateAttributes()));
}


NetworkUpdateManager::~NetworkUpdateManager()
{
    disconnectFromHost();
}

void NetworkUpdateManager::setUpdateInterval(int msec)
{
    m_timer->setInterval(msec);
}



void NetworkUpdateManager::startUpdates()
{
    m_timer->start();
}



void NetworkUpdateManager::stopUpdates()
{
    m_timer->stop();
}


bool NetworkUpdateManager::connectToHost(const std::string &host, int port)
{
    return m_client.connectToSimulator(host, port, 3);
}


void NetworkUpdateManager::disconnectFromHost()
{
    if (m_client.connected())
        m_client.disconnectFromSimulator();
}



void NetworkUpdateManager::setNeurons(NCSNeuronSet *neurons)
{
    QMap<QString, NCSAttribute*> attributes;

    if(m_neurons)
    {
        //Delete current reported attributes that match the ones being replaced.
        attributes = m_neurons->attributes();

        for(QMap<QString, NCSAttribute*>::iterator it = attributes.begin(); it != attributes.end(); it++)
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

        for(QMap<QString, NCSAttribute*>::iterator it = attributes.begin(); it != attributes.end(); it++)
        {
            if(it.value()->reportable())
                m_addAttribute(it.key(), it.value(), m_neurons->count());
        }
    }
}



void NetworkUpdateManager::setConnections(NCSConnectionSet *connections)
{
    QMap<QString, NCSAttribute*> attributes;

    if(m_connections)
    {
        //Delete current reported attributes that match the ones being replaced.
        attributes = m_connections->attributes();

        for(QMap<QString, NCSAttribute*>::iterator it = attributes.begin(); it != attributes.end(); it++)
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

        for(QMap<QString, NCSAttribute*>::iterator it = attributes.begin(); it != attributes.end(); it++)
        {
            if(it.value()->reportable())
                m_addAttribute(it.key(), it.value(), m_connections->count());
        }
    }
}



void NetworkUpdateManager::m_updateAttribute(NCSAttribute *attribute)
{
    if(m_reported.count(attribute) < 1)
        return;

    ReportedAttribute reportedAttribute = m_reported[attribute];
    std::vector<char> data;

    bool pullSuccess = reportedAttribute.report->pull(data, m_step);

    if(reportedAttribute.attribute->type() == Continuous)
    {
        std::vector<float> values;
        NCSContinuousAttribute *contAttr = dynamic_cast<NCSContinuousAttribute*>(reportedAttribute.attribute);
        reportedAttribute.report->permute(data, values);
        QVector<float>  qData = QVector<float>::fromStdVector(values);
        contAttr->attachData(qData);
    }
    else if(reportedAttribute.attribute->type() == Discrete)
    {
        //Note: this will just handle bit attributes for now, since the mechanism for
        //more complex discrete attributes doesn't exist in NCS yet. At some point we
        //should probably convert to NCS's bit array format to speed this update process
        //up a little. Let's see how this performs first though.
        std::vector<unsigned int> values;
        QVector<unsigned char> byteValues;
        NCSDiscreteAttribute *discAttr = dynamic_cast<NCSDiscreteAttribute*>(reportedAttribute.attribute);
        reportedAttribute.report->permute(data, values);
        byteValues.resize(values.size() * sizeof(unsigned int));

        for(int i = 0; i < m_neurons->count(); i++)
        {
            if(values[i >> 5] & (0x80000000 >> (i & 0x1F)))
            {
                byteValues[i >> 3] |= 1 << (i & 0x07);
            }
        }

        discAttr->attachData(byteValues);
    }
}//updateAttribute()



void NetworkUpdateManager::m_updateAttributes()
{
    if(NULL != m_neurons)
    {
        QMap<QString, NCSAttribute*> attributes = m_neurons->attributes();
        for(QMap<QString, NCSAttribute*>::iterator it = attributes.begin(); it != attributes.end(); it++)
        {
            if (it.value()->reportable())
                m_updateAttribute(it.value());
        }

    }

    if(NULL != m_connections)
    {
        QMap<QString, NCSAttribute*> attributes = m_connections->attributes();
        for(QMap<QString, NCSAttribute*>::iterator it = attributes.begin(); it != attributes.end(); it++)
        {
            if (it.value()->reportable())
                m_updateAttribute(it.value());
        }

    }
}//updateCurrentAttributes()



void NetworkUpdateManager::m_addAttribute(const QString& name, NCSAttribute *attribute, int elementCount)
{
    QReportClient::ReportRequest req;
    QReportClient::Report *report;
    ReportedAttribute ra;

    req.key = name.toStdString();
    req.dataspace = "device";
    req.frequency = 1;

    for(int i = 0; i < elementCount; i++)
        req.indices.push_back(i);

    if(attribute->elementType() == Neuron)
        req.elementType = "neuron";
    else if(attribute->elementType() == Connection)
        req.elementType = "synapse";

    if(attribute->type() == Continuous)
        req.datatype = "float";
    else if(attribute->type() == Discrete)
        req.datatype = "bit";

    report = m_client.report(req);
    if(NULL == report)
    {
        qDebug() << "Error setting up report for attribute " << name << ".\n";
        return;
    }

    ra.name = req.key;
    ra.attribute = attribute;
    ra.report = report;
    m_reported[attribute] = ra;
}//addAttribute()
