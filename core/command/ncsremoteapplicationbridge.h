#ifndef NCSREMOTEAPPLICATIONBRIDGE_H
#define NCSREMOTEAPPLICATIONBRIDGE_H
#include "qsshsocket.h"
#include "core/ncsapplicationbridge.h"

class NCSRemoteApplicationBridge:public NCSApplicationBridge
{
    Q_OBJECT
public:
    explicit NCSRemoteApplicationBridge(QString name,QString projectDirectory, QObject *parent = 0);
    virtual ~NCSRemoteApplicationBridge();
    void setSocket(QSshSocket * socket,bool own = false);
    void start(QString application,NCSCommandArguments arguments);
    QString readAllStandardError();
    QString readAllStandardOutput();
    void stopExecution(bool destroy);
    QString applicationName();

private slots:
    void m_onCommandExecuted(QString command,QString stdOut,QString stdError);
    void m_onSocketError(QSshSocket::SshError err);
    void m_executeNextPull();
    void m_executeNextPush();
    void m_checkIfAlive();

private:
    QVector<NCSCommandFileArgument> m_uploadArguments;
    QVector<NCSCommandFileArgument> m_downloadArguments;
    bool m_destroyProcess,m_alive;
    QSshSocket * m_socket;
    QString m_stdOut, m_stdErr,m_name, m_pidString, m_projectDir, m_wholeCommand;
    QTimer * m_timer;
};

#endif // NCSREMOTEAPPLICATIONBRIDGE_H
