#ifndef LIFMODELDISTRIBUTIONWIDGET_H
#define LIFMODELDISTRIBUTIONWIDGET_H
#include "ncsdistributionwidgetplugin.h"
#include "qwidgetvector.h"
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QSpinBox>
#include "qgroupvector.h"

class LIFModelDistributionWidget : public NCSDistributionWidgetPlugin
{
    Q_OBJECT
public:
    explicit LIFModelDistributionWidget(QString projectDir,QWidget *parent = 0);
    QIcon icon();
    QString title();

public slots:
    void initialize(NCSCommandBridge * bridge);

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

    void m_modelLoadPressed();
    void m_modelAppendPressed();
    void m_modelRemovePressed();

    void m_clusterEditChanged(QString newText);
    void m_clusterEditValid(QString newText);
    void m_clusterEditInvalid(QString newText);

    void m_browseClusterPressed();
    void m_launchSimulationPressed();
    void m_timeUnitSelected(QString unit);
private:
    QString m_projectDir;
    bool m_launching, m_modelValid, m_clusterValid;
    QVBoxLayout * m_layout;
    QString m_distributionOutputDir;
    QListWidget * m_modelDependencyWidget;
    QLineEdit * m_clusterFileEdit, * m_modelFileEdit;
    QLabel * m_clusterFileLabel, *m_modelDependencyLabel, * m_timeLabel;
    QPushButton * m_browseClusterButton, * m_loadModelFileButton, * m_appendModelFileButton, * m_removeModelFileButton;
    QWidgetVector * m_clusterWidgetVector, * m_modelIncludeVector, *m_modelWidgetVector, * m_timeWidgetVector,* m_modelButtonVector;
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
