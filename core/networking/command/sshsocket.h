#ifndef SSHCONNECTION_H
#define SSHCONNECTION_H

#include <libssh/libssh.h>
#include <QByteArray>
#include <QThread>
#include <QVector>
#include <QFile>
#include <sys/stat.h>
#include <sys/types.h>
#include <QDebug>

class SSHSocket :public QThread
{
    Q_OBJECT
public:

    enum SSHSocketError
    {
        SessionCreationError,
        ChannelCreationError,
        SocketError,
        SCPChannelCreationError,
        SCPPullRequestError,
        SCPPushRequestError,
        SCPFileNotCreatedError,
        SCPReadError,
        SCPWriteError,
        PasswordAuthenticationFailedError,
        PublicKeyAuthenticationFailedError,
        ShellCreationError
    };

    explicit SSHSocket(QObject * parent = 0);

    ~SSHSocket();

    void connectToHost(QString host, int port =22);

    void disconnectFromHost();

    void login(QString user, QString password);

    void executeCommand(QString command);

    void pullFile(QString localPath, QString remotePath);

    void pushFile(QString localPath, QString remotePath);

    void setWorkingDirectory(QString path);

    QString user();

    QString host();

    int port();

    bool isConnected();

signals:
    void authenticated();
    void connected();
    void disconnected();
    void error(SSHSocket::SSHSocketError);
    void commandExecuted(QString command,QString response);
    void pullSuccessfull(QString localFile, QString remoteFile);
    void pushSuccessfull(QString localFile, QString remoteFile);
    void workingDirectorySet(QString cwd);

private:
    void run();

    enum SSHOperationType
    {
        Command,
        WorkingDirectoryTest,
        Pull,
        Push
    };

    struct SSHOperation
    {
        SSHOperationType type;
        QString adminCommand,command, localPath, remotePath;
    };

    void m_emitError(SSHSocketError err);

    int m_port;
    bool m_authenticated ;
    QString m_workingDirectory,m_nextWorkingDir,m_user, m_host,m_password;
    QVector<SSHOperation> m_operationsToExecute;
    ssh_session m_session;
    bool m_connected,m_run;
};


#endif // SSHCONNECTION_H
