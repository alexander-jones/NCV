#include "networkupdatethread.h"



NetworkUpdateThread::NetworkUpdateThread(QObject *parent) : QThread(parent)
{
    this->start();
}



void NetworkUpdateThread::setInterval(int msec)
{
    m_timer->setInterval(msec);
}



void NetworkUpdateThread::startUpdates()
{
    m_timer->start();
}



void NetworkUpdateThread::stopUpdates()
{
    m_timer->stop();
}



void NetworkUpdateThread::run()
{
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timerFired()));
    exec();
}



void NetworkUpdateThread::timerFired()
{
    emit updateRequested();
}
