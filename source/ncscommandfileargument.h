#ifndef NCSCOMMANDFILEARGUMENT_H
#define NCSCOMMANDFILEARGUMENT_H
#include <QString>
#include <QVector>

class NCSCommandFileArgument
{
public:
    enum SyncOperation
    {
        NoSync = 0,
        UploadBeforeExecution = 1,
        DownloadAfterExecution = 2
    };

    NCSCommandFileArgument();
    NCSCommandFileArgument(QString argument);
    NCSCommandFileArgument(QString argument,QString localFile,SyncOperation op);
    NCSCommandFileArgument(QString argument,QString localFile,SyncOperation op, QVector<NCSCommandFileArgument> dependencies);
    void syncWith(QString localFile, SyncOperation op);
    void setDependencies(QVector<NCSCommandFileArgument> dependencies);
    void setArgument(QString argument);
    void addDependency(NCSCommandFileArgument dependency);
    SyncOperation syncOperation();
    QString argument();
    QString localSyncFile();
    QVector<NCSCommandFileArgument> fileDependencies();

private:
    QString m_argument, m_localFile;
    SyncOperation m_operation;
    QVector<NCSCommandFileArgument> m_dependencies;
};


#endif // NCSCOMMANDFILEARGUMENT_H
