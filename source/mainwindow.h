#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ncvwidget.h"
#include <QMainWindow>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    NCVWidget * m_visualization;
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
