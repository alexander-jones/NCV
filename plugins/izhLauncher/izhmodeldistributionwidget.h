#ifndef IZHMODELDISTRIBUTIONWIDGET_H
#define IZHMODELDISTRIBUTIONWIDGET_H
#include "core/gui/plugin-interfaces/ncsdistributionwidgetplugin.h"
#include "core/gui/utilities/qwidgetvector.h"
#include "core/gui/utilities/qgroupvector.h"
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QSpinBox>

class IzhModelDistributionWidget : public NCSDistributionWidgetPlugin
{
    Q_OBJECT
public:
    explicit IzhModelDistributionWidget(QWidget *parent = 0);
    QIcon icon();
    QString title();
    void loadProject(QString projectDir);

public slots:
    void setCommandBridge(NCSCommandBridge * bridge);
    void initialize();
    void cleanup();

private slots:
    void m_readStandardOutput();
    void m_readStandardError();


    void m_distributionStarted(NCSApplicationBridge * app);
    void m_destroyDistribution();

    void m_simulationStarted(NCSApplicationBridge * app);
    void m_destroySimulation();

    void m_distributionFailed(NCSApplicationBridge::ApplicationError);
    void m_distributionFinished();

    void m_simulationFailed(NCSApplicationBridge::ApplicationError);
    void m_simulationFinished();

    void m_browseClusterFilePressed();
    void m_browseNeuronFilePressed();
    void m_browseSynapseFilePressed();
    void m_browseCurrentFilePressed();
    void m_launchSimulationPressed();
    void m_timeUnitSelected(QString unit);
private:
    QString m_getFilename(QString path);

    QString m_reportHost;
    QVBoxLayout * m_layout;
    QWidgetVector * m_neuronFileVector,* m_synapseFileVector,*m_currentFileVector,*m_clusterFileVector;
    QLineEdit * m_neuronFileEdit,* m_synapseFileEdit,*m_currentFileEdit,*m_clusterFileEdit;
    QLabel *m_neuronFileLabel,* m_synapseFileLabel,*m_currentFileLabel,*m_clusterFileLabel;
    QPushButton * m_brosweNeuronFileButton, *m_brosweSynapseFileButton, * m_brosweCurrentFileButton, * m_brosweClusterFileButton;
    QString m_projectDir,m_distributionOutputDir;
    bool m_launching, m_modelValid, m_clusterValid;
    QLabel * m_neuronCountLabel, *m_timeLabel;
    QWidgetVector * m_neuronCountVector, * m_timeWidgetVector;
    QString m_topologyFilename;
    QComboBox * m_timeUnitsComboBox;
    QSpinBox * m_neuronCountSpinBox,*m_timeSpinBox;
    QPushButton * m_launchSimulationButton;
    NCSCommandBridge * m_commandBridge;
    NCSApplicationBridge * m_currentApplication;

};

#endif // IZHMODELDISTRIBUTIONWIDGET_H
