#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "core/gui/utilities/qwidgetvector.h"
#include "plugins/connection/ncsconnectionwidget.h"
#include "plugins/clusterEditor/ncsclustereditor.h"
#include "plugins/lifLauncher/lifmodeldistributionwidget.h"
#include "plugins/izhLauncher/izhmodeldistributionwidget.h"
#include "plugins/visualizer/ncvwidget.h"
#include "core/networking/reporting/ncsdatasource.h"
#include <QMainWindow>
#include <QVBoxLayout>
#include <qxt/QxtGui/QxtConfigWidget>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    void addWidget(NCSWidgetPlugin *widget);
    void addWidget(NCSConnectionWidgetPlugin *widget);
    void addWidget(NCSApplicationWidgetPlugin *widget);
    void addWidget(NCSDistributionWidgetPlugin *widget);
    void addWidget(NCSSubscriberWidgetPlugin *widget);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *);

private slots:
    void m_setCommandBridge(NCSCommandBridge * bridge);
    void m_createNetwork(QString topologyFilename);
    void m_publishNetwork();
    void m_loadProject(QString projectDirectory);
    void m_updateTimeScale(int multiplier);
    void m_openProjectPressed();
    void m_newProjectPressed();
    void m_ncsApplicationLaunched(NCSApplicationBridge * );
    void m_ncsApplicationFinished(QObject * );

    void m_runSimulation();
    void m_pauseSimulation();
    void m_stopSimulation();
    void m_setSimulationToolbar(bool on);
    void m_showLoadingSimulation();

private:
    QVector< NCSWidgetPlugin *> m_allPlugins;
    QVector< NCSConnectionWidgetPlugin *> m_connectionPlugins;
    QVector< NCSApplicationWidgetPlugin *> m_applicationPlugins;
    QVector< NCSDistributionWidgetPlugin *> m_distributionPlugins;
    QVector< NCSSubscriberWidgetPlugin *> m_subscriberPlugins;

    int m_simulationApplicationIndex;
    QToolBar * m_simulationToolbar;
    QSlider * m_simulationTimeSlider;
    QAction *m_runSimulationButton, * m_pauseSimulationButton, * m_stopSimulationButton;
    QSignalMapper * m_applicationMapper;
    QVector<NCSApplicationBridge *> m_activeApplications;
    QTimer * m_updateTimer;
    NCSCommandBridge * m_commandBridge;
    NCSDataSource * m_dataSource;
    QString m_rootPath;
    QVBoxLayout * m_layout;
    QxtConfigWidget * m_applicationLauncher;
    QToolBar * m_toolbar;
    QSlider * m_timeScaleSlider;
    QLabel * m_timeScale, * m_simulationLoadingLabel;
    QMovie * m_simulationLoadingMovie;
    QMenu * m_fileMenu, * m_editMenu, *m_toolsMenu;
	QMenuBar * m_menuBar;
    NCSNeuronSet * m_neurons;
    NCSConnectionSet * m_connections;
    QMap<QString,NCSDiscreteAttribute *> m_discreteNeuronAttributes,m_discreteConnectionAttributes;
    QMap<QString,NCSContinuousAttribute *> m_continuousNeuronAttributes,m_continuousConnectionAttributes;

};

#endif // MAINWINDOW_H
