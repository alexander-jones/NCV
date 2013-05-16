#include "continuouscolorselector.h"
#include <QApplication>
#include <QtCore/qmath.h>

const int NO_MARKER = -1;

 Marker::Marker(Type type , Size size )
{
	m_size = size;
	m_type = type;
	m_position = -1;
	m_value = -1;
	m_leftColor = QColor(0,0,0);
	m_rightColor = QColor(0,0,0);
	m_drawImage();
}
QColor  Marker::leftColor(){return m_leftColor;}
QColor  Marker::rightColor(){return m_rightColor;}
QImage  Marker::image(){return m_image;}
int  Marker::position(){return m_position;}
bool  Marker::solid(){return m_type == Solid;}
bool  Marker::divided(){return m_type == Divided;}
Marker::Type  Marker::type(){return m_type;}
float  Marker::value(){return m_value;}
Marker::Size  Marker::size() {return m_size;}

void  Marker::setPosition(int position)
{
    m_position = position;
}
void  Marker::setSize(Size size)
{
	 Marker::m_size = size;
	 Marker::m_drawImage();
}
void  Marker::setValue(float value)
{
    m_value = value;
}

void  Marker::setRightColor(QColor color)
{
	m_setColors(m_leftColor,color);
	m_drawImage();
}

void  Marker::setLeftColor(QColor color)
{
	m_setColors(color,m_rightColor);
	m_drawImage();

}

void  Marker::setColor(QColor color)
{
	m_setColors(color,color);
}


bool  Marker::operator< (const Marker& other) const { return m_value < other.m_value; }
bool  Marker::operator == (const Marker& other) const { return (m_value == other.m_value && m_leftColor == other.m_leftColor && m_rightColor == other.m_rightColor); }

bool  Marker::operator != (const Marker& other) const { return !(*this == other); }
void Marker::m_setColors(QColor leftColor, QColor rightColor)
{
	m_leftColor = leftColor;
	m_rightColor = rightColor;

	float leftBrightness = m_leftColor.redF() *0.299 + m_leftColor.greenF()*0.587 + m_leftColor.blueF()*0.114;
	if (leftBrightness > 0.5)
		m_leftColorBorder = QColor(0,0,0);
	else
		m_leftColorBorder = QColor(255,255,255);

	float rightBrightness = m_rightColor.redF() *0.299 + m_rightColor.greenF()*0.587 + m_rightColor.blueF()*0.114;
	if (rightBrightness > 0.5)
		m_rightColorBorder = QColor(0,0,0);
	else
		m_rightColorBorder = QColor(255,255,255);
	m_drawImage();

}
void Marker::m_drawImage()
{

	if (m_type == Solid)
	{
		if (m_size == Large)
			m_image = QImage(":/media/markerWholeTemplate.png");
		else
			m_image = QImage(":/media/markerWholeTemplateSmall.png");
	}
	else
	{

		if (m_size == Large)
			m_image = QImage(":/media/markerDividedTemplate.png");
		else
			m_image = QImage(":/media/markerDividedTemplateSmall.png");
	}

	for (int y = 0; y < m_image.height(); y++)
	{
		QRgb *pixelData = (QRgb *)m_image.scanLine(y);
		for (int col = 0 ; col < m_image.width(); col ++)
		{
			QColor pixel = QColor::fromRgb(pixelData[col]);
			int alpha = qAlpha(pixelData[col]);

			if (alpha > 200)
			{
				if (pixel == QColor(255,0,0))
					pixelData[col] = m_leftColor.rgb();

				else if (pixel == m_leftColorBorderTemplate())
					pixelData[col] = m_leftColorBorder.rgb();

				else if (pixel == m_rightColorTemplate())
					pixelData[col] = m_rightColor.rgb();

				else if (pixel == m_rightColorBorderTemplate())
					pixelData[col] = m_rightColorBorder.rgb();
			}
		}

	}
}
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

ContinuousColorSelector::ContinuousColorSelector(QWidget *parent) :
    QWidget(parent)
{
    this->setToolTip("Interact with this widget to change the range attribute's coloration.\n Left click any space without a marker to add one.\n Left click over a marker and drag to move marker.\n Double click a marker to change it's color.\n Double click a marker's text to change it's value.\n Right click a marker or marker's text' to remove it.");

    m_voidColor = QColor(0,0,0);
    m_leftMouseDown = false;
    m_rightMouseDown = false;

    m_selectedMarker = NO_MARKER;
    m_hoveredMarker = NO_MARKER;
    m_lowThreshold = 0.0;
    m_highThreshold = 1.0;



    m_layout = new QVBoxLayout();
    m_layout->setAlignment(Qt::AlignCenter);

    m_markerRangeContainer = new ImageContainer();
    m_markerRangeContainer->setAttribute(Qt::WA_TranslucentBackground);
    connect(m_markerRangeContainer,SIGNAL(doubleClicked(Qt::MouseButton,QPoint)),this,SLOT(m_markerRangeDoubleClicked(Qt::MouseButton,QPoint)));
    connect(m_markerRangeContainer,SIGNAL(exited()),this,SLOT(m_handleExitRange()));
    m_markerRangeContainer->installEventFilter( this );
    m_markerRangeContainer->setMouseTracking(true);
    m_layout->addWidget(m_markerRangeContainer);

    m_valueLayerContainer = new ImageContainer();
    connect(m_valueLayerContainer,SIGNAL(doubleClicked(Qt::MouseButton,QPoint)),this,SLOT(m_valueLayerDoubleClicked(Qt::MouseButton,QPoint)));
    m_valueLayerContainer->installEventFilter( this );
    m_layout->addWidget(m_valueLayerContainer);

    m_markerHeight = QImage(":/media/markerWholeTemplate.png").height();
    m_markerWidth = QImage(":/media/markerWholeTemplate.png").width();

    m_markerSelector = new QComboBox();
    m_markerSelector->addItem(QIcon(":/media/markerWholeTemplate.png"),"Solid");
    m_markerSelector->addItem(QIcon(":/media/markerDividedTemplate.png"),"Divided");
    connect(m_markerSelector,SIGNAL(currentIndexChanged(QString)),this,SLOT(m_markerTypeSelected(QString)));
    m_currentMarkerType = Marker::Solid;

    m_layout->addWidget(m_markerSelector);

    this->setLayout(m_layout);
    m_rangeLayerPadding = m_markerWidth/2;
    m_rangeLayer = new QImage(size().width() - m_markerWidth/2,m_markerHeight,QImage::Format_RGB32);
    m_markerLayer = new QImage(size().width(),m_markerHeight,QImage::Format_ARGB32);
    m_markerLayer->fill(QColor(0,0,0,0));
    m_valueLayer = new QImage(size().width() ,m_markerHeight,QImage::Format_ARGB32);
    m_valueLayer->fill(QColor(0,0,0,0));
    m_updateRange();
    m_updateValueLayerContainer();



}
void ContinuousColorSelector::resizeEvent(QResizeEvent *ev)
{
    QSize newSize = ev->size();
    m_rangeLayerPadding = m_markerWidth;
    m_rangeLayer = new QImage(newSize.width() - m_markerWidth* 2,m_markerHeight,QImage::Format_RGB32);
    m_markerLayer = new QImage(newSize.width()- m_markerWidth,m_markerHeight,QImage::Format_ARGB32);
    m_markerLayer->fill(QColor(0,0,0,0));
    m_valueLayer = new QImage(newSize.width() - m_markerWidth,m_markerHeight,QImage::Format_ARGB32);
    m_valueLayer->fill(QColor(0,0,0,0));

    for (int i = 0; i < m_markers.size();i++)
        m_markers[i].setPosition( (m_rangeLayer->width()) * ((m_markers[i].value() - m_lowThreshold) /(m_highThreshold - m_lowThreshold)));

    m_updateRange();
    m_updateValueLayerContainer();
}

void ContinuousColorSelector::m_markerTypeSelected(QString name)
{
    if (name == "Solid")
        m_currentMarkerType = Marker::Solid;
    else
        m_currentMarkerType = Marker::Divided;
}

QVector<Marker> & ContinuousColorSelector::getMarkers()
{
    return m_markers;
}

void ContinuousColorSelector:: setMarkers( QVector<Marker> & markers)
{

    bool markersSame = true;

    for (int i = 0 ; i< markers.size();i++)
    {
        if (i < m_markers.count())
        {
            if (markers[i] != m_markers[i])
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

    if (!markersSame)
    {
        m_markers.clear();
        for (int i = 0 ; i< markers.size();i++)
        {
            Marker marker(markers[i].type());
            marker.setLeftColor(markers[i].leftColor());
            marker.setRightColor(markers[i].rightColor());
            marker.setValue( markers[i].value());
            marker.setPosition( markers[i].position());
            m_markers.insert(0,marker);
        }
        qSort(m_markers.begin(),m_markers.end());
        m_updateRange();
        m_updateValueLayerContainer();

    }
}

void ContinuousColorSelector::setLowThreshold(float threshold)
{
    m_lowThreshold = threshold;


}

void ContinuousColorSelector::setHighThreshold(float threshold)
{
    m_highThreshold = threshold;

}

void ContinuousColorSelector::setData(QVector<QColor> data)
{
    QVector<QRgb> newData;
    for (int i = 0; i < data.count(); i ++)
        newData.append( data[i].rgb());
    setData(newData);
}

void ContinuousColorSelector::setData(QVector<QRgb> data)
{

    int imageHeight = m_markerHeight;

    if (m_rangeLayer != NULL)
        delete m_rangeLayer;
    m_rangeLayer = new QImage(data.count(),imageHeight,QImage::Format_RGB32);
    for (int i = 0; i < imageHeight; i ++)
        for (int j = 0; j < data.count(); j ++)
        {
            QRgb * pixels = (QRgb *)m_rangeLayer->scanLine(i);
            pixels[j] = data[j];
        }

    if (m_markerLayer != NULL)
        delete m_markerLayer;
    m_markerLayer = new QImage(data.count() +  m_rangeLayerPadding,imageHeight,QImage::Format_ARGB32);
    m_markerLayer->fill(QColor(0,0,0,0));


    m_markers.clear();
    float valStep = (m_highThreshold - m_lowThreshold)/2.0;
    int posStep = data.count() /2;
    for (int i = 0; i < 3;i++)
    {
        Marker marker;
        int index = qMin(posStep *i,data.count() -1);
        marker.setColor(QColor::fromRgb(data[index]));
        marker.setValue( qMin(m_lowThreshold + (valStep*i),m_highThreshold));
        marker.setPosition( index);
        m_markers.append(marker);
    }

    m_updateRange();

    if (m_valueLayer != NULL)
        delete m_valueLayer;
    m_valueLayer = new QImage(data.count() +  m_rangeLayerPadding,m_markerHeight,QImage::Format_ARGB32);
    m_valueLayer->fill(QColor(0,0,0,0));

    m_updateValueLayerContainer();

}



void ContinuousColorSelector::addMarker(float value, QColor color)
{

    int position =  ((value - m_lowThreshold) / (m_highThreshold - m_lowThreshold) )* m_rangeLayer->width();
    int toRemove =-1;
    for (int i = 0; i < m_markers.count();i++)
        if (qAbs(m_markers[i].value() - value) < 0.01 )
            toRemove = i;

    if (toRemove != -1)
        m_markers.remove(toRemove);

    Marker marker;
    marker.setColor(color);
    marker.setValue( value);
    marker.setPosition( position);
    m_markers.insert(0,marker);
    qSort(m_markers.begin(),m_markers.end());
    m_updateRange();

}

void ContinuousColorSelector::addMarker(float value, QColor leftColor, QColor rightColor)
{

    int position =  ((value - m_lowThreshold) / (m_highThreshold - m_lowThreshold) )* m_rangeLayer->width();
    int toRemove =-1;
    for (int i = 0; i < m_markers.count();i++)
        if (qAbs(m_markers[i].value() - value) < 0.01 )
            toRemove = i;

    if (toRemove != -1)
        m_markers.remove(toRemove);
    Marker marker;
    marker.setLeftColor(leftColor);
    marker.setRightColor(rightColor);
    marker.setValue( value);
    marker.setPosition( position);
    m_markers.insert(0,marker);
    qSort(m_markers.begin(),m_markers.end());
    m_updateRange();
}


bool ContinuousColorSelector::m_fillRangeLayer(float start,float end, QColor startColor, QColor endColor)
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
        colorRangeChanged();
        return true;
    }
    return false;
}


QVector<QRgb> ContinuousColorSelector::getData()
{
    QVector<QRgb> data;
    QRgb * pixels =  (QRgb *)m_rangeLayer->scanLine(0);
    for (int i= 0; i < m_rangeLayer->width(); i ++)
        data.append(pixels[i]);
    return data;
}


void  ContinuousColorSelector::m_updateRange()
{

    if (m_markers.count() > 0)
    {
        Marker selected;
        if (m_selectedMarker != NO_MARKER)
            selected = m_markers[m_selectedMarker];

        int i = 0;
        if (m_markers[i].value() > m_lowThreshold)
            m_fillRangeLayer(m_lowThreshold,m_markers[i].value(),m_markers[0].leftColor(),m_markers[0].leftColor());


        for (i = 1; i < m_markers.count(); i ++)
            m_fillRangeLayer(m_markers[i-1].value(),m_markers[i].value(),m_markers[i-1].rightColor(),m_markers[i].leftColor());


        if (m_markers[i-1].value() < m_highThreshold)
            m_fillRangeLayer(m_markers[i-1].value(),m_highThreshold,m_markers[i-1].rightColor(),m_markers[i-1].rightColor());

        if (m_selectedMarker != NO_MARKER)
            m_selectedMarker = m_markers.indexOf(selected);
    }
    //else
        //m_fillRangeLayer(m_lowThreshold,m_highThreshold,QColor(0,0,0),QColor(0,0,0));


    m_markerLayer->fill(QColor(0,0,0,0));
    QPainter painter(m_markerLayer);

    painter.drawImage(QPoint(m_rangeLayerPadding/2,0),*m_rangeLayer);

    Marker selected;
    if (m_selectedMarker != NO_MARKER)
        selected = m_markers[m_selectedMarker];

    for (int i =0; i < m_markers.count(); i ++)
    {
        if (m_selectedMarker != NO_MARKER && m_markers[i] == selected)
            continue;
        painter.drawImage(QPoint(m_rangeLayerPadding/2 + m_markers[i].position()-(m_markers[i].image().width()/2),m_markerLayer->height() - m_markers[i].image().height()),m_markers[i].image());
    }
    if (m_selectedMarker != NO_MARKER)
        painter.drawImage(QPoint(m_rangeLayerPadding/2 + selected.position()-(selected.image().width()/2),m_markerLayer->height() - selected.image().height()),selected.image());
    m_markerRangeContainer->setPixmap(QPixmap::fromImage(*m_markerLayer));
}

void ContinuousColorSelector::m_updateValueLayerContainer()
{
    m_valueLayer->fill(QColor(0,0,0,0));
    QPainter painter(m_valueLayer);

    QPen pen= painter.pen();
    for (int i =0; i < m_markers.count(); i ++)
    {
        if (m_selectedMarker !=NO_MARKER)
        {
            if (i ==m_selectedMarker)
                pen.setColor(QColor(0,0,0,255));
            else
                pen.setColor(QColor(0,0,0,50));
            painter.setPen(pen);
        }
        QFont font = painter.font();
        font.setPointSize(8);
        painter.setFont(font);
        QString text = QString::number(m_markers[i].value());
        QSize size = QFontMetrics(text).size(Qt::TextSingleLine,text);
        int width = m_markers[i].position() + m_rangeLayerPadding/2;
        QRect rect = QRect(width-size.width()/2,0,size.width(),size.height() );
        painter.drawText(rect,Qt::AlignCenter,text);


    }

    m_valueLayerContainer->setPixmap(QPixmap::fromImage(*m_valueLayer));
}

void ContinuousColorSelector::m_handleExitRange()
{

    if ( m_hoveredMarker !=NO_MARKER)
    {
        m_markers[m_hoveredMarker].setSize(Marker::Small);
        m_hoveredMarker = NO_MARKER;
        m_updateRange();
    }
}

QPoint ContinuousColorSelector::m_clampPosToImage(QPoint pos)
{
    int imagePosition = qMin(qMax(pos.x(),m_rangeLayerPadding/2),m_rangeLayer->width()+m_rangeLayerPadding/2)-m_rangeLayerPadding/2;
    return QPoint(imagePosition,pos.y());

}

void ContinuousColorSelector::m_markerRangeDoubleClicked(Qt::MouseButton button,QPoint pos)
{
    if (button == Qt::LeftButton)
    {
        int selectedMarker = m_overMarker(m_clampPosToImage( pos));

        if (selectedMarker != NO_MARKER)
        {
            QColorDialog colorPicker;
            if (m_markers[selectedMarker].solid())
                colorPicker.setWindowTitle("Select Marker Color");
            else
                colorPicker.setWindowTitle("Select Marker Left Color");
            colorPicker.exec();
            QColor firstColor = colorPicker.selectedColor();

            if (firstColor.isValid())
            {
                if (m_markers[selectedMarker].solid())
                    m_markers[selectedMarker].setColor(firstColor);

                else
                {
                    m_markers[selectedMarker].setLeftColor(firstColor);
                    colorPicker.setWindowTitle("Select Marker Right Color");
                    colorPicker.exec();
                    QColor secondColor = colorPicker.selectedColor();
                    if (secondColor.isValid())
                    {
                        m_markers[selectedMarker].setRightColor(secondColor);
                    }
                }
                m_updateRange();
            }

            m_updateValueLayerContainer();
        }
    }

}
void ContinuousColorSelector::m_valueLayerDoubleClicked(Qt::MouseButton button,QPoint pos)
{
    if (button == Qt::LeftButton)
    {
        int selectedMarker = m_overValue(pos);
        bool update = true;
        int toRemove = -1;

        if (selectedMarker != NO_MARKER)
        {
            float newVal = (float)QInputDialog::getDouble(this,"Enter Marker Value"," Enter new marker value.",m_markers[selectedMarker].value(),m_lowThreshold,m_highThreshold,3);

            if (qAbs(m_markers[selectedMarker].value() - newVal) > 0.01 )
            {
                for (int i = 0; i < m_markers.count();i++)
                    if (qAbs(m_markers[i].value() - newVal) < 0.01 )
                    {

                        QMessageBox msgBox;
                        msgBox.setText("There is already a marker at that value.");

                        QPushButton *replaceButton = msgBox.addButton(tr("Replace Old Marker"), QMessageBox::ActionRole);

                        QPushButton *keepButton = msgBox.addButton(tr("Keep Old Marker"), QMessageBox::ActionRole);
                        msgBox.exec();

                        if (msgBox.clickedButton() == keepButton)
                            update = false;
                        else if (msgBox.clickedButton() == replaceButton)
                            toRemove = i;
                        break;

                    }

                if (update)
                {

                    m_markers[selectedMarker].setValue( newVal);
                    m_markers[selectedMarker].setPosition(  ((newVal - m_lowThreshold) / (m_highThreshold - m_lowThreshold) )* m_rangeLayer->width() );

                    if (toRemove >=0)
                        m_markers.remove(0);
                    qSort(m_markers.begin(),m_markers.end());

                    m_updateRange();
                    m_updateValueLayerContainer();
                }
            }
        }
    }
}

bool ContinuousColorSelector::eventFilter( QObject* watched, QEvent* event )
{

    QMouseEvent* me;
    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease ||event->type() ==QEvent::MouseMove||event->type() ==QEvent::HoverMove ||event->type() ==QEvent::HoverLeave)
        me= (QMouseEvent*) event ;
    else
        return false;
    if (event->type() == QEvent::HoverLeave)
    {
        qDebug() << "hover leave";
    }
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
    int labelPosition = me->pos().x();
    int imagePosition = qMin(qMax(labelPosition,m_rangeLayerPadding/2),m_rangeLayer->width()+m_rangeLayerPadding/2)-m_rangeLayerPadding/2;
    int relPosition = labelPosition - m_rangeLayerPadding/2;
    float val = (((float)(imagePosition)/(float)m_rangeLayer->width()) *(m_highThreshold - m_lowThreshold)) + m_lowThreshold;
    val = qMin(qMax(val,m_lowThreshold),m_highThreshold);


    if ( watched == m_markerRangeContainer  )
    {
        if (m_selectedMarker == NO_MARKER)
            m_selectedMarker = m_overMarker(m_clampPosToImage(me->pos()));

        if (m_leftMouseDown)
        {
            if (m_selectedMarker == NO_MARKER && event->type() == QEvent::MouseButtonPress)
            {
                Marker marker(m_currentMarkerType);
                marker.setColor(QColor(255,255,255));
                marker.setPosition(imagePosition);
                marker.setValue(val);
                m_markers.insert(0,marker);
                qSort(m_markers.begin(),m_markers.end());
                m_selectedMarker = m_markers.indexOf(marker);

            }
            if (m_selectedMarker != NO_MARKER )
            {
                if (m_selectedMarker > 0 && m_markers[m_selectedMarker-1].position() ==imagePosition)
                {
                    imagePosition +=1;
                    val = (((float)(imagePosition)/(float)m_rangeLayer->width()) *(m_highThreshold - m_lowThreshold)) + m_lowThreshold;
                    val = qMin(qMax(val,m_lowThreshold),m_highThreshold);
                }

                if(m_selectedMarker < m_markers.count() -1 && m_markers[m_selectedMarker+1].position() ==imagePosition)
                {
                    imagePosition -=1;
                    val = (((float)(imagePosition)/(float)m_rangeLayer->width()) *(m_highThreshold - m_lowThreshold)) + m_lowThreshold;
                    val = qMin(qMax(val,m_lowThreshold),m_highThreshold);
                }

                m_markers[m_selectedMarker].setPosition(imagePosition);
                m_markers[m_selectedMarker].setValue(val);
                Marker selected = m_markers[m_selectedMarker];
                qSort(m_markers.begin(),m_markers.end());
                m_selectedMarker = m_markers.indexOf(selected);
            }

            m_updateRange();
            m_updateValueLayerContainer();

        }
        else if (m_rightMouseDown)
        {
            if ( m_selectedMarker != NO_MARKER)
            {
                if ( m_selectedMarker != NO_MARKER)
                    m_markers.remove(m_selectedMarker);
                m_selectedMarker = NO_MARKER;
                m_hoveredMarker = NO_MARKER;
                m_updateRange();
                m_updateValueLayerContainer();
            }


        }
        else
        {
            m_selectedMarker = NO_MARKER;
            int markerOver = m_overMarker(QPoint( relPosition,me->pos().y()));
            if (markerOver != NO_MARKER && m_hoveredMarker != markerOver)
            {
                m_hoveredMarker = markerOver;
                m_markers[m_hoveredMarker].setSize(Marker::Large);
                m_updateRange();
            }
            else if (markerOver == NO_MARKER &&  m_hoveredMarker !=NO_MARKER)
            {
                m_markers[m_hoveredMarker].setSize(Marker::Small);
                m_hoveredMarker = NO_MARKER;
                m_updateRange();
            }
            m_updateValueLayerContainer();
        }


    }
    else if (watched == m_valueLayerContainer)
    {

        if (m_selectedMarker == NO_MARKER)
            m_selectedMarker = m_overValue(me->pos());

        if (m_leftMouseDown)
        {

            if (m_selectedMarker != NO_MARKER )
            {
                if (m_selectedMarker > 0 && m_markers[m_selectedMarker-1].position() ==imagePosition)
                {
                    imagePosition +=1;
                    val = (((float)(imagePosition)/(float)m_rangeLayer->width()) *(m_highThreshold - m_lowThreshold)) + m_lowThreshold;
                    val = qMin(qMax(val,m_lowThreshold),m_highThreshold);
                }

                if(m_selectedMarker < m_markers.count() -1 && m_markers[m_selectedMarker+1].position() ==imagePosition)
                {
                    imagePosition -=1;
                    val = (((float)(imagePosition)/(float)m_rangeLayer->width()) *(m_highThreshold - m_lowThreshold)) + m_lowThreshold;
                    val = qMin(qMax(val,m_lowThreshold),m_highThreshold);
                }

                m_markers[m_selectedMarker].setPosition(imagePosition);
                m_markers[m_selectedMarker].setValue(val);
                Marker selected = m_markers[m_selectedMarker];
                qSort(m_markers.begin(),m_markers.end());
                m_selectedMarker = m_markers.indexOf(selected);
            }
            m_updateRange();

            m_updateValueLayerContainer();

        }
        else if (m_rightMouseDown)
        {
            if ( m_selectedMarker != NO_MARKER )
            {
                if ( m_selectedMarker != 0 && m_selectedMarker != m_markers.count()-1)
                    m_markers.remove(m_selectedMarker);
                m_selectedMarker = NO_MARKER;
                m_updateRange();
                m_updateValueLayerContainer();
            }


        }
        else
        {
            m_selectedMarker = NO_MARKER;
            m_updateValueLayerContainer();
        }


    }

    return false;
}
int ContinuousColorSelector::m_overValue(QPoint pos)
{
    int position = pos.x();
    for (int i = 0; i < m_markers.count(); i ++)
    {
        QString text = QString::number(m_markers[i].value());
        QSize size = QFontMetrics(text).size(Qt::TextSingleLine,text);
        int width = m_markers[i].position() + m_rangeLayerPadding/2;
        int relativeWidth  =  (position - width) + size.width()/2;
        if (relativeWidth >= 0 && relativeWidth < size.width())
        {
            return i;
        }

    }
    return NO_MARKER;
}

int ContinuousColorSelector::m_overMarker(QPoint pos)
{
    for (int i = 0; i < m_markers.count(); i ++)
    {
        int imagePos = (m_markerLayer->height() - m_markers[i].image().height());
        int relativeHeight = pos.y() - imagePos;
        int relativeWidth  =  (pos.x() -m_markers[i].position()) + m_markers[i].image().width()/2;
        if (m_markers[i].image().rect().contains(relativeWidth,relativeHeight))
        {
            QColor atMarker = QColor::fromRgba(m_markers[i].image().pixel(relativeWidth,relativeHeight));
            if (atMarker.isValid() && atMarker.alpha() > 0)
                return i;

        }

    }
    return NO_MARKER;
}

float ContinuousColorSelector::lowThreshold(){return m_lowThreshold;}
float ContinuousColorSelector::highThreshold(){return m_highThreshold;}

