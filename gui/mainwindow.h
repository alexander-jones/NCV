#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gui/utilities/qwidgetvector.h"
#include "gui/ncsinstallationdialog.h"
#include "plugins/clusterEditor/ncsclustereditor.h"
#include "plugins/lifLauncher/lifmodeldistributionwidget.h"
#include "plugins/izhLauncher/izhmodeldistributionwidget.h"
#include "plugins/visualizer/ncvwidget.h"
#include "plugins/pythonEditor/pythoneditor.h"
#include "core/reporting/networkupdatemanager.h"
#include "core/ncsproject.h"
#include <QMainWindow>
#include <QVBoxLayout>
#include <QxtConfigWidget>
#include <QLabel>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    void addPlugin(NCSWidgetPlugin *widget);
    void addPlugin(NCSApplicationWidgetPlugin *widget);
    void addPlugin(NCSDistributionWidgetPlugin *widget);
    void addPlugin(NCSSubscriberWidgetPlugin *widget);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *);

private slots:
    void m_defaultNCSInstallationFailed(NCSInstallationDialog::BridgeCreationError);
    void m_setCommandBridge(NCSCommandBridge * bridge);
    void m_createNetwork(QString topologyFilename);
    void m_publishNetwork(QString reportHost);
    void m_loadProject(QString projectDirectory);
    void m_closeProject();
    void m_updateTimeScale(int updateInterval);
    void m_openProjectPressed();
    void m_newProjectPressed();
    void m_ncsApplicationLaunched(NCSApplicationBridge * );
    void m_ncsApplicationFinished(QObject * );

    void m_runSimulationPressed();
    void m_pauseSimulationPressed();
    void m_stopSimulationPressed();
    void m_disconnectFromSimulator(bool destroy = true);
    void m_setSimulationToolbar(bool on);
    void m_showLoadingSimulation();
    void m_hideLoadingSimulation();

private:
    void m_setPluginEnabled(NCSWidgetPlugin * plugin, bool enabled);
    bool m_removeDir(const QString & dirName);

    QVector< NCSWidgetPlugin *> m_allPlugins;
    QVector< NCSApplicationWidgetPlugin *> m_applicationPlugins;
    QVector< NCSDistributionWidgetPlugin *> m_distributionPlugins;
    QVector< NCSSubscriberWidgetPlugin *> m_subscriberPlugins;

    NCSInstallationDialog * m_installationDialog;
    NCSProject * m_project;
    int m_simulationApplicationIndex;
    QToolBar * m_simulationToolbar;
    QSlider * m_simulationTimeSlider;
    QAction *m_runSimulationButton, * m_pauseSimulationButton, * m_stopSimulationButton;
    QSignalMapper * m_applicationMapper;
    QVector<NCSApplicationBridge *> m_activeApplications;
    NCSCommandBridge * m_commandBridge;
    NetworkUpdateManager * m_reportingManager;
    QString m_rootPath;
    QVBoxLayout * m_layout;
    QxtConfigWidget * m_applicationLauncher;
    QSlider * m_timeScaleSlider;
    QLabel * m_timeScaleLabel ,* m_simulationLoadingLabel, *m_ncsContextLabel;
    QMovie * m_simulationLoadingMovie;
    QMenu * m_fileMenu, * m_editMenu, *m_toolsMenu;
	QMenuBar * m_menuBar;
    NCSNeuronSet * m_neurons;
    NCSConnectionSet * m_connections;
    QMap<QString,NCSDiscreteAttribute *> m_discreteNeuronAttributes,m_discreteConnectionAttributes;
    QMap<QString,NCSContinuousAttribute *> m_continuousNeuronAttributes,m_continuousConnectionAttributes;
};

#endif // MAINWINDOW_H
