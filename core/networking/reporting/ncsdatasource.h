#ifndef NCSDATASOURCE_H
#define NCSDATASOURCE_H

#include <QObject>
#include <QMap>
#include <string>

#include "qreportclient.h"
#include "core/ncsneuronset.h"
#include "core/ncsconnectionset.h"


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
    ///Establishes a connection to a simulation using a network socket.
    ///Takes a network host name and port to connect to.
    bool establishConnection(const std::string& host, int port);

    ///Closes a previously opened connection to a simulator.
    void closeConnection();

    ///Sets the neuron set used for storing neuron data, replacing whatever neuron set was used previously.
    void replaceNeuronSet(NCSNeuronSet *neurons);

    ///Sets the connection set used for storing connection data, replacing whatever connection set was used previously.
    void replaceConnectionSet(NCSConnectionSet *connections);

    ///Updates a specific attribute by pulling data from the report client.
    void updateAttribute(NCSAttribute *attribute);

    ///Updates all reportable attributes that are currently active.
    void updateAttributes();

private:
    struct ReportedAttribute
    {
        std::string name;
        QReportClient::Report *report;
        NCSAttribute *attribute;
    };

    void addAttribute(const QString& name, NCSAttribute *attribute, int elementCount);

    QReportClient m_client;
    NCSNeuronSet *m_neurons;
    NCSConnectionSet *m_connections;
    QMap<NCSAttribute*, ReportedAttribute> m_reported;
    unsigned int m_step;
};


#endif // NCSDATASOURCE_H
