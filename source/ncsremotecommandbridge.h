#ifndef NCSREMOTECOMMANDBRIDGE_H
#define NCSREMOTECOMMANDBRIDGE_H
#include "ncscommandbridge.h"
#include "sshsocket.h"
class NCSRemoteApplicationBridge:public NCSApplicationBridge
{
    Q_OBJECT
public:
    explicit NCSRemoteApplicationBridge(SSHSocket * socket,QString workingDirectory = "",QObject *parent = 0);
    ~NCSRemoteApplicationBridge();
    void start(QString application,QStringList arguments);
    QString readAllStandardError();
    QString readAllStandardOutput();

private slots:
    void m_onCommandExecuted(QString command,QString response);
    void m_onSocketError(SSHSocket::SSHSocketError err);

private:
    SSHSocket * m_socket;
    QString stdOut, stdErr;
};

class NCSRemoteCommandBridge : public NCSCommandBridge
{
    Q_OBJECT
public:
    explicit NCSRemoteCommandBridge( QObject *parent = 0);
    void initialize(QString projectSubDir, SSHSocket * socket);
    void validate(QString path);
    NCSApplicationBridge * executeApplication(QString application, NCSCommandArguments arguments);
    NCSApplicationBridge * executeApplication(QString application, NCSCommandArguments arguments,int numProcesses, QString hostFile = "" );
    bool valid();

private slots:
    void m_onCommandExecuted(QString command,QString response);
    void m_onSocketError(SSHSocket::SSHSocketError err);
    void m_executeNextPull();
    void m_executeNextPush();

private:
    void m_clearApplicationContext();
    void m_clearProjectContext();

    SSHSocket * m_socket;
    NCSRemoteApplicationBridge * m_currentApplication;
    QVector<NCSCommandFileArgument> m_uploadArguments;
    QVector<NCSCommandFileArgument> m_downloadArguments;
    QString m_remoteRootPath, m_remoteBuildPath, m_projectPath,m_remoteProjectPath;
    QString m_application;
    QStringList m_applicationArguments;
    bool m_valid;
};

#endif // NCSREMOTECOMMANDBRIDGE_H
