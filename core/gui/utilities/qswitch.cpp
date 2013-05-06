#include "qswitch.h"

QSwitch::QSwitch(QWidget *parent )
    :QLabel(parent)
{
    m_offBackground = QPixmap(":/media/switchBackgroundOff.png");
    m_onBackground = QPixmap(":/media/switchBackgroundOn.png");
    m_handle = QPixmap(":/media/switchHandle.png");
    this->setFixedSize(m_offBackground.size());
    m_handleOffset = m_handle.size().width()/2 ;
    m_switchPosition = m_handleOffset;
    this->setMouseTracking(true);//Qt::WA_Hover
    m_pressed = false;
    m_on = true;
}

void QSwitch::setSwitched(bool on)
{
    m_on = on;
    if (m_on)
        m_switchPosition =m_handleOffset;
    else
        m_switchPosition =width() - m_handleOffset;
    update();
}
void QSwitch::mousePressEvent(QMouseEvent *ev)
{
    int eventPos =ev->pos().x();
    m_switchPosition = qMax(m_handleOffset,qMin(width() -m_handleOffset,eventPos));
    m_pressed = true;
    pressed();
}

void QSwitch::mouseReleaseEvent(QMouseEvent *ev)
{
    if (m_switchPosition <= this->width()/2)
    {
        m_switchPosition = m_handleOffset;
        if (m_on == false)
            switched(!m_on);
        m_on = true;
        update();
    }
    else
    {
        m_switchPosition =width() - m_handleOffset;
        if (m_on == true)
            switched(!m_on);
        m_on = false;
        update();
    }

    m_pressed = false;
    released();
}
void QSwitch::mouseMoveEvent(QMouseEvent *ev)
{
    if (m_pressed)
    {
        int eventPos =ev->pos().x();
        m_switchPosition = qMax(m_handleOffset,qMin(width() -m_handleOffset,eventPos));
        update();
    }
}

void QSwitch::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    if (m_switchPosition <= this->width() / 2)
        painter.drawPixmap(this->rect(),m_onBackground);
    else
        painter.drawPixmap(this->rect(),m_offBackground);

    QRect handleRect = m_handle.rect();
    painter.drawPixmap(QRect(m_switchPosition - m_handleOffset,0,handleRect.width(),handleRect.height()),m_handle);
    painter.end();
}
