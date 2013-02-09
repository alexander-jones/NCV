#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_menuBar = new QMenuBar();
    m_fileMenu = new QMenu(tr("File"));
    m_fileMenu->addAction("Quit");
    m_menuBar->addMenu(m_fileMenu);

    m_editMenu = new QMenu(tr("Edit"));
    m_menuBar->addMenu(m_editMenu);

    m_toolsMenu = new QMenu(tr("Tools"));
    m_menuBar->addMenu(m_toolsMenu);
    setMenuBar(m_menuBar);


    m_toolbar = new QToolBar();

    m_playButton = new QPushButton(QIcon(":/assets/playIcon.png"),"");
    m_toolbar->addWidget(m_playButton);

    m_pauseButton = new QPushButton(QIcon(":/assets/pauseIcon.png"),"");
    m_toolbar->addWidget(m_pauseButton);

    m_stopButton = new QPushButton(QIcon(":/assets/stopIcon.png"),"");
    m_toolbar->addWidget(m_stopButton);

    m_recordButton = new QPushButton(QIcon(":/assets/recordIcon.png"),"");
    m_toolbar->addWidget(m_recordButton);

    m_timeScaleSlider = new QSlider(Qt::Horizontal);
    m_timeScaleSlider->setMinimum(0);
    m_timeScaleSlider->setMaximum(100);
    m_timeScaleSlider->setValue(100);
    m_timeScaleSlider->setMaximumWidth(100);
    m_toolbar->addWidget(m_timeScaleSlider);

    connect(m_timeScaleSlider,SIGNAL(valueChanged(int)),this,SLOT(m_updateTimeScale(int)));

    m_timeScale = new QLabel("Time Scale: 1.0 X");
    m_timeScale->setAlignment(Qt::AlignCenter);
    m_toolbar->addWidget(m_timeScale);

    addToolBar(m_toolbar);


    m_tabWidget = new QwwConfigWidget();
    m_tabWidget->setIconSize(QSize(64,64));

    //m_tabWidget->setTabPosition(QTabWidget::West);

    m_buildWidget = new QWidget();
    m_tabWidget->addGroup(m_buildWidget,QIcon(":/assets/setupIcon.png"),"Build");

    m_distributeWidget = new DistributeWidget();
    m_tabWidget->addGroup(m_distributeWidget,QIcon(":/assets/hardwareIcon.png"),"Distribute");

    m_visualizationWidget = new NCVWidget();
    m_tabWidget->addGroup(m_visualizationWidget,QIcon(":/assets/visualizationIcon.png"),"Analyze");
    this->setCentralWidget(m_tabWidget);
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
