#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "distributewidget.h"
#include <QMainWindow>
#include <QVBoxLayout>
#include <QwwConfigWidget>
#include "ncvwidget.h"
#include "ncvbuildwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void m_updateTimeScale(int multiplier);
    void m_distributionFinalized();

private:
    Ui::MainWindow *ui;
    QVBoxLayout * m_layout;
    QwwConfigWidget * m_tabWidget;
    NCVBuildWidget * m_buildWidget;
    NCVWidget * m_visualizationWidget;
    //QWidget * m_buildWidget;
    DistributeWidget  * m_distributeWidget;
    QToolBar * m_toolbar;
    QSlider * m_timeScaleSlider;
    QLabel * m_timeScale;
    QMenu * m_fileMenu, * m_editMenu, *m_toolsMenu;
	QMenuBar * m_menuBar;


};

#endif // MAINWINDOW_H
