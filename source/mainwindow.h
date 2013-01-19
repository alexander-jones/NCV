#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include "ncvwidget.h"
#include "distributewidget.h"
#include <QwwConfigWidget>

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

private:
    Ui::MainWindow *ui;
    QVBoxLayout * m_layout;
    QwwConfigWidget * m_tabWidget;
    NCVWidget * m_visualizationWidget;
    QWidget * m_buildWidget;
    DistributeWidget  * m_distributeWidget;
    QToolBar * m_toolbar;
    QSlider * m_timeScaleSlider;
    QLabel * m_timeScale;
    QPushButton * m_playButton, *m_pauseButton, *m_stopButton, *m_recordButton;
    QMenu * m_fileMenu, * m_editMenu, *m_toolsMenu;
    QMenuBar * m_menuBar;


};

#endif // MAINWINDOW_H
