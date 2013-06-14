#ifndef SLEEPER_H
#define SLEEPER_H

#include <QThread>


class Sleeper : public QThread
{
public:
    static void usleep(unsigned long us) { QThread::usleep(us); }
    static void msleep(unsigned long ms) { QThread::msleep(ms); }
    static void sleep(unsigned long s) { QThread::sleep(s); }
};


#endif // SLEEPER_H
