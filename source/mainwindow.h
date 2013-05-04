#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qwidgetvector.h"
#include "ncsconnectionwidget.h"
#include "ncsclustereditor.h"
#include "lifmodeldistributionwidge.h"
#include "izhmodeldistributionwidget.h"
#include "ncsdatasource.h"
#include "ncvwidget.h"
#include <QMainWindow>
#include <QVBoxLayout>
#include <QwwConfigWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    void addWidget(NCSWidgetPlugin *widget);
    void addWidget(NCSApplicationWidgetPlugin *widget);
    void addWidget(NCSDistributionWidgetPlugin *widget);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *);

private slots:
    void m_setCommandBridge(NCSCommandBridge * bridge);
    void m_createNetwork(QString topologyFilename);
    void m_publishNetwork();
    void m_loadProject(QString projectDirectory);
    void m_updateTimeScale(int multiplier);
    void m_distributionFinalized();
    void m_openProjectPressed();
    void m_newProjectPressed();
    void m_ncsApplicationLaunched(NCSApplicationBridge * );
    void m_ncsApplicationFinished(QObject * );

    void m_runSimulation();
    void m_pauseSimulation();
    void m_stopSimulation();

private:
    int m_simulationApplicationIndex;
    QToolBar * m_simulationToolbar;
    QSlider * m_simulationTimeSlider;
    QAction *m_runSimulationButton, * m_pauseSimulationButton, * m_stopSimulationButton;
    QSignalMapper * m_applicationMapper;
    QVector<NCSApplicationBridge *> m_activeApplications;
    QTimer * m_updateTimer;
    NCSCommandBridge * m_commandBridge;
    NCSDataSource * m_dataSource;
    NCVWidget * m_visualizationWidget;
    QString m_rootPath;
    QVBoxLayout * m_layout;
    QwwConfigWidget * m_tabWidget;
    NCSConnectionWidget * m_connectionWidget;
    QToolBar * m_toolbar;
    QSlider * m_timeScaleSlider;
    QLabel * m_timeScale;
    QMenu * m_fileMenu, * m_editMenu, *m_toolsMenu;
	QMenuBar * m_menuBar;
    NCVNeuronSet * m_neurons;
    NCVConnectionSet * m_connections;
    QMap<QString,NCVDiscreteAttribute *> m_discreteNeuronAttributes,m_discreteConnectionAttributes;
    QMap<QString,NCVContinuousAttribute *> m_continuousNeuronAttributes,m_continuousConnectionAttributes;

};

#endif // MAINWINDOW_H
