#ifndef NETWORKUPDATETHREAD_H
#define NETWORKUPDATETHREAD_H

#include <QThread>
#include <QTimer>


/*!
  \class NetworkUpdateThread
  \author Justin Cardoza
  \brief A special type of thread that starts a separate event loop, passes messages to a
         private timer object, and emits signals at a requested interval independent of
         any other threads or event loops currently running.
  */
class NetworkUpdateThread : public QThread
{
    Q_OBJECT
public:
    explicit NetworkUpdateThread(QObject *parent = 0);
    
signals:
    void updateRequested();

public slots:
    void setInterval(int msec);
    void startUpdates();
    void stopUpdates();

protected:
    void run();

private slots:
    void timerFired();

private:
    QTimer *m_timer;
};


#endif // NETWORKUPDATETHREAD_H
