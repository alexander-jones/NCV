#ifndef NCSCOMMANDBRIDGE_H
#define NCSCOMMANDBRIDGE_H
#include "ncscommandfileargument.h"
#include <QString>
#include <QProcess>
#include <QDir>
#include <QTimer>
#include <QHostAddress>

class NCSCommandArguments
{
public:
    NCSCommandArguments(){}

    void append(NCSCommandFileArgument arg)
    {
        m_arguments.append(arg.argument());
        m_addToFileArguments(arg);
    }

    void append(const QString & literal)
    {
        m_arguments.append(literal);
    }
    NCSCommandArguments& operator<<(const NCSCommandFileArgument & arg)
    {
        append(arg);
        return *this;
    }

    NCSCommandArguments& operator<<(const QString & literal)
    {
        append(literal);
        return *this;
    }


    QStringList literals(){return m_arguments;}
    QVector<NCSCommandFileArgument> fileArguments(){return m_fileArguments;}
private:
    void m_addToFileArguments(NCSCommandFileArgument arg)
    {
        m_fileArguments.append(arg);
        QVector<NCSCommandFileArgument> dependencies= arg.fileDependencies();
        for (int i = 0; i < dependencies.size(); i ++)
            m_addToFileArguments(dependencies[i]);
    }

    QStringList m_arguments;
    QVector<NCSCommandFileArgument> m_fileArguments;

};

class NCSApplicationBridge:public QObject
{
    Q_OBJECT
public:
    enum ApplicationError
    {
        FailedToStart,
        Timedout,
        Crashed,
        ReadError,
        WriteError,
        ContextInvalid,
        OtherApplicationRunning,
        SyncUploadError,
        SyncDownloadError,
        UnknownError
    };

    explicit NCSApplicationBridge(QObject * parent):QObject(parent){}
    virtual QString readAllStandardError() = 0;
    virtual QString readAllStandardOutput() = 0;
    virtual void scheduleDestruction(bool destroy) = 0;
    virtual QString applicationName() = 0;
signals:
    void executionError(NCSApplicationBridge::ApplicationError err);
    void executionFinished();
    void readyReadStandardOutput();
    void readyReadStandardError();
};

class NCSCommandBridge: public QObject
{
    Q_OBJECT
public:
    enum ValidationError
    {
        MissingMPI,
        MissingRootDirectory,
        MissingBuildDirectory,
        MissingApplicationDirectory,
        MissingPluginDirectory
    };

    enum PluginType
    {
        Inputs,
        Neurons,
        Reports,
        Synapses
    };

    explicit NCSCommandBridge(QObject * parent):QObject(parent){}
    virtual void validate(QString path) = 0;
    virtual void executeApplication(QString application, NCSCommandArguments arguments) = 0;
    virtual void executeApplication(QString application, NCSCommandArguments arguments,int numProcesses, QString hostFile = "" ) = 0;
    virtual bool valid() = 0;
    virtual QString hostname() = 0;


signals:
    void applicationStarted(NCSApplicationBridge * app);
    void validated();
    void validationError(NCSCommandBridge::ValidationError err);
};
#endif // NCSCOMMANDBRIDGE_H
