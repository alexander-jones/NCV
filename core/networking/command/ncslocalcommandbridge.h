#ifndef NCSLOCALCOMMANDBRIDGE_H
#define NCSLOCALCOMMANDBRIDGE_H
#include "ncscommandbridge.h"
#include <QTimer>

class NCSLocalApplicationBridge:public NCSApplicationBridge
{
    Q_OBJECT
public:
    explicit NCSLocalApplicationBridge(QString name,QString workingDirectory = "",QObject *parent = 0);
    ~NCSLocalApplicationBridge();
    void start(QString application,NCSCommandArguments arguments);
    QString readAllStandardError();
    QString readAllStandardOutput();

    void scheduleDestruction(bool destroy);
    QString applicationName();
private slots:
    void m_processErrorOccured(QProcess::ProcessError);
    void m_processFinished();

private:
    QProcess * m_process;
    QString m_name;
    QString m_runProcessName;
    bool m_destroyProcess;
};

class NCSLocalCommandBridge : public NCSCommandBridge
{
    Q_OBJECT
public:
    explicit NCSLocalCommandBridge(QObject *parent = 0);
    void initialize(QString projectSubDir);
    void validate(QString path);
    void launchApplication(QString application, NCSCommandArguments arguments);
    void launchApplication(QString application, NCSCommandArguments arguments,int numProcesses, QString hostFile = "" );
    bool valid();
    QString hostname();

private:
    void m_invalidate(NCSCommandBridge::ValidationError err);

    QVector<NCSCommandFileArgument> m_downloadArguments;
    QString m_rootPath, m_buildPath, m_projectPath;
    NCSLocalApplicationBridge * m_applicationBridge;
    bool m_valid;
    QTimer m_timer;
};

#endif // NCSLOCALCOMMANDBRIDGE_H
