#ifndef NETWORKUPDATEMANAGER_H
#define NETWORKUPDATEMANAGER_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include "qreportclient.h"
#include "core/ncsneuronset.h"
#include "core/ncsconnectionset.h"


/*!
  \class NetworkUpdateManager
  \author Justin Cardoza
  \editor
  \brief This class manages network updates in one easy interface. Give it a data source and
         an update interval, and it will handle retrieving simulation data in a threaded manner.
  */
class NetworkUpdateManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkUpdateManager(QObject *parent = 0);
    ~NetworkUpdateManager();

signals:
    ///Emitted when the update interval changes.
    void updateIntervalChanged(int msec);

    ///Emitted when updates are started.
    void updatesStarted();

    ///Emitted when updates are stopped.
    void updatesStopped();


public slots:

    ///Establishes a connection to a simulation using a network socket.
    ///Takes a network host name and port to connect to.
    bool connectToHost(const std::string& host, int port);

    ///Closes a previously opened connection to a simulator.
    void disconnectFromHost();

    ///Sets the neuron set used for storing neuron data, replacing whatever neuron set was used previously.
    void setNeurons(NCSNeuronSet *neurons);

    ///Sets the connection set used for storing connection data, replacing whatever connection set was used previously.
    void setConnections(NCSConnectionSet *connections);

    ///Sets the update interval in milliseconds.
    void setUpdateInterval(int msec);

    ///Starts the update timer.
    void startUpdates();

    ///Stops the update timer.
    void stopUpdates();

private slots:
    ///Updates a specific attribute by pulling data from the report client.
    void m_updateAttribute(NCSAttribute *attribute);

    ///Updates all reportable attributes that are currently active.
    void m_updateAttributes();

private:
    struct ReportedAttribute
    {
        std::string name;
        QReportClient::Report *report;
        NCSAttribute *attribute;
    };

    void m_addAttribute(const QString& name, NCSAttribute *attribute, int elementCount);

    QReportClient m_client;
    NCSNeuronSet *m_neurons;
    NCSConnectionSet *m_connections;
    QMap<NCSAttribute*, ReportedAttribute> m_reported;
    unsigned int m_step;
    QTimer * m_timer;
};


#endif // NETWORKUPDATEMANAGER_H
