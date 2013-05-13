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
    ///Emitted when the update interval changes.
    void updateIntervalChanged(int msec);

    ///Emitted when updates are started.
    void updatesStarted();

    ///Emitted when updates are stopped.
    void updatesStopped();

public slots:
    ///Sets the data source used to fetch updates.
    void setDataSource(NCSDataSource *source);

    ///Sets the update interval in milliseconds.
    void setUpdateInterval(int msec);

    ///Starts the update timer.
    void startUpdates();

    ///Stops the update timer.
    void stopUpdates();

private:
    NCSDataSource *m_dataSource;
    NetworkUpdateThread *m_updateThread;
};


#endif // NETWORKUPDATEMANAGER_H
