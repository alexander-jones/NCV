#ifndef NCSREMOTECOMMANDBRIDGE_H
#define NCSREMOTECOMMANDBRIDGE_H
#include "ncscommandbridge.h"
#include "qsshsocket.h"

class NCSRemoteApplicationBridge:public NCSApplicationBridge
{
    Q_OBJECT
public:
    explicit NCSRemoteApplicationBridge(QString name,QString projectDirectory, QObject *parent = 0);
    ~NCSRemoteApplicationBridge();
    void setSocket(QSshSocket * socket,bool own = false);
    void start(QString application,NCSCommandArguments arguments);
    QString readAllStandardError();
    QString readAllStandardOutput();
    void scheduleDestruction(bool destroy);
    QString applicationName();

private slots:
    void m_onCommandExecuted(QString command,QString response);
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

class NCSRemoteCommandBridge : public NCSCommandBridge
{
    Q_OBJECT
public:
    explicit NCSRemoteCommandBridge( QObject *parent = 0);
    void initialize(QString projectSubDir, QSshSocket * socket);
    void validate(QString path);
    void launchApplication(QString application, NCSCommandArguments arguments);
    void launchApplication(QString application, NCSCommandArguments arguments,int numProcesses, QString hostFile = "" );
    bool valid();
    QString hostname();


private slots:
    void m_onSocketCloned(QSshSocket * applicationSocket);
    void m_onCommandExecuted(QString command,QString response);
    void m_onSocketError(QSshSocket::SshError err);
    void m_socketDirectorySet(QString);
    void m_clearProjectContext();

private:

    struct ApplicationContext
    {
        QString applicationPath;
        NCSCommandArguments arguments;
        NCSRemoteApplicationBridge * application;
    };
    QVector<ApplicationContext> m_launchingApplications;
    QSshSocket * m_socket;
    QString m_remoteRootPath, m_remoteBuildPath, m_projectPath,m_remoteProjectPath;
    bool m_valid;
};

#endif // NCSREMOTECOMMANDBRIDGE_H
