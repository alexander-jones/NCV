#include "networkupdatemanager.h"



NetworkUpdateManager::NetworkUpdateManager(QObject *parent) : QObject(parent)
{
    m_updateThread = new NetworkUpdateThread(this);

    //Pass these on via signals for thread safety.
    connect(this, SIGNAL(updateIntervalChanged(int)), m_updateThread, SLOT(setInterval(int)));
    connect(this, SIGNAL(updatesStarted()), m_updateThread, SLOT(startUpdates()));
    connect(this, SIGNAL(updatesStopped()), m_updateThread, SLOT(stopUpdates()));
}



void NetworkUpdateManager::setDataSource(NCSDataSource *source)
{
    if(m_dataSource)
        disconnect(m_updateThread, SIGNAL(updateRequested()), m_dataSource, SLOT(updateCurrentAttributes()));

    m_dataSource = source;

    if(m_dataSource)
        connect(m_updateThread, SIGNAL(updateRequested()), m_dataSource, SLOT(updateCurrentAttributes()));
}



void NetworkUpdateManager::setUpdateInterval(int msec)
{
    emit updateIntervalChanged(msec);
}



void NetworkUpdateManager::startUpdates()
{
    emit updatesStarted();
}



void NetworkUpdateManager::stopUpdates()
{
    emit updatesStopped();
}
