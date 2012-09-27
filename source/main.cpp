#include <QApplication>
#include <QPushButton>
#include <QGLFormat>
#include <QDebug>
#include <QGridLayout>
#include "glwidget.h"
#include "mainwindow.h"

int main( int argc, char* argv[] )
{
    QApplication a( argc, argv );
    MainWindow * w = new MainWindow();
    // Specify an OpenGL 3.3 format using the Core profile.
    // That is, no old-school fixed pipeline functionality
    QGLFormat glFormat;
    glFormat.setVersion( 3, 3 );
    glFormat.setProfile( QGLFormat::CoreProfile ); // Requires >=Qt-4.8.0
    glFormat.setSampleBuffers( true );
    qDebug() << "Running OpenGL Version " <<glFormat.majorVersion() << "." <<glFormat.minorVersion();

    // Create a GLWidget requesting our format
    GLWidget *glCanvas = new GLWidget( glFormat,w );
    w->setCentralWidget(glCanvas);
    w->show();

    return a.exec();
}
