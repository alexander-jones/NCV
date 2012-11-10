
#include "mainwindow.h"
#include <QApplication>
#include <QPushButton>
#include <QDebug>
#include <QGridLayout>

int main( int argc, char* argv[] )
{
    QApplication a( argc, argv );
    MainWindow * w = new MainWindow();
    w->show();

    return a.exec();
}
