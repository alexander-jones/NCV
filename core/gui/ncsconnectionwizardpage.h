#ifndef NCSCONNECTIONWIZARDPAGE_H
#define NCSCONNECTIONWIZARDPAGE_H
#include <QWizardPage>
#include "core/gui/remoteconnectionwidget.h"
#include "core/gui/utilities/qgroupvector.h"
#include "core/networking/command/ncscommandbridge.h"
#include "core/networking/command/ncsremotecommandbridge.h"
#include "core/networking/command/ncslocalcommandbridge.h"

class NCSConnectionWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    explicit NCSConnectionWizardPage(QWidget *parent = 0);
    void loadProject(QString projectDir);
    bool validatePage();

public slots:
    void initialize();
    void cleanup();

signals:
    void bridgeEstablished(NCSCommandBridge * bridge);

private slots:
    void m_connectionInvalidated(NCSCommandBridge::ValidationError err);
    void m_localConnectionValidated();
    void m_localNCSDirectoryChanged(QString newText);
    void m_browseForNCS();

    void m_validateRemoteConnection();
    void m_validateLocalConnection();

    void m_clearRemoteContext();
    void m_remoteConnectionEstablished(QSshSocket * connection);
    void m_remoteConnectionError(QSshSocket::SshError err);
    void m_remoteConnectionFailed();
    void m_remoteConnectionValidated();
    void m_connectionGroupChecked(QLayout * layout);

private:

    bool m_complete;
    QString m_projectPath;
    QGroupVector * m_connectionGroupVector;
    QWidgetVector *m_remoteNCSDirectoryVector;
    QHBoxLayout * m_localLayout;
    QVBoxLayout * m_remoteLayout;
    QLineEdit * m_ncsDirectoryEdit, * m_remoteNCSDirectoryEdit;
    QPushButton * m_ncsBrowseButton,*m_hostfileBrowseButton;
    QLabel * m_localNCSDirectoryLabel, *m_remoteNCSDirectoryLabel;
    QSshSocket * m_socket;
    QVBoxLayout * m_layout;
    QPushButton * m_remoteValidateButton, * m_localValidateButton;
    RemoteConnectionWidget * m_remoteConnectionWidget;
    NCSLocalCommandBridge * m_localCommandBridge;
    NCSRemoteCommandBridge * m_remoteCommandBridge;
    
};

#endif // NCSCONNECTIONWIZARDPAGE_H
