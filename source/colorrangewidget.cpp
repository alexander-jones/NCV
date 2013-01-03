#include "colorrangewidget.h"
#include <QApplication>
#include <QtCore/qmath.h>

ColorRangeWidget::ColorRangeWidget(QWidget *parent, int width, int height) :
    QWidget(parent)
{
    m_leftMouseDown = false;
    m_rightMouseDown = false;

    m_lowThreshold = 0.0;
    m_highThreshold = 1.0;

    m_cursorImage = new QImage(":/assets/triangle.png");
    m_layout = new QVBoxLayout();
    m_layout->setAlignment(Qt::AlignCenter);
    m_rangeLayerPadding = 100;

    m_cursorRangeContainer = new ImageContainer();
    connect(m_cursorRangeContainer,SIGNAL(doubleClicked(Qt::MouseButton,QPoint)),this,SLOT(m_cursorRangeDoubleClicked(Qt::MouseButton,QPoint)));
    m_cursorRangeContainer->installEventFilter( this );
    m_layout->addWidget(m_cursorRangeContainer);

    m_valueLayerContainer = new ImageContainer();
    connect(m_valueLayerContainer,SIGNAL(doubleClicked(Qt::MouseButton,QPoint)),this,SLOT(m_valueLayerDoubleClicked(Qt::MouseButton,QPoint)));
    m_valueLayerContainer->installEventFilter( this );
    m_layout->addWidget(m_valueLayerContainer);
    m_rangeLayer = NULL;
    m_cursorLayer = NULL;
    m_valueLayer = NULL;
    setImageSize(width,height);


    this->setLayout(m_layout);
}

QList<Cursor> & ColorRangeWidget::getMarkers()
{
    return m_cursors;
}

void ColorRangeWidget:: setMarkers(const QList<Cursor> & markers)
{
    bool markersSame = true;
    QList<Cursor> oldCursors(m_cursors);
    qSort(oldCursors.begin(),oldCursors.end());

    QList<Cursor> newCursors(markers);
    qSort(newCursors.begin(),newCursors.end());

    for (int i = 0 ; i< markers.size();i++)
    {

        if (i < oldCursors.size())
        {
            if (newCursors[i].color != oldCursors[i].color || newCursors[i].value != oldCursors[i].value || newCursors[i].position != oldCursors[i].position)
            {
                markersSame = false;
                break;
            }

        }
        else
        {
           markersSame = false;
           break;
        }
    }
    oldCursors.clear();
    newCursors.clear();

    if (!markersSame)
    {
        m_cursors.clear();
        for (int i = 0 ; i< markers.size();i++)
        {
            Cursor cursor;
            cursor.color = markers[i].color;
            if (markers[i].cursorImage != NULL)
                cursor.cursorImage = new QImage(*markers[i].cursorImage);
            cursor.value = markers[i].value;
            cursor.position = markers[i].position;
            m_cursors.insert(0,cursor);
        }
        m_updateRange();
        markersChanged(m_cursors);
        m_updateValueLayerContainer();

    }
}

void ColorRangeWidget::setLowThreshold(float threshold)
{
    m_lowThreshold = threshold;

}

void ColorRangeWidget::setHighThreshold(float threshold)
{
    m_highThreshold = threshold;

}


void ColorRangeWidget::setImageSize(int width, int height)
{
    QImage * oldImage = m_rangeLayer;

    int imageWidth = width - 100;
    int imageHeight = height;

    m_rangeLayer = new QImage(imageWidth,imageHeight,QImage::Format_RGB32);
    if (oldImage != NULL)
    {
        for (int y = 0; y < imageHeight; y++)
            for (int x = 0; x < imageWidth; x++)
            {
                if (x < oldImage->width() && y < oldImage->height())
                    m_rangeLayer->setPixel(x,y,oldImage->pixel(x,y));
                else
                    m_rangeLayer->setPixel(x,y,qRgb(0,0,0));
            }
        delete oldImage;

    }

    if (m_cursorLayer != NULL)
        delete m_cursorLayer;
    m_cursorLayer = new QImage(width,imageHeight,QImage::Format_ARGB32);
    m_cursorLayer->fill(QColor(0,0,0,0));
    m_updateCursorRangeContainer();

    if (m_valueLayer != NULL)
        delete m_valueLayer;
    m_valueLayer = new QImage(width ,m_cursorImage->height(),QImage::Format_ARGB32);
    m_valueLayer->fill(QColor(0,0,0,0));
    m_updateValueLayerContainer();


}
QSize ColorRangeWidget::getImageSize()
{
    return m_rangeLayer->size();
}

void ColorRangeWidget::setRangeToColor(float start,float end, QRgb color)
{
    int startPixel =  ((start - m_lowThreshold) / (m_highThreshold - m_lowThreshold) )* m_rangeLayer->width() ;
    int endPixel =  ((end - m_lowThreshold) / (m_highThreshold - m_lowThreshold) )* m_rangeLayer->width() ;

    if (startPixel <= m_rangeLayer->width() && endPixel <= m_rangeLayer->width())
    {
        for (int y = 0; y < m_rangeLayer->height(); y++)
            for (int x = startPixel; x < endPixel; x++)
                m_rangeLayer->setPixel(x,y,color);


        if (m_cursors.size() ==0)
        {
            Cursor startCursor;
            startCursor.color = color;
            startCursor.value = start;
            startCursor.position = startPixel;
            startCursor.cursorImage = new QImage(*m_cursorImage);
            for (int y = 0; y < startCursor.cursorImage->height(); y++)
                for (int x = 0; x < startCursor.cursorImage->width(); x++)
                {
                    QColor pixel = startCursor.cursorImage->pixel(x,y);
                    if (pixel.redF()  >0.05f && pixel.blueF() >0.05f  && pixel.greenF() >0.05f)
                        startCursor.cursorImage->setPixel(x,y,color);
                }
            m_cursors.insert(0,startCursor);

            Cursor endCursor;
            endCursor.color = color;
            endCursor.value = end;
            endCursor.position = endPixel;
            endCursor.cursorImage = new QImage(*m_cursorImage);
            for (int y = 0; y < endCursor.cursorImage->height(); y++)
                for (int x = 0; x < endCursor.cursorImage->width(); x++)
                {
                    QColor pixel = endCursor.cursorImage->pixel(x,y);
                    if (pixel.redF()  >0.05f && pixel.blueF() >0.05f  && pixel.greenF() >0.05f)
                        endCursor.cursorImage->setPixel(x,y,color);
                }
            m_cursors.insert(0,endCursor);
            markersChanged(m_cursors);

        }
        colorRangeChanged(getData());
        m_updateCursorRangeContainer();

    }

}

void ColorRangeWidget::setRangeToGradient(float start,float end, QColor startColor, QColor endColor)
{
    int startPixel =  ((start - m_lowThreshold) / (m_highThreshold - m_lowThreshold) )* m_rangeLayer->width();
    int endPixel =  ((end - m_lowThreshold) / (m_highThreshold - m_lowThreshold) )* m_rangeLayer->width() ;

    if (startPixel <= m_rangeLayer->width() && endPixel <= m_rangeLayer->width())
    {
        QLinearGradient gradient(startPixel,0,endPixel,0);
        gradient.setColorAt(0, startColor);
        gradient.setColorAt(1, endColor);
        QRect rect(startPixel,0,endPixel-startPixel,m_rangeLayer->height());
        QPainter painter;
        painter.begin(m_rangeLayer);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillRect(rect, QBrush(gradient));
        painter.end();

        if (m_cursors.size() ==0)
        {
            Cursor startCursor;
            startCursor.color = startColor;
            startCursor.value = start;
            startCursor.position = startPixel;
            startCursor.cursorImage = new QImage(*m_cursorImage);
            for (int y = 0; y < startCursor.cursorImage->height(); y++)
                for (int x = 0; x < startCursor.cursorImage->width(); x++)
                {
                    QColor pixel = startCursor.cursorImage->pixel(x,y);
                    if (pixel.redF()  >0.05f && pixel.blueF() >0.05f  && pixel.greenF() >0.05f)
                        startCursor.cursorImage->setPixel(x,y,startColor.rgb());
                }
            m_cursors.insert(0,startCursor);

            Cursor endCursor;
            endCursor.color = endColor;
            endCursor.value = end;
            endCursor.position = endPixel;
            endCursor.cursorImage = new QImage(*m_cursorImage);
            for (int y = 0; y < endCursor.cursorImage->height(); y++)
                for (int x = 0; x < endCursor.cursorImage->width(); x++)
                {
                    QColor pixel = endCursor.cursorImage->pixel(x,y);
                    if (pixel.redF()  >0.05f && pixel.blueF() >0.05f  && pixel.greenF() >0.05f)
                        endCursor.cursorImage->setPixel(x,y,endColor.rgb());
                }
            m_cursors.insert(0,endCursor);
            markersChanged(m_cursors);
            m_updateValueLayerContainer();

        }

        colorRangeChanged(getData());
        m_updateCursorRangeContainer();


    }

}
QRgb * ColorRangeWidget::getData()
{
    QRgb * pixels =  (QRgb *)m_rangeLayer->scanLine(0);
    return pixels;
}


void  ColorRangeWidget::m_updateRange()
{

    if (m_cursors.size() >=2)
    {
        QList<Cursor> valueSortedCursors (m_cursors);
        qSort(valueSortedCursors.begin(),valueSortedCursors.end());
        for (int i = 1; i < valueSortedCursors.size(); i ++)
        {
            if (valueSortedCursors[i].color == valueSortedCursors[i-1].color)
                setRangeToColor(valueSortedCursors[i-1].value,valueSortedCursors[i].value,valueSortedCursors[i].color.rgb());
            else
                setRangeToGradient(valueSortedCursors[i-1].value,valueSortedCursors[i].value,valueSortedCursors[i-1].color.rgb(),valueSortedCursors[i].color.rgb());
        }
    }
    else
        m_updateCursorRangeContainer();
}
void ColorRangeWidget::m_updateCursorRangeContainer()
{
    m_cursorLayer->fill(QColor(0,0,0,0));
    QPainter painter(m_cursorLayer);

    painter.drawImage(QPoint(m_rangeLayerPadding/4,0),*m_rangeLayer);
    for (int i =0; i < m_cursors.size(); i ++)
        painter.drawImage(QPoint(m_rangeLayerPadding/4 + m_cursors[i].position-(m_cursorImage->width()/2),m_cursorLayer->height() - m_cursorImage->height()),*m_cursors[i].cursorImage);
    m_cursorRangeContainer->setPixmap(QPixmap::fromImage(*m_cursorLayer));
}

void ColorRangeWidget::m_updateValueLayerContainer()
{
    m_valueLayer->fill(QColor(0,0,0,0));
    QPainter painter(m_valueLayer);

    for (int i =0; i < m_cursors.size(); i ++)
    {
        QFont font = painter.font();
        font.setPointSize(8);
        painter.setFont(font);
        QString text = QString::number(m_cursors[i].value);
        QSize size = QFontMetrics(text).size(Qt::TextSingleLine,text);
        int width = m_cursors[i].position + m_rangeLayerPadding/4;
        QRect rect = QRect(width-size.width()/2,0,size.width(),size.height() );
        painter.drawText(rect,Qt::AlignCenter,text);
        if (i ==0 && m_leftMouseDown)
        {
            QPen pen= painter.pen();
            pen.setColor(QColor(0,0,0,0));
            painter.setPen(pen);
        }


    }

    m_valueLayerContainer->setPixmap(QPixmap::fromImage(*m_valueLayer));
}

void ColorRangeWidget::m_cursorRangeDoubleClicked(Qt::MouseButton button,QPoint pos)
{
    if (button == Qt::LeftButton)
    {
        bool overCursor = m_markerSelected(pos);
        if (overCursor)
        {
            QColorDialog colorPicker;
            colorPicker.setWindowTitle("Select Marker Color");
            colorPicker.exec();
            QColor color = colorPicker.selectedColor();
            if (color.isValid())
            {
                m_cursors[0].color = color;
                delete m_cursors[0].cursorImage;
                m_cursors[0].cursorImage = new QImage(*m_cursorImage);
                for (int y = 0; y < m_cursors[0].cursorImage->height(); y++)
                    for (int x = 0; x < m_cursors[0].cursorImage->width(); x++)
                    {
                        QColor pixel = m_cursors[0].cursorImage->pixel(x,y);
                        if (pixel.redF()  >0.05f && pixel.blueF() >0.05f  && pixel.greenF() >0.05f)
                            m_cursors[0].cursorImage->setPixel(x,y,color.rgb());
                    }

                m_updateRange();
                markersChanged(m_cursors);
            }

            m_updateValueLayerContainer();
        }
    }

}
void ColorRangeWidget::m_valueLayerDoubleClicked(Qt::MouseButton button,QPoint pos)
{
    if (button == Qt::LeftButton)
    {
        bool textSelected = m_valueSelected(pos);
        if (textSelected)
        {
            double newVal = QInputDialog::getDouble(this,"Enter Marker Value"," Enter new marker value.",m_cursors[0].value,m_lowThreshold,m_highThreshold,3);
            m_cursors[0].value = newVal;
            m_cursors[0].position =  ((newVal - m_lowThreshold) / (m_highThreshold - m_lowThreshold) )* m_rangeLayer->width() ;
            m_updateRange();
            m_updateValueLayerContainer();
        }
    }
}

bool ColorRangeWidget::eventFilter( QObject* watched, QEvent* event )
{

    QMouseEvent* me;
    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease ||event->type() ==QEvent::MouseMove)
        me= (QMouseEvent*) event ;
    else
        return false;
    if (event->type() == QEvent::MouseButtonPress)
    {
        if (me->button() == Qt::LeftButton)
            m_leftMouseDown = true;
        else if (me->button() == Qt::RightButton)
            m_rightMouseDown = true;

    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        if (me->button() == Qt::LeftButton)
            m_leftMouseDown = false;
        else if (me->button() == Qt::RightButton)
            m_rightMouseDown = false;
    }

    int imageWidth = qMin(qMax(me->pos().x(),m_rangeLayerPadding/4),m_rangeLayer->width()+m_rangeLayerPadding/4)-m_rangeLayerPadding/4;
    float val = (((float)(imageWidth)/(float)m_rangeLayer->width()) *(m_highThreshold - m_lowThreshold)) + m_lowThreshold;
    val = qMin(qMax(val,m_lowThreshold),m_highThreshold);
    val = float(qCeil(val * 10)) / 10;


    if ( watched == m_cursorRangeContainer  )
    {
        bool overCursor = m_markerSelected(me->pos());

        if (m_leftMouseDown)
        {
            if (!overCursor && event->type() == QEvent::MouseButtonPress)
            {
                Cursor cursor;
                cursor.cursorImage = new QImage(*m_cursorImage);
                cursor.color = QColor(255,255,255);
                m_cursors.insert(0,cursor);

            }
            m_cursors[0].position =imageWidth;
            m_cursors[0].value =val;
            m_updateRange();
            markersChanged(m_cursors);
            m_updateValueLayerContainer();

        }
        else if (m_rightMouseDown)
        {
            if ( overCursor)
            {
            if (m_cursors.size() > 0)
                m_cursors.removeAt(0);
            m_updateRange();
            markersChanged(m_cursors);
            m_updateValueLayerContainer();
            }


        }
        else
            m_updateValueLayerContainer();


    }
    else if (watched == m_valueLayerContainer)
    {


        bool textSelected = m_valueSelected(me->pos());

        if (m_leftMouseDown)
        {

            m_cursors[0].position =imageWidth;
            m_cursors[0].value =val;
            m_updateRange();

            markersChanged(m_cursors);
            m_updateValueLayerContainer();

        }
        else if (m_rightMouseDown)
        {
            if ( textSelected)
            {
                if (m_cursors.size() > 0)
                    m_cursors.removeAt(0);
                m_updateRange();

                markersChanged(m_cursors);
                m_updateValueLayerContainer();
            }


        }
        else
            m_updateValueLayerContainer();


    }

    return false;
}
bool ColorRangeWidget::m_valueSelected(QPoint pos)
{
    int position = pos.x();
    bool textSelected = false;
    for (int i = 0; i < m_cursors.size(); i ++)
    {
        QString text = QString::number(m_cursors[i].value);
        QSize size = QFontMetrics(text).size(Qt::TextSingleLine,text);
        int width = m_cursors[i].position + m_rangeLayerPadding/4;
        int relativeWidth  =  (position - width) + size.width()/2;
        if (relativeWidth >= 0 && relativeWidth < size.width())
        {
            if (i != 0)
            {
                Cursor cursor = m_cursors[i];
                m_cursors.removeAt(i);
                m_cursors.insert(0,cursor);
            }
            textSelected = true;
            break;
        }

    }
    return textSelected;
}

bool ColorRangeWidget::m_markerSelected(QPoint pos)
{


    int imageWidth = qMin(qMax(pos.x(),m_rangeLayerPadding/4),m_rangeLayer->width()+m_rangeLayerPadding/4)-m_rangeLayerPadding/4;
    float val = (((float)(imageWidth)/(float)m_rangeLayer->width()) *(m_highThreshold - m_lowThreshold)) + m_lowThreshold;
    val = qMin(qMax(val,m_lowThreshold),m_highThreshold);
    int height = pos.y();
    int cursorImagePos = (m_cursorLayer->height() - m_cursorImage->height());
    bool overCursor;
    if (height >= 0 && height < cursorImagePos)
        overCursor =false;
    else if (height >=cursorImagePos && height < m_cursorLayer->height())
    {
        int relativeHeight = height - cursorImagePos;
        overCursor = false;
        for (int i = 0; i < m_cursors.size(); i ++)
        {
            int relativeWidth  =  (imageWidth -m_cursors[i].position) + m_cursorImage->width()/2;
            if (m_cursorImage->rect().contains(relativeWidth,relativeHeight))
            {
                QColor atCursor = QColor::fromRgba(m_cursorImage->pixel(relativeWidth,relativeHeight));
                if (atCursor.alpha() > 0)
                {
                    if (i != 0)
                    {
                        Cursor cursor = m_cursors[i];
                        m_cursors.removeAt(i);
                        m_cursors.insert(0,cursor);
                    }
                    overCursor = true;
                    break;
                }
            }

        }
    }
    return overCursor;
}


void ColorRangeWidget::setColorRange(QRgb * newRange)
{
    for (int y = 0; y < m_rangeLayer->height(); y++)
        for (int x = 0; x < m_rangeLayer->width(); x++)
            m_rangeLayer->setPixel(x,y,newRange[x]);
    colorRangeChanged(getData());

    m_updateCursorRangeContainer();
}
