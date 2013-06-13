#ifndef NCSPROJECT_H
#define NCSPROJECT_H
#include <QVector>
#include "ncsprojectportal.h"
#include "core/plugin-interfaces/ncswidgetplugin.h"
#include "core/command/ncslocalcommandbridge.h"
#include "core/command/ncsremotecommandbridge.h"

class NCSProject : public QObject
{
    Q_OBJECT
public:
    explicit NCSProject(const QString & filepath,QObject *parent = 0);
    QString parentDirectory();
    void registerPlugin(NCSWidgetPlugin * plugin);
    void save();
    void saveAs(QString filename);
    void setNCSInstallation(QSshSocket * socket,QString installationPath);
    void setNCSInstallation(QString installationPath);
    
signals:
    void ncsInstallationValidated(NCSCommandBridge * bridge);
    void ncsInstallationInvalidated();

private slots:
    void m_connectionInvalidated(NCSInternalCommandBridge::ValidationError err);
    void m_localConnectionValidated();
    void m_remoteConnectionValidated();

private:
    void m_load(QFile * file);
    void m_save(QFile * file);
    NCSCommandBridge * m_commandBridge;
    NCSRemoteCommandBridge * m_remoteCommandBridge;
    NCSLocalCommandBridge * m_localCommandBridge;
    QString m_filepath, m_parentDirectory;
    QDomDocument m_document;
};

#endif // NCSPROJECT_H
