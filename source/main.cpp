
#include "mainwindow.h"
#include <QApplication>
#include <QPushButton>
#include <QDebug>
#include <QGridLayout>

int main( int argc, char* argv[] )
{
    QApplication a( argc, argv );
    MainWindow * w = new MainWindow();
    a.setWindowIcon(QIcon(":/assets/ncs.png"));
    a.setActiveWindow(w);

    w->show();

    a.exec();
    delete w;
    return 0;
}
