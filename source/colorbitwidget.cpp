#include "colorbitwidget.h"

ColorBitWidget::ColorBitWidget(QWidget *parent) :
    QWidget(parent)
{
    m_layout = new QGridLayout();
    m_layout->setAlignment(Qt::AlignCenter);
    m_offColorContainer = new QLabel();
    m_offColor = new QImage(100,100,QImage::Format_RGB32);
    m_offColor->fill(QColor(0,0,0));
    m_offColorContainer->setPixmap(QPixmap::fromImage(*m_offColor));
    m_offColorContainer->installEventFilter( this );
    m_layout->addWidget(m_offColorContainer,0,0);

    m_offColorLabel = new QLabel("Off Color");
    m_offColorLabel->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(m_offColorLabel,1,0);


    m_onColorContainer = new QLabel();
    m_onColor = new QImage(100,100,QImage::Format_RGB32);
    m_onColor->fill(QColor(255,255,255));
    m_onColorContainer->setPixmap(QPixmap::fromImage(*m_onColor));
    m_onColorContainer->installEventFilter( this );
    m_layout->addWidget(m_onColorContainer,0,1);


    m_onColorLabel = new QLabel("On Color");
    m_onColorLabel->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(m_onColorLabel,1,1);

    this->setLayout(m_layout);
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
    return false;
}
