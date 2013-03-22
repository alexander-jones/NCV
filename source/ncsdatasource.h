#ifndef NCSDATASOURCE_H
#define NCSDATASOURCE_H

#include <QObject>
#include <QMap>
#include <slug/sim/ReportClient.h>

#include <string>

#include "ncvneuronset.h"
#include "ncvconnectionset.h"


/*!
  \class NCSDataSource
  \author Justin Cardoza
  \brief The main class for receiving data from NCS over a network connection.
  */
class NCSDataSource : public QObject
{
    Q_OBJECT
public:
    NCSDataSource(QObject *parent = 0);
    virtual ~NCSDataSource();
    
signals:


public slots:
    bool establishConnection(const std::string& host, int port);
    void closeConnection();
    void replaceNeuronSet(NCVNeuronSet *neurons);
    void replaceConnectionSet(NCVConnectionSet *connections);
    void updateAttribute(NCVAttribute *attribute);
    void updateCurrentAttributes();

private:
    struct ReportedAttribute
    {
        std::string name;
        slug::sim::ReportClient::Report *report;
        NCVAttribute *ncvAttribute;
    };

    void addAttribute(const QString& name, NCVAttribute *ncvAttribute);

    slug::sim::ReportClient m_client;
    NCVNeuronSet *m_neurons;
    NCVConnectionSet *m_connections;
    QMap<NCVAttribute*, ReportedAttribute> m_reported;
    unsigned int m_step;
};


#endif // NCSDATASOURCE_H
