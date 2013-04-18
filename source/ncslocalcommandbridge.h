#ifndef NCSLOCALCOMMANDBRIDGE_H
#define NCSLOCALCOMMANDBRIDGE_H
#include "ncscommandbridge.h"
#include <QTimer>

class NCSLocalApplicationBridge:public NCSApplicationBridge
{
    Q_OBJECT
public:
    explicit NCSLocalApplicationBridge(QString workingDirectory = "",QObject *parent = 0);
    ~NCSLocalApplicationBridge();
    void start(QString application,QStringList arguments);
    QString readAllStandardError();
    QString readAllStandardOutput();

private slots:
    void m_processErrorOccured(QProcess::ProcessError);
    void m_processFinished();

private:
    QProcess * m_process;
};

class NCSLocalCommandBridge : public NCSCommandBridge
{
    Q_OBJECT
public:
    explicit NCSLocalCommandBridge(QObject *parent = 0);
    void initialize(QString projectSubDir);
    void validate(QString path);
    NCSApplicationBridge * executeApplication(QString application, NCSCommandArguments arguments);
    NCSApplicationBridge * executeApplication(QString application, NCSCommandArguments arguments,int numProcesses, QString hostFile = "" );
    bool valid();


private:
    void m_invalidate(NCSCommandBridge::ValidationError err);
    bool m_transfer(QString sourcePath, QString destPath);

    QVector<NCSCommandFileArgument> m_downloadArguments;
    QString m_rootPath, m_buildPath, m_projectPath;
    NCSLocalApplicationBridge * m_applicationBridge;
    bool m_valid;
    QTimer m_timer;
};

#endif // NCSLOCALCOMMANDBRIDGE_H