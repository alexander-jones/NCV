#ifndef NCSREMOTECOMMANDBRIDGE_H
#define NCSREMOTECOMMANDBRIDGE_H
#include "ncscommandbridge.h"
#include "qsshsocket.h"
class NCSRemoteApplicationBridge:public NCSApplicationBridge
{
    Q_OBJECT
public:
    explicit NCSRemoteApplicationBridge(QString name,QSshSocket * socket,QObject *parent = 0);
    ~NCSRemoteApplicationBridge();
    void start(QString application,QStringList arguments,QVector<NCSCommandFileArgument> downloadArgs);
    QString readAllStandardError();
    QString readAllStandardOutput();
    void scheduleDestruction(bool destroy);
    QString applicationName();

private slots:
    void m_onCommandExecuted(QString command,QString response);
    void m_onSocketError(QSshSocket::SshError err);
    void m_executeNextPull();
    void m_checkIfAlive();

private:
    QVector<NCSCommandFileArgument> m_downloadArguments;
    bool m_destroyProcess,m_alive;
    QSshSocket * m_socket;
    QString m_stdOut, m_stdErr,m_name, m_pidString;
    QTimer * m_timer;
};

class NCSRemoteCommandBridge : public NCSCommandBridge
{
    Q_OBJECT
public:
    explicit NCSRemoteCommandBridge( QObject *parent = 0);
    void initialize(QString projectSubDir, QSshSocket * socket);
    void validate(QString path);
    void executeApplication(QString application, NCSCommandArguments arguments);
    void executeApplication(QString application, NCSCommandArguments arguments,int numProcesses, QString hostFile = "" );
    bool valid();
    QString hostname();


private slots:
    void m_onCommandExecuted(QString command,QString response);
    void m_onSocketError(QSshSocket::SshError err);
    void m_executeNextPush();
    void m_socketDirectorySet(QString);
    void m_clearApplicationContext();
    void m_clearProjectContext();

private:

    QSshSocket * m_socket;
    NCSRemoteApplicationBridge * m_currentApplication;
    QVector<NCSCommandFileArgument> m_uploadArguments;
    QVector<NCSCommandFileArgument> m_downloadArguments;
    QString m_remoteRootPath, m_remoteBuildPath, m_projectPath,m_remoteProjectPath;
    QString m_application;
    QStringList m_applicationArguments;
    bool m_valid;
};

#endif // NCSREMOTECOMMANDBRIDGE_H
