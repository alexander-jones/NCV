#ifndef NCSCONNECTIONWIDGET_H
#define NCSCONNECTIONWIDGET_H

#include "remoteconnectionwidget.h"
#include "ncslocalcommandbridge.h"
#include "ncsremotecommandbridge.h"
#include "ncswidgetplugin.h"
#include "qwidgetvector.h"
#include "qgroupvector.h"
#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QLineEdit>
#include <QGroupBox>

class NCSConnectionWidget : public NCSWidgetPlugin
{
    Q_OBJECT
public:
    explicit NCSConnectionWidget(QString projectPath,QWidget *parent = 0);
    QIcon icon();
    QString title();
    
signals:
    void validNCSBridgeEstablished(NCSCommandBridge * bridge);
    
public slots:

private slots:
    void m_connectionInvalidated(NCSCommandBridge::ValidationError err);
    void m_localConnectionValidated();
    void m_localNCSDirectoryChanged(QString newText);
    void m_browseForNCS();

    void m_validateRemoteConnection();
    void m_validateLocalConnection();

    void m_remoteConnectionEstablished(SSHSocket * connection);
    void m_remoteConnectionError(SSHSocket::SSHSocketError err);
    void m_remoteConnectionFailed();
    void m_remoteConnectionValidated();

private:
    QLabel * m_statusLabel;
    QString m_projectPath;
    QWidgetVector * m_localNCSDirectoryVector,* m_remoteNCSDirectoryVector;
    QGroupVector * m_connectionGroupVector;
    QLineEdit * m_ncsDirectoryEdit, * m_remoteNCSDirectoryEdit;
    QPushButton * m_ncsBrowseButton,*m_hostfileBrowseButton;
    QLabel * m_localNCSDirectoryLabel, *m_remoteNCSDirectoryLabel;
    SSHSocket * m_socket;
    QVBoxLayout * m_layout;
    QPushButton * m_remoteValidateButton, * m_localValidateButton;
    RemoteConnectionWidget * m_remoteConnectionWidget;
    NCSLocalCommandBridge * m_localCommandBridge;
    NCSRemoteCommandBridge * m_remoteCommandBridge;
    
};

#endif // NCSCONNECTIONWIDGET_H
