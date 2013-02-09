#include "colorbitwidget.h"
#include <QPainter>

ColorBitWidget::ColorBitWidget(QWidget *parent) :
    QWidget(parent)
{
    m_layout = new QHBoxLayout();
    m_layout->setAlignment(Qt::AlignCenter);

    m_offColorButton = new ColorButton();
    m_offColorButton->setText("OFF");
    connect(m_offColorButton,SIGNAL(clicked()),this,SLOT(m_offColorClicked()));


    m_layout->addWidget(m_offColorButton);

    m_onColorButton = new ColorButton();
    m_onColorButton->setText("ON");
    connect(m_onColorButton,SIGNAL(clicked()),this,SLOT(m_onColorClicked()));

    m_layout->addWidget(m_onColorButton);

    this->setLayout(m_layout);
}


QColor ColorBitWidget::getOnColor()
{
    return m_onColorButton->color();
}

QColor ColorBitWidget::getOffColor()
{
    return m_offColorButton->color();
}

void ColorBitWidget::setOffColor( QColor color )
{
    m_offColorButton->setColor(color);
    colorsChanged(color,m_onColorButton->color());
    changed();
}

void ColorBitWidget::setOnColor( QColor color )
{
    m_onColorButton->setColor(color);
    colorsChanged(m_offColorButton->color(),color);
    changed();
}
void ColorBitWidget::setColors(QColor offColor, QColor onColor)
{

    m_offColorButton->setColor(offColor);
    m_onColorButton->setColor(onColor);
    colorsChanged(offColor,onColor);
    changed();
}


void ColorBitWidget::m_offColorClicked()
{

    QColorDialog colorPicker;
    colorPicker.setWindowTitle("Select Off Color");
    colorPicker.exec();
    QColor col = colorPicker.selectedColor();

    if (col.isValid())
        setOffColor(col);
}

void ColorBitWidget::m_onColorClicked()
{

    QColorDialog colorPicker;
    colorPicker.setWindowTitle("Select On Color");
    colorPicker.exec();
    QColor col = colorPicker.selectedColor();

    if (col.isValid())
        setOnColor(col);
}
