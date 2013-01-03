#include "colorbitwidget.h"

ColorBitWidget::ColorBitWidget(QWidget *parent) :
    QWidget(parent)
{
    m_layout = new QHBoxLayout();
    m_layout->setAlignment(Qt::AlignCenter);
    m_offColorContainer = new QLabel();
    m_offColor = new QImage(25,25,QImage::Format_RGB32);
    m_offColor->fill(QColor(0,0,0));
    m_offColorContainer->setPixmap(QPixmap::fromImage(*m_offColor));
    m_offColorContainer->installEventFilter( this );


    m_offColorLabel = new LabeledWidget("Off Color",m_offColorContainer);
    m_layout->addWidget(m_offColorLabel);


    m_onColorContainer = new QLabel();
    m_onColor = new QImage(25,25,QImage::Format_RGB32);
    m_onColor->fill(QColor(255,255,255));
    m_onColorContainer->setPixmap(QPixmap::fromImage(*m_onColor));
    m_onColorContainer->installEventFilter( this );


    m_onColorLabel = new LabeledWidget("On Color",m_onColorContainer);
    m_layout->addWidget(m_onColorLabel);

    this->setLayout(m_layout);
}

QColor ColorBitWidget::getOnColor()
{
    return m_onColor->pixel(0,0);
}

QColor ColorBitWidget::getOffColor()
{
    return m_offColor->pixel(0,0);
}
void ColorBitWidget::setOffColor( QColor color )
{
    m_offColor->fill(color);
    m_offColorContainer->setPixmap(QPixmap::fromImage(*m_offColor));
    colorsChanged(color,m_onColor->pixel(0,0));
}

void ColorBitWidget::setOnColor( QColor color )
{
    m_onColor->fill(color);
    m_onColorContainer->setPixmap(QPixmap::fromImage(*m_onColor));
    colorsChanged(m_offColor->pixel(0,0),color);
}
void ColorBitWidget::setColors(QColor offColor, QColor onColor)
{
    m_offColor->fill(offColor);
    m_offColorContainer->setPixmap(QPixmap::fromImage(*m_offColor));

    m_onColor->fill(onColor);
    m_onColorContainer->setPixmap(QPixmap::fromImage(*m_onColor));
    colorsChanged(offColor,onColor);
}

bool ColorBitWidget::eventFilter( QObject* watched, QEvent* event )
{
    if ( watched != m_offColorContainer  && watched != m_onColorContainer)
        return false;
    if ( event->type() != QEvent::MouseButtonPress )
        return false;

    QColorDialog colorPicker;
    if (watched == m_offColorContainer)
        colorPicker.setWindowTitle("Select Off Color");
    else
        colorPicker.setWindowTitle("Select On Color");
    colorPicker.exec();
    QColor col = colorPicker.selectedColor();

    if (col.isValid())
    {
        if (watched == m_offColorContainer)
        {
            m_offColor->fill(col);
            m_offColorContainer->setPixmap(QPixmap::fromImage(*m_offColor));
            colorsChanged(col,m_onColor->pixel(0,0));
        }
        else
        {
            m_onColor->fill(col);
            m_onColorContainer->setPixmap(QPixmap::fromImage(*m_onColor));
            colorsChanged(m_offColor->pixel(0,0),col);
        }
    }
    return false;
}
