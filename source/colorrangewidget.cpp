#include "colorrangewidget.h"
#include <QDebug>
#include <QPainter>
#include <QRect>
#include <QEvent>
#include <QMouseEvent>
#include <QColorDialog>

ColorRangeWidget::ColorRangeWidget(QWidget *parent, int width, int height) :
    QWidget(parent)
{
    m_startValueSelected = false;
    m_endValueSelected = false;

    m_endSelectedVal = 0.0;
    m_highThreshold = 1.0;

    m_layout = new QVBoxLayout();
    m_layout->setAlignment(Qt::AlignCenter);
    m_title = new QLabel("");
    m_title->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(m_title);
    m_imageContainer = new QLabel();
    m_image = new QImage(width,height,QImage::Format_RGB32);
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            m_image->setPixel(x,y,qRgb(rand() % 255,rand() %255,rand() %255));
    m_imageContainer->setPixmap(QPixmap::fromImage(*m_image));
    m_imageContainer->installEventFilter( this );
    m_layout->addWidget(m_imageContainer);


    m_startCursor = new QImage(":/assets/startTriangle.png");
    m_endCursor = new QImage(":/assets/endTriangle.png");
    m_cursorImageContainer = new QLabel();
    m_cursorImage = new QImage(width + m_startCursor->width(),m_startCursor->height(),QImage::Format_ARGB32);
    m_cursorImage->fill(QColor(0,0,0,0));
    m_cursorImageContainer->setPixmap(QPixmap::fromImage(*m_cursorImage));
    m_cursorImageContainer->installEventFilter( this );

    m_startCursorImage = new QImage(width,m_startCursor->height(),QImage::Format_ARGB32);
    m_startCursorImage->fill(QColor(0,0,0,0));

    m_endCursorImage = new QImage(width,m_startCursor->height(),QImage::Format_ARGB32);
    m_endCursorImage->fill(QColor(0,0,0,0));
    m_layout->addWidget(m_cursorImageContainer);

    m_rangeAxisLabel = new QLabel();
    m_spaceWidth = QFontMetrics(m_rangeAxisLabel->font()).width(" ");
    m_setAxisLabel();
    m_layout->addWidget(m_rangeAxisLabel);

    m_buttonWidget = new QWidget();
    m_buttonLayout = new QHBoxLayout();
    m_gradientButton = new QPushButton("Set Range To Gradient");
    m_gradientButton->setEnabled(false);
    connect(m_gradientButton,SIGNAL(clicked()),this,SLOT(m_gradientButtonClicked()));
    m_buttonLayout->addWidget(m_gradientButton);
    m_colorButton =  new QPushButton("Set Range To Color");
    m_colorButton->setEnabled(false);
    connect(m_colorButton,SIGNAL(clicked()),this,SLOT(m_colorButtonClicked()));
    m_buttonLayout->addWidget(m_colorButton);
    m_buttonWidget->setLayout(m_buttonLayout);
    m_layout->addWidget(m_buttonWidget);

    this->setLayout(m_layout);
}

void ColorRangeWidget::setLowThreshold(float threshold)
{
    m_lowThreshold = threshold;
    m_startSelectedVal = m_lowThreshold;
    m_endSelectedVal = m_lowThreshold;
    m_setAxisLabel();
}

void ColorRangeWidget::setHighThreshold(float threshold)
{
    m_highThreshold = threshold;
    m_setAxisLabel();
}

void  ColorRangeWidget::m_setAxisLabel()
{
    QString lowString =QString("%1").arg(m_lowThreshold);
    int lowStringWidth = QFontMetrics(m_rangeAxisLabel->font()).width(lowString);

    QString highString = QString("%1").arg(m_highThreshold);
    int highStringWidth = QFontMetrics(m_rangeAxisLabel->font()).width(highString);

    int availableSpace = m_image->width() - (lowStringWidth + highStringWidth);
    int midChars = availableSpace / m_spaceWidth;

    QString midString;
    for(int i = 0; i < midChars; i ++)
        midString += " ";
    m_rangeAxisLabel->setText(lowString + midString + highString);

}
void ColorRangeWidget::setTitle(QString title)
{
    m_title->setText(title);
}

void ColorRangeWidget::setImageSize(int width, int height)
{
    QImage * oldImage = m_image;
    m_image = new QImage(width,height,QImage::Format_RGB32);
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
        {
            if (x < oldImage->width() && y < oldImage->height())
                m_image->setPixel(x,y,oldImage->pixel(x,y));
            else
                m_image->setPixel(x,y,qRgb(0,0,0));
        }
    m_imageContainer->setPixmap(QPixmap::fromImage(*m_image));
    delete oldImage;

    QImage * oldCursorImage = m_cursorImage;
    m_cursorImage = new QImage(width+ m_startCursor->width(),m_startCursor->height(),QImage::Format_ARGB32);
    m_cursorImage->fill(QColor(0,0,0,0));
    m_cursorImageContainer->setPixmap(QPixmap::fromImage(*m_cursorImage));

    QImage * oldStartCursorImage = m_startCursorImage;
    m_startCursorImage = new QImage(width ,m_startCursor->height(),QImage::Format_ARGB32);
    m_startCursorImage->fill(QColor(0,0,0,0));

    QImage * oldEndCursorImage = m_endCursorImage;
    m_endCursorImage = new QImage(width ,m_endCursor->height(),QImage::Format_ARGB32);
    m_endCursorImage->fill(QColor(0,0,0,0));

    delete oldStartCursorImage;
    delete oldEndCursorImage;
    delete oldCursorImage;
}

void ColorRangeWidget::setRangeToColor(float start,float end, QRgb color)
{
    int startPixel =  ((start - m_lowThreshold) / (m_highThreshold - m_lowThreshold) )* m_image->width() ;
    int endPixel =  ((end - m_lowThreshold) / (m_highThreshold - m_lowThreshold) )* m_image->width() ;

    if (startPixel <= m_image->width() && endPixel <= m_image->width())
    {
        for (int y = 0; y < m_image->height(); y++)
            for (int x = startPixel; x <= endPixel; x++)
                m_image->setPixel(x,y,color);
        m_imageContainer->setPixmap(QPixmap::fromImage(*m_image));
    }

}

void ColorRangeWidget::setRangeToGradient(float start,float end, QColor startColor, QColor endColor)
{
    int startPixel =  ((start - m_lowThreshold) / (m_highThreshold - m_lowThreshold) )* m_image->width();
    int endPixel =  ((end - m_lowThreshold) / (m_highThreshold - m_lowThreshold) )* m_image->width() ;

    if (startPixel <= m_image->width() && endPixel <= m_image->width())
    {
        QLinearGradient gradient(startPixel,0,endPixel,0);
        gradient.setColorAt(0, startColor);
        gradient.setColorAt(1, endColor);
        QRect rect(startPixel,0,endPixel-startPixel,m_image->height());
        QPainter painter;
        painter.begin(m_image);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillRect(rect, QBrush(gradient));
        painter.end();
        m_imageContainer->setPixmap(QPixmap::fromImage(*m_image));
        colorRangeChanged(getData());

    }

}
QRgb * ColorRangeWidget::getData()
{
    QRgb * pixels =  (QRgb *)m_image->scanLine(0);
    return pixels;
}

bool ColorRangeWidget::eventFilter( QObject* watched, QEvent* event )
{
    if ( watched != m_imageContainer  && watched != m_cursorImageContainer)
        return false;
    if ( event->type() != QEvent::MouseButtonPress )
        return false;

    const QMouseEvent* const me = (const QMouseEvent*) event ;
    float val = (((float)me->pos().x()/(float)m_image->width()) *(m_highThreshold - m_lowThreshold)) + m_lowThreshold;
    if (me->button() == Qt::LeftButton)
    {
        m_startSelectedVal = val;
        m_startValueSelected = true;
        if(m_endValueSelected)
        {
            m_colorButton->setEnabled(true);
            m_gradientButton->setEnabled(true);
        }
        m_startCursorImage->fill(QColor(0,0,0,0));
        QPainter painter(m_startCursorImage);
        painter.drawImage(QPoint(me->pos().x() -m_startCursor->width()/2,0),*m_startCursor);

        m_cursorImage->fill(QColor(0,0,0,0));
        QPainter painterTwo(m_cursorImage);
        painterTwo.drawImage(QPoint(0,0),*m_startCursorImage);
        painterTwo.drawImage(QPoint(0,0),*m_endCursorImage);
        m_cursorImageContainer->setPixmap(QPixmap::fromImage(*m_cursorImage));
    }
    else if (me->button()  == Qt::RightButton)
    {
        m_endSelectedVal = val;
        m_endValueSelected = true;
        if(m_startValueSelected)
        {
            m_colorButton->setEnabled(true);
            m_gradientButton->setEnabled(true);
        }
        m_endCursorImage->fill(QColor(0,0,0,0));
        QPainter painter(m_endCursorImage);
        painter.drawImage(QPoint(me->pos().x() -m_endCursor->width()/2,0),*m_endCursor);

        m_cursorImage->fill(QColor(0,0,0,0));
        QPainter painterTwo(m_cursorImage);
        painterTwo.drawImage(QPoint(0,0),*m_startCursorImage);
        painterTwo.drawImage(QPoint(0,0),*m_endCursorImage);
        m_cursorImageContainer->setPixmap(QPixmap::fromImage(*m_cursorImage));
    }


    return false;
}

void ColorRangeWidget::m_gradientButtonClicked()
{
    QColorDialog colorPicker;
    colorPicker.setWindowTitle("Select First Color");
    colorPicker.exec();
    QColor firstCol = colorPicker.selectedColor();

    if (firstCol.isValid())
    {
        colorPicker.setWindowTitle("Select Second Color");
        colorPicker.exec();
        QColor secondCol = colorPicker.selectedColor();
        if (secondCol.isValid())
        {
            setRangeToGradient(m_startSelectedVal,m_endSelectedVal,firstCol,secondCol);
            m_resetCursorSection();
        }
    }

}
void ColorRangeWidget::m_colorButtonClicked()
{
    QColorDialog colorPicker;
    colorPicker.exec();
    QColor col = colorPicker.selectedColor();
    if (col.isValid())
    {
        setRangeToColor(m_startSelectedVal,m_endSelectedVal,qRgb(col.red(),col.green(),col.blue()));
        m_resetCursorSection();
    }

}

void ColorRangeWidget::m_resetCursorSection()
{
    m_startValueSelected = false;
    m_endValueSelected = false;
    m_colorButton->setEnabled(false);
    m_gradientButton->setEnabled(false);
    colorRangeChanged(getData());
    m_cursorImage->fill(QColor(0,0,0,0));
    m_startCursorImage->fill(QColor(0,0,0,0));
    m_endCursorImage->fill(QColor(0,0,0,0));
    m_cursorImageContainer->setPixmap(QPixmap::fromImage(*m_cursorImage));
}
