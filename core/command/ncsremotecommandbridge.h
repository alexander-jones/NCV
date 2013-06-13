#ifndef NCSREMOTECOMMANDBRIDGE_H
#define NCSREMOTECOMMANDBRIDGE_H
#include "core/ncscommandbridge.h"
#include "ncsremoteapplicationbridge.h"
#include "ncsinternalcommandbridge.h"

class NCSRemoteCommandBridge : public NCSInternalCommandBridge
{
    Q_OBJECT
public:
    explicit NCSRemoteCommandBridge( QSshSocket * socket,QObject *parent = 0);
    QString hostname();
    void launchApplicationBridge(QString application, NCSCommandArguments arguments);
    void launchApplicationBridge(QString application, NCSCommandArguments arguments,int numProcesses, QString hostFile = "" );
    void queryApplication(QString applicationName);
    void queryPlugin(NCSCommandBridge::PluginType type,QString pluginName);
    void queryReader(QString readerName);
    void validate(QString path);
    bool valid();


private slots:
    void m_onApplicationQueried(QString command,QString stdOut,QString stdError);
    void m_onPluginQueried(QString command,QString stdOut,QString stdError);
    void m_onReaderQueried(QString command,QString stdOut,QString stdError);
    void m_onSocketCloned(QSshSocket * applicationSocket);
    void m_onCommandExecuted(QString command,QString stdOut,QString stdError);
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

    struct queryContext
    {
        queryContext(QString n = "")
        {
            name = n;
            builtChecked = false;
        }

        QString name;
        bool builtChecked;
    };

    QVector<ApplicationContext> m_launchingApplications;
    QSshSocket * m_socket;
    QString m_remoteRootPath, m_remoteBuildPath,m_remoteProjectPath;
    queryContext m_pluginToquery, m_applicationToquery,m_readerToquery;
    bool m_valid;
};

#endif // NCSREMOTECOMMANDBRIDGE_H
