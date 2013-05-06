#ifndef IMAGECONTAINER_H
#define IMAGECONTAINER_H

#include <QLabel>
#include <QMouseEvent>

class ImageContainer: public QLabel
{
    Q_OBJECT
public:
    explicit ImageContainer(QWidget * parent = 0);

signals:
    void doubleClicked(Qt::MouseButton button, QPoint pos);
    void exited();


protected:
    void mouseDoubleClickEvent ( QMouseEvent * event );
    void leaveEvent(QEvent *);
};

#endif // IMAGECONTAINER_H
