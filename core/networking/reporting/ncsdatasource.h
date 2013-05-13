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
    bool establishConnection(const std::string& host, int port);
    void closeConnection();
    void replaceNeuronSet(NCSNeuronSet *neurons);
    void replaceConnectionSet(NCSConnectionSet *connections);
    void updateAttribute(NCSAttribute *attribute);
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
