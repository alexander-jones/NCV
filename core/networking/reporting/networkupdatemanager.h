#ifndef NETWORKUPDATEMANAGER_H
#define NETWORKUPDATEMANAGER_H

#include <QObject>
#include "ncsdatasource.h"
#include "networkupdatethread.h"


/*!
  \class NetworkUpdateManager
  \author Justin Cardoza
  \brief This class manages network updates in one easy interface. Give it a data source and
         an update interval, and it will handle retrieving simulation data in a threaded manner.
  */
class NetworkUpdateManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkUpdateManager(QObject *parent = 0);
    
signals:
    void updateIntervalChanged(int msec);
    void updatesStarted();
    void updatesStopped();

public slots:
    void setDataSource(NCSDataSource *source);
    void setUpdateInterval(int msec);
    void startUpdates();
    void stopUpdates();

private:
    NCSDataSource *m_dataSource;
    NetworkUpdateThread *m_updateThread;
};


#endif // NETWORKUPDATEMANAGER_H
