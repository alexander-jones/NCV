#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_visualization = new NCVWidget();
    setCentralWidget(m_visualization);
}


MainWindow::~MainWindow()
{

    delete ui;
    delete m_visualization;
}
