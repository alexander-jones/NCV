#ifndef NCSCONNECTIONWIDGET_H
#define NCSCONNECTIONWIDGET_H

#include "remoteconnectionwidget.h"
#include "core/networking/command/ncslocalcommandbridge.h"
#include "core/networking/command/ncsremotecommandbridge.h"
#include "core/networking/reporting/ncsdatasource.h"
#include "core/gui/plugin-interfaces/ncsconnectionwidgetplugin.h"
#include "core/gui/utilities/qwidgetvector.h"
#include "core/gui/utilities/qgroupvector.h"
#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QLineEdit>
#include <QGroupBox>

class NCSConnectionWidget : public NCSConnectionWidgetPlugin
{
    Q_OBJECT
public:
    explicit NCSConnectionWidget(QString projectPath,QWidget *parent = 0);
    QIcon icon();
    QString title();

public slots:
    void initialize();
    void cleanup();

private slots:
    void m_connectionInvalidated(NCSCommandBridge::ValidationError err);
    void m_localConnectionValidated();
    void m_localNCSDirectoryChanged(QString newText);
    void m_browseForNCS();

    void m_validateRemoteConnection();
    void m_validateLocalConnection();

    void m_remoteConnectionEstablished(QSshSocket * connection);
    void m_remoteConnectionError(QSshSocket::SshError err);
    void m_remoteConnectionFailed();
    void m_remoteConnectionValidated();
    void m_connectionGroupChecked(QString groupName,bool checked);

private:
    QLabel * m_statusLabel;
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

#endif // NCSCONNECTIONWIDGET_H
