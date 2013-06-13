#ifndef NCSLOCALCOMMANDBRIDGE_H
#define NCSLOCALCOMMANDBRIDGE_H
#include "ncslocalapplicationbridge.h"
#include "ncsinternalcommandbridge.h"
#include <QTimer>

class NCSLocalCommandBridge : public NCSInternalCommandBridge
{
    Q_OBJECT
public:
    explicit NCSLocalCommandBridge(QObject *parent = 0);
    QString hostname();
    void launchApplicationBridge(QString application, NCSCommandArguments arguments);
    void launchApplicationBridge(QString application, NCSCommandArguments arguments,int numProcesses, QString hostFile = "" );
    void queryApplication(QString applicationName);
    void queryPlugin(NCSCommandBridge::PluginType type,QString pluginName);
    void queryReader(QString readerName);
    void validate(QString path);
    bool valid();

private:
    bool m_fileExists(QDir rootDir, QString pattern);
    void m_invalidate(NCSInternalCommandBridge::ValidationError err);

    QVector<NCSCommandFileArgument> m_downloadArguments;
    QString m_rootPath, m_buildPath;
    NCSLocalApplicationBridge * m_applicationBridge;
    bool m_valid;
    QTimer m_timer;
};

#endif // NCSLOCALCOMMANDBRIDGE_H
