#ifndef DISTRIBUTEWIDGET_H
#define DISTRIBUTEWIDGET_H

#include "connectionwidget.h"
#include <QWidget>
#include <QLabel>
#include "ncsclustereditor.h"
#include "slidingstackedwidget.h"
#include "continuouscolorselector.h"
#include "colorbutton.h"
#include "modelselector.h"
#include "deploymentwidget.h"
#include <iostream>
#include <stdio.h>
#include <QProgressBar>

class DistributeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DistributeWidget(QWidget *parent = 0);
    
signals:
    void finalized();
    
public slots:

private slots:
	void m_localComputerSelected();
	void m_clusterSelected();
    void m_mainWidgetChanged(int index);
    void m_distribute();
    void m_validateNCSCapability(SSHSocket * session);
    void m_detectDevices();
    void m_error(SSHSocket::SSHSocketError err);
    void m_modelLoaded(QString model);
    void m_onCommandExecuted(QString command,QString result);
    void m_onFilePushed(QString localFile,QString remoteFile);
    void m_onFilePulled(QString localFile,QString remoteFile);
    void m_connectionAttempted();
    void m_deploymentPicked();
    void m_onConnectionFailed();

private:
	enum DeploymentType
	{
		LocalComputer,
		Cluster
    };
    QString m_executeLocalCommand(QString cmd);
    void m_showErrorPopup(QString text, QString detailedText= "");


	DeploymentType m_deploymentType;
	DeploymentWidget * m_deploymentWidget;
    bool m_mpiExists, m_brainslugExists;
    bool m_validConnection;
    QLabel  *m_machinesLabel;
    SSHSocket * m_clusterConnection;
    NCSClusterEditor * m_clusterEditor;
    SlidingStackedWidget * m_widgetStack;
    QVBoxLayout * m_layout,*m_clusterLayout ;
    QHBoxLayout* m_buttonLayout, *m_promptLayout;
    QWidget * m_buttonWidget;
    ConnectionWidget* m_connectionWidget;
    ColorButton * m_previousButton, * m_nextButton;
    QWidget *m_clusterWidget,*m_promptWidget;
    ModelSelector*m_modelWidget;
    QSpinBox *m_numComputersEntryBox;
    QString m_model;
    QPushButton * m_detectDevicesButton;
    QProgressBar * m_stepProgress;
};

#endif // DISTRIBUTEWIDGET_H
