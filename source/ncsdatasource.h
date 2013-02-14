#ifndef NCSDATASOURCE_H
#define NCSDATASOURCE_H

#include <QObject>
#include <slug/sim/ReportClient.h>

#include <string>
#include <vector>


/*!
  \class NCSDataSource
  \author Justin Cardoza
  \brief The main class for retrieving data from NCS over a network connection.
  */
class NCSDataSource : public QObject
{
    Q_OBJECT
public:
    explicit NCSDataSource(const std::string& host, int port);
    virtual ~NCSDataSource();
    
signals:
    void neuronFiringsUpdated(QString attribName, GLuint *data);
    void neuronVoltagesUpdated(QString attribName, GLfloat *data);

public slots:
    void updateFirings();
    void updateVoltages();

private:
    slug::sim::ReportClient client;
    slug::sim::ReportClient::Report *report;
    unsigned int step;
    std::vector<char> data;
    std::vector<float> voltages;
};


#endif // NCSDATASOURCE_H
