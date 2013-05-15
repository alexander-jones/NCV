#ifndef RemoteConnectionWidget_H
#define RemoteConnectionWidget_H

#include "core/networking/command/qsshsocket.h"
#include "core/utilities/simplecrypt.h"
#include "core/gui/utilities/qwidgetvector.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QLabel>
#include <QListWidget>
#include <QFileDialog>
#include <QLineEdit>


/*!
    \class RemoteConnectionWidget
    \author Alex Jones
    \brief A widget that manages user interaction for connecting to a remote machine via ssh.
*/
class RemoteConnectionWidget : public QWidget
{
    Q_OBJECT
public:
	
	/*!
        \class RemoteConnectionWidget
		\author Alex Jones
		\brief A widget that manages user interaction 
	*/
    explicit RemoteConnectionWidget(QWidget *parent = 0);

signals:
    void connectionAttempted();

    void connectionFailed();
	/*!
		\param connection The ssh connection established through this widget.
		\brief This signal is emitted when a connection is established. 
	*/
    void connected(QSshSocket *connection);

public slots:

	/*!
		\param filename The filename of an encrypted credential file saved from  this widget.
		\brief This signal is emitted when a connection is established. 
	*/
	bool loadCredentials(QString filename);

	/*!
		\param filename The filename to save current credentials to in an encrypted format. 
		\brief This signal is emitted when a connection is established. 
	*/
	void saveCredentials(QString filename);
    
private slots:
    void m_onAuthentication();
    void m_onError(QSshSocket::SshError err);
    void m_onConnect();
	void m_saveCredentials();
    void m_tryConnect();
    void m_loadCredentials();
    void m_loadCredentials(QListWidgetItem* item);

private:
    void m_addToRecentCredentials(QString fileName);

    QSshSocket * m_connection;
    QLabel * m_credentialEntryLabel,*m_recentCredentialsLabel;
    QWidget *m_buttonWidget;
    QFrame *m_credentialEntryWidget, *m_recentCredentialWidget ;
    QListWidget * m_recentCredentials;
    QVBoxLayout *m_credentialEntryLayout,*m_recentCredentialLayout;
    QHBoxLayout * m_layout;
    QGridLayout * m_buttonLayout;
    QPushButton *m_connectButton, * m_loadCredentialsButton, * m_saveCredentialsButton;
    QWidgetVector *m_hostVector,* m_userVector,* m_passwordVector;
    QLineEdit *m_hostEdit,* m_userEdit,* m_passwordEdit;
};

#endif // RemoteConnectionWidget_H
