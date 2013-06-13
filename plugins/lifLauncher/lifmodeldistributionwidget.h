#ifndef LIFMODELDISTRIBUTIONWIDGET_H
#define LIFMODELDISTRIBUTIONWIDGET_H
#include "core/ncscluster.h"
#include "core/plugin-interfaces/ncsdistributionwidgetplugin.h"
#include "gui/utilities/qwidgetvector.h"
#include "gui/utilities/qgroupvector.h"
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QSpinBox>

class LIFModelDistributionWidget : public NCSDistributionWidgetPlugin
{
    Q_OBJECT
public:
    explicit LIFModelDistributionWidget(QWidget *parent = 0);
    QIcon icon();
    QString title();
    void loadProject(QString projectDir);
    QString name();
    float version();

public slots:
    void setCommandBridge(NCSCommandBridge * bridge);
    void initialize();
    void cleanup();

private slots:
    void m_toy();
    void m_readSimulatorOutput();
    void m_readSimulatorError();

    void m_readDistributorOutput();
    void m_readDistributorError();

    void m_distributionStarted(NCSApplicationBridge * app);
    void m_destroyDistribution();

    void m_simulationStarted(NCSApplicationBridge * app);
    void m_destroySimulation();

    void m_distributionFailed(NCSApplicationBridge::ApplicationError);
    void m_distributionFinished();

    void m_simulationFailed(NCSApplicationBridge::ApplicationError);
    void m_simulationFinished();

    void m_modelLoadPressed();
    void m_modelAppendPressed();
    void m_modelRemovePressed();

    void m_browseClusterPressed();
    void m_launchSimulationPressed();
    void m_timeUnitSelected(QString unit);

private:
    bool m_fieldsValid();

    QString m_reportHost, m_projectDir;
    bool m_launching, m_modelValid, m_clusterValid;
    QVBoxLayout * m_layout;
    QString m_distributionOutputDir;
    QListWidget * m_modelDependencyWidget;
    QLineEdit * m_clusterFileEdit, * m_modelFileEdit;
    QLabel * m_clusterFileLabel, *m_modelDependencyLabel, * m_timeLabel;
    QPushButton * m_browseClusterButton, * m_loadModelFileButton, * m_appendModelFileButton, * m_removeModelFileButton;
    QHBoxLayout * m_modelLayout;
    QVBoxLayout * m_modelIncludeLayout;
    QWidgetVector * m_clusterWidgetVector,* m_modelButtonVector, * m_timeWidgetVector;
    QGroupVector * m_mainGroupVector;
    QComboBox * m_timeUnitsComboBox;
    QSpinBox * m_timeSpinBox;
    QString m_topologyFilename;
    QPushButton * m_launchSimulationButton;
    NCSCommandBridge * m_commandBridge;
    NCSApplicationBridge * m_currentApplication;
    bool m_launched;
    
};

#endif // LIFMODELDISTRIBUTIONWIDGET_H
