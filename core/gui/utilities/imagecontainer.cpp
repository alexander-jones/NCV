#include "imagecontainer.h"

ImageContainer::ImageContainer(QWidget *parent) :
    QLabel(parent)
{
}


void ImageContainer::mouseDoubleClickEvent ( QMouseEvent * event )
{
    ImageContainer::doubleClicked(event->button(),event->pos());
}

void ImageContainer::leaveEvent(QEvent *)
{
    exited();
}
