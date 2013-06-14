#ifndef NCSINSTALLATIONDIALOG_H
#define NCSINSTALLATIONDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QLabel>
#include <QListWidget>
#include <QFormLayout>
#include <QFileDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QDomDocument>
#include "core/ncscommandbridge.h"
#include "core/command/ncsremotecommandbridge.h"
#include "core/command/ncslocalcommandbridge.h"
#include "core/command/qsshsocket.h"
#include "core/utilities/simplecrypt.h"
#include "gui/utilities/qgroupvector.h"
#include "gui/ncsdialog.h"

class NCSInstallationDialog : public NCSDialog
{
    Q_OBJECT
public:

    enum BridgeCreationError
    {
        /*! \brief The NCS installation's host system does not contain a visible MPI installation.*/
        MissingMPI,
        /*! \brief The NCS installation path supplied can not be found.*/
        MissingRootDirectory,
        /*! \brief The NCS installation path supplied does not contain a build directory.*/
        MissingBuildDirectory,
        /*! \brief The NCS installation path supplied does not contain an application directory.*/
        MissingApplicationDirectory,
        /*! \brief The NCS installation path supplied does not contain a plugin directory.*/
        MissingPluginDirectory,
        /*! \brief The host system could not be connected to.*/
        ConnectionError
    };

    explicit NCSInstallationDialog(QWidget *parent = 0);
    bool hasDefaultInstallation();
    bool attemptDefault();
    bool attemptLocal(QString path = ".");
    bool attemptRemote(QString host, QString user,QString password, QString path = ".");
    bool attemptingConnection();

public slots:
    void show();

signals:
    void attemptSuccesful(NCSCommandBridge * bridge);
    void attemptFailed(NCSInstallationDialog::BridgeCreationError err);
    
public slots:

private slots:
    void m_loadRemoteInstallation(QModelIndex index);

    void m_validateRemoteConnection();
    void m_onRemoteAuthentication();
    void m_onRemoteError(QSshSocket::SshError err);
    void m_onRemoteConnect();
    void m_remoteConnectionValidated();
    void m_remoteConnectionInvalidated(NCSInternalCommandBridge::ValidationError err);

    void m_browseForLocalPath();
    void m_loadLocalInstallation(QModelIndex index);

    void m_validateLocalConnection();
    void m_localConnectionValidated();
    void m_localConnectionInvalidated(NCSInternalCommandBridge::ValidationError err);

    void m_saveCurrentInstallation();
    void m_onTextEditChanged();
    void m_installationTypeSwitched(QLayout* layout);
    void m_enableDefaultCheckBox(bool enable);
    void m_defaultChecked(bool checked);

private:
    struct InstallationContext
    {
        InstallationContext(QString path = "")
        {
            this->host = "localhost";
            this->user = "";
            this->password = "";
            this->path = path;
            guiTriggered = false;
        }

        InstallationContext(QString host, QString user,QString password,QString path)
        {
            this->host = host;
            this->user = user;
            this->password = password;
            this->path = path;
            guiTriggered = false;
        }
        bool operator==(InstallationContext other)
        {
            return host == other.host && path == other.path && password == other.password && user == other.user;
        }

        bool guiTriggered;
        QString host, user, password, path;
    };

    void m_buildRemoteWidget();
    void m_buildLocalWidget();
    void m_warnOfActiveConnection();
    void m_reportError(NCSInternalCommandBridge::ValidationError err);
    void m_checkValidationEnabled();

    void m_loadSavedInstallations();
    void m_removeSavedContext(InstallationContext context);
    void m_save();
    void m_reportSavedContextFailed();

    bool m_local;
    QSshSocket * m_socket;
    InstallationContext m_currentContext,m_defaultContext;
    QVector<InstallationContext> m_localContexts,m_remoteContexts;

    NCSRemoteCommandBridge * m_remoteCommandBridge;
    NCSLocalCommandBridge * m_localCommandBridge;

    QGroupVector * m_groupBoxVector;
    QHBoxLayout * m_localLayout;
    QWidgetVector  * m_localEntryVector, * m_savedLocalsVector, *m_localPathVector;
    QLineEdit * m_localPathEdit;
    QPushButton * m_localPathBrowseButton;
    QListWidget * m_savedLocalsWidget;

    QHBoxLayout * m_remoteLayout;
    QWidgetVector *m_savedRemotesVector, * m_remoteEntryVector;
    QWidget * m_remoteEntryWidget;
    QFormLayout * m_remoteEntryLayout;
    QLineEdit * m_remoteHostEdit,* m_remoteUserEdit,* m_remotePasswordEdit, * m_remotePathEdit;
    QPushButton * m_remotePathBrowseButton;
    QListWidget * m_savedRemotesWidget;

    QWidgetVector * m_mainVector, * m_bottomVector;
    QCheckBox * m_defaultCheckBox, * m_saveCheckBox;
    QPushButton * m_validateButton;

    QDomDocument m_savedInstallationsDocument;
    QDomElement m_rootDocumentElement;
    QString m_installationFilename;
};

#endif // NCSINSTALLATIONDIALOG_H
