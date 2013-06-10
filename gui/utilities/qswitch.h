#ifndef SWITCHBUTTON_H
#define SWITCHBUTTON_H

#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

class QSwitch: public QLabel
{
    Q_OBJECT
public:
    QSwitch(QWidget *parent = 0);
	void setSwitched(bool on = true);

protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);

    void paintEvent(QPaintEvent *);
signals:
    void pressed();
    void clicked();
    void released();
    void switched(bool on);
public:
    int m_handleOffset;
    bool m_pressed,m_on;
    int m_switchPosition;
    QPixmap m_onBackground, m_offBackground, m_handle;
};

#endif // SWITCHBUTTON_H
