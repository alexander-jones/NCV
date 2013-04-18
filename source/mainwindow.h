#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qwidgetvector.h"
#include "ncsconnectionwidget.h"
#include "ncsclustereditor.h"
#include "ncsmodeldistributionwidget.h"
#include "ncvwidget.h"
#include <QMainWindow>
#include <QVBoxLayout>
#include <QwwConfigWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    void addWidget(NCSWidgetPlugin *widget);
    void addWidget(NCSApplicationWidgetPlugin *widget);
    void addWidget(NCSDistributionWidgetPlugin *widget);
    ~MainWindow();

private slots:
    void m_createNetwork(QString topologyFilename);
    void m_publishNetwork();
    void m_loadProject(QString projectDirectory);
    void m_updateTimeScale(int multiplier);
    void m_distributionFinalized();
    void m_openProjectPressed();
    void m_newProjectPressed();

private:

    NCVWidget * m_visualizationWidget;
    QString m_rootPath;
    Ui::MainWindow *ui;
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
