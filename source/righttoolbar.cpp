#include "righttoolbar.h"
#include "qlabel.h"
#include <QToolBar>
#include <QLayout>
#include <QSlider>
#include <QSizePolicy>

RightToolBar::RightToolBar(QWidget *parent) :
    QToolBar(parent)
{
    QLabel *data;
    QLabel *voltagelabel;
    QLabel *currentlabel;
    QLabel *firecountlabel;
    QLabel *channellabel;
    QLabel *synapticlabel;
    QLabel *simulationspeed;
    QLabel *speed;

    QWidget *space;



    QSizePolicy sizePolicy;

    space = new QWidget();
    sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);

    space->setSizePolicy(sizePolicy);


    data = new QLabel("CURRENT SELECTION");
    voltagelabel = new QLabel("Voltage:");
    currentlabel = new QLabel("Current:");
    firecountlabel = new QLabel("Firecount:");
    channellabel = new QLabel("Channel:");
    synapticlabel = new QLabel("Synaptic strengths:");


    simulationspeed = new QLabel("SIMULATION SPEED");
    speed= new QLabel("Speed:");



    data->setAlignment(Qt::AlignCenter);

    addWidget(data);
    addSeparator ();
    addWidget(voltagelabel);
    addWidget(currentlabel);
    addWidget(firecountlabel);
    addWidget(channellabel);
    addWidget(synapticlabel);

    addWidget(space);
    addSeparator ();
    addWidget(simulationspeed);
    addSeparator ();

    QSlider *slider = new QSlider(Qt::Horizontal);
    addWidget(slider);
    addWidget(speed);


    setFixedWidth(200);
    setMovable (0);

    //setAutoFillBackground(1);


}
