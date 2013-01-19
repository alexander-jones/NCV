
#include "mainwindow.h"
#include <QApplication>
#include <QPushButton>
#include <QDebug>
#include <QGridLayout>

int main( int argc, char* argv[] )
{
    QApplication a( argc, argv );

    MainWindow * w = new MainWindow();
    a.setActiveWindow(w);

    a.setWindowIcon(QIcon(":/assets/ncs.ico"));
    w->show();

    a.exec();
    delete w;
    return 0;
}
