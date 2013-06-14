#ifndef NCSLOCALAPPLICATIONBRIDGE_H
#define NCSLOCALAPPLICATIONBRIDGE_H
#include "core/ncsapplicationbridge.h"
#include <QProcess>

class NCSLocalApplicationBridge:public NCSApplicationBridge
{
    Q_OBJECT
public:
    explicit NCSLocalApplicationBridge(QString name,QString workingDirectory = "",QObject *parent = 0);
    virtual ~NCSLocalApplicationBridge();
    void start(QString application,NCSCommandArguments arguments);
    QString readAllStandardError();
    QString readAllStandardOutput();

    void stopExecution(bool destroy);
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

#endif // NCSLOCALAPPLICATIONBRIDGE_H
