#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("NCV");
    m_menuBar = new QMenuBar();
    m_fileMenu = new QMenu(tr("File"));
    m_fileMenu->addAction("Quit");
    m_menuBar->addMenu(m_fileMenu);

    m_editMenu = new QMenu(tr("Edit"));
    m_menuBar->addMenu(m_editMenu);

    m_toolsMenu = new QMenu(tr("Tools"));
    m_menuBar->addMenu(m_toolsMenu);
    setMenuBar(m_menuBar);



    m_tabWidget = new QwwConfigWidget();
    m_tabWidget->setIconSize(QSize(64,64));

    //m_tabWidget->setTabPosition(QTabWidget::West);

    m_buildWidget = new NCVBuildWidget();
    m_tabWidget->addGroup(m_buildWidget,QIcon(":/assets/setupIcon.png"),"Build");

    m_distributeWidget = new DistributeWidget();
    m_tabWidget->addGroup(m_distributeWidget,QIcon(":/assets/hardwareIcon.png"),"Distribute");
    connect(m_distributeWidget,SIGNAL(finalized()),this,SLOT(m_distributionFinalized()));

    m_visualizationWidget = new NCVWidget();
    m_tabWidget->addGroup(m_visualizationWidget,QIcon(":/assets/visualizationIcon.png"),"Analyze");

    connect(m_buildWidget,SIGNAL(neuronsFinalized(NCVNeuronSet*)),m_visualizationWidget,SLOT(setNeurons(NCVNeuronSet*)));
    connect(m_buildWidget,SIGNAL(connectionsFinalized(NCVConnectionSet*)),m_visualizationWidget,SLOT(setConnections(NCVConnectionSet*)));
    this->setCentralWidget(m_tabWidget);


    m_buildWidget->createNetwork(50000,50000,QVector3D(50000,50000,50000)); // temporary (see buildwidget)


}

void MainWindow::m_distributionFinalized()
{
    m_tabWidget->setCurrentGroup(m_visualizationWidget);
    m_distributeWidget->setEnabled(false);
}

void MainWindow::m_updateTimeScale(int value)
{
    float multiplier = (float)value / (float)m_timeScaleSlider->maximum();
    QString str = "Time Scale:";
    str.append(QString(" %1 X").arg(multiplier));
    m_timeScale->setText(str);

}

MainWindow::~MainWindow()
{

    delete ui;
}
