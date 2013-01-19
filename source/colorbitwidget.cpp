#include "colorbitwidget.h"
#include <QPainter>


ColorBitWidget::ColorBitWidget(QWidget *parent) :
    QWidget(parent)
{
    m_offColorText = "LOW";
    m_onColorText = "HIGH";

    m_layout = new QHBoxLayout();
    m_layout->setAlignment(Qt::AlignCenter);
    //m_layout->setSpacing(0);

    m_offColorContainer = new QLabel();
    m_offColorImage = new QImage(100,25,QImage::Format_RGB32);
    m_offColorImage->fill(QColor(0,0,0));
    m_offColorContainer->setPixmap(QPixmap::fromImage(*m_offColorImage));
    m_offColorContainer->installEventFilter( this );

    m_layout->addWidget(m_offColorContainer);


    m_onColorContainer = new QLabel();
    m_onColorImage = new QImage(100,25,QImage::Format_RGB32);
    m_onColorImage->fill(QColor(255,255,255));
    m_onColorContainer->setPixmap(QPixmap::fromImage(*m_onColorImage));
    m_onColorContainer->installEventFilter( this );


    m_layout->addWidget(m_onColorContainer);

    this->setLayout(m_layout);
}

QColor ColorBitWidget::getOnColor()
{
    return m_onColor;
}

QColor ColorBitWidget::getOffColor()
{
    return m_offColor;
}

void ColorBitWidget::m_setOffColor( QColor color )
{
    m_offColor = color;
    QColor oppositeColor = QColor(255-color.red(),255-color.green(),255-color.blue());
    QPainter painter(m_offColorImage);
    QRect textRect = QFontMetrics(painter.font()).tightBoundingRect(m_offColorText);
    QRect imgRect = m_offColorImage->rect();
    QPoint topLeft = QPoint((imgRect.width() - textRect.width())/2 +1,(imgRect.height() - textRect.height())/4 +1);
    QRect drawRect = QRect(topLeft.x(),topLeft.y(),topLeft.x() +textRect.width(),topLeft.y() +textRect.height());

    m_offColorImage->fill(color );
    QPen pen = painter.pen();
    pen.setColor(QColor(0,0,0));
    painter.setPen(pen);
    painter.drawRect(imgRect.x(),imgRect.y(),imgRect.width()-1,imgRect.height()-1);

    pen.setColor(oppositeColor);
    painter.setPen(pen);
    painter.drawRect(imgRect.x()+1,imgRect.y()+1,imgRect.width()-3,imgRect.height()-3);
    painter.drawText(drawRect ,m_offColorText);
    m_offColorContainer->setPixmap(QPixmap::fromImage(*m_offColorImage));
}

void ColorBitWidget::m_setOnColor( QColor color )
{
    m_onColor = color;

    QColor oppositeColor = QColor(255-color.red(),255-color.green(),255-color.blue());
    QPainter painter(m_onColorImage);
    QRect textRect = QFontMetrics(painter.font()).tightBoundingRect(m_onColorText);
    QRect imgRect = m_onColorImage->rect();
    QPoint topLeft = QPoint((imgRect.width() - textRect.width())/2 +1,(imgRect.height() - textRect.height())/4 +1);
    QRect drawRect = QRect(topLeft.x(),topLeft.y(),topLeft.x() +textRect.width(),topLeft.y() +textRect.height());

    m_onColorImage->fill(color );

    QPen pen = painter.pen();
    pen.setColor(QColor(0,0,0));
    painter.setPen(pen);
    painter.drawRect(imgRect.x(),imgRect.y(),imgRect.width()-1,imgRect.height()-1);

    pen.setColor(oppositeColor);
    painter.setPen(pen);
    painter.drawRect(imgRect.x()+1,imgRect.y()+1,imgRect.width()-3,imgRect.height()-3);
    painter.drawText(drawRect ,m_onColorText);
    m_onColorContainer->setPixmap(QPixmap::fromImage(*m_onColorImage));

}

void ColorBitWidget::setOffColor( QColor color )
{
    m_setOffColor(color);
    colorsChanged(color,m_onColorImage->pixel(0,0));
    changed();
}

void ColorBitWidget::setOnColor( QColor color )
{
    m_setOnColor(color);
    colorsChanged(m_offColorImage->pixel(0,0),color);
    changed();
}
void ColorBitWidget::setColors(QColor offColor, QColor onColor)
{

    m_setOffColor(offColor);
    m_setOnColor(onColor);
    colorsChanged(offColor,onColor);
    changed();
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
            setOffColor(col);

        else
            setOnColor(col);
    }
    return false;
}
