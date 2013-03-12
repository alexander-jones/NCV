#ifndef DEPLOYMENTWIDGET_H
#define DEPLOYMENTWIDGET_H

#include <QWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include "continuouscolorselector.h"
#include <QTextDocument>


enum DeploymentType
{
	LocalComputer,
	Cluster
};


class DeploymentWidget : public QWidget
{
	Q_OBJECT

public:

	DeploymentWidget(QWidget *parent = 0);
	~DeploymentWidget();

	DeploymentType typeSelected();

protected:
	void paintEvent(QPaintEvent * e);
signals:
	void localComputerPicked();
	void clusterPicked();

	private slots:
void m_onLocalComputerChecked(bool checked);
void m_onClusterChecked(bool checked);

private:
	DeploymentType m_typeSelected;
	QGroupBox *m_clusterDescriptionWidget, * m_localComputerDescriptionWidget;
	QVBoxLayout * m_deploymentLayout;
	QHBoxLayout * m_clusterDescriptionLayout, *m_localComputerDescriptionLayout;
	ImageContainer * m_clusterDescriptionImage,*m_localComputerDescriptionImage;
	QTextDocument * m_localComputerDescriptionText, *m_clusterDescriptionText;
	QWidget * m_clusterDescriptionLabel,*m_localComputerDescriptionLabel;
	QPushButton * m_useLocalComputerButton,* m_useClusterButton ;
};

#endif // DEPLOYMENTWIDGET_H