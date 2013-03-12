#ifndef COLORRANGEWIDGET_H
#define COLORRANGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QVector3D>
#include <QColorDialog>
#include <QDebug>
#include <QPainter>
#include <QRect>
#include <QEvent>
#include <QMouseEvent>
#include <QMessageBox>
#include <QInputDialog>
#include <QComboBox>


class ImageContainer: public QLabel
{
    Q_OBJECT
signals:
    void doubleClicked(Qt::MouseButton button, QPoint pos);
    void exited();


public:

    void mouseDoubleClickEvent ( QMouseEvent * event )
    {
        doubleClicked(event->button(),event->pos());
    }

    void leaveEvent(QEvent *)
    {
        exited();
    }

};

struct Marker
{

public:
    enum Type
    {
        Solid,
        Divided
    };
    enum Size
    {
        Large,
        Small
    };

    Marker(Type type = Solid, Size size = Small)
    {
        m_size = size;
        m_type = type;
        m_position = -1;
        m_value = -1;
        m_leftColor = QColor(0,0,0);
        m_rightColor = QColor(0,0,0);
        m_drawImage();
    }
    QColor leftColor(){return m_leftColor;}
    QColor rightColor(){return m_rightColor;}
    QImage image(){return m_image;}
    int position(){return m_position;}
    bool solid(){return m_type == Solid;}
    bool divided(){return m_type == Divided;}
    Type type(){return m_type;}
    float value(){return m_value;}
    Size size() {return m_size;}

    void setPosition(int position){m_position = position;}
    void setSize(Size size)
    {
        m_size = size;
        m_drawImage();
    }
    void setValue(float value){m_value = value;}

    void setRightColor(QColor color)
    {
        m_rightColor = color;
        m_drawImage();
    }

    void setLeftColor(QColor color)
    {
        m_leftColor = color;
        m_drawImage();
    }

    void setColor(QColor color)
    {
        m_leftColor = color;
        m_rightColor = color;
        m_drawImage();
    }

    bool operator< (const Marker& other) const { return m_value < other.m_value; }

    bool operator == (const Marker& other) const { return (m_value == other.m_value && m_leftColor == other.m_leftColor && m_rightColor == other.m_rightColor); }
    bool operator != (const Marker& other) const { return !(*this == other); }

private:
    void m_drawImage()
    {
        if (m_type == Solid)
        {
            if (m_size == Large)
                m_image = QImage(":/assets/markerWholeTemplate.png");
            else
                m_image = QImage(":/assets/markerWholeTemplateSmall.png");
        }
        else
        {

            if (m_size == Large)
                m_image = QImage(":/assets/markerDividedTemplate.png");
            else
                m_image = QImage(":/assets/markerDividedTemplateSmall.png");
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
                    if (pixel == m_leftColorTemplate())
                        pixelData[col] = m_leftColor.rgb();

                    else if (pixel == m_leftColorBorderTemplate())
                        pixelData[col] = QColor(255 - m_leftColor.red(),255- m_leftColor.green(),255- m_leftColor.blue()).rgb();

                    else if (pixel == m_rightColorTemplate())
                        pixelData[col] = m_rightColor.rgb();

                    else if (pixel == m_rightColorBorderTemplate())
                        pixelData[col] = QColor(255 - m_rightColor.red(),255- m_rightColor.green(),255- m_rightColor.blue()).rgb();
                }
            }

        }
    }



    static QColor m_leftColorTemplate(){return QColor(255,0,0);}
    static QColor m_leftColorBorderTemplate(){return QColor(0,255,255);}
    static QColor m_rightColorTemplate(){return QColor(0,255,0);}
    static QColor m_rightColorBorderTemplate(){ return QColor(255,0,255);}

    QWidget * m_parent;
    Type m_type;
    int m_position;
    float m_value;
    QColor m_leftColor,m_rightColor;
    QImage m_image;
    Size m_size;
};

class ColorRangeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ColorRangeWidget(QWidget *parent = NULL);
    void setLowThreshold(float threshold);
    void setHighThreshold(float threshold);
    void setWidth(int width);
    QSize getImageSize();
    QRgb * getData();
    QList<Marker> & getMarkers();

signals:
    void colorRangeChanged();

public slots:
    void setMarkers( QList<Marker> &markers) ;
    void addMarker(float value, QColor color);
    void addMarker(float value, QColor leftColor, QColor rightColor);

private slots:
    void m_valueLayerDoubleClicked(Qt::MouseButton,QPoint);
    void m_markerRangeDoubleClicked(Qt::MouseButton,QPoint);
    void m_handleExitRange();
    void m_markerTypeSelected(QString name);


private:
    QPoint m_clampPosToImage(QPoint pos);
    bool eventFilter( QObject* watched, QEvent* event ) ;
    void m_updateValueLayerContainer();
    bool m_compareMarkersByValue(Marker first, Marker second);
    int m_overValue(QPoint pos);
    int m_overMarker(QPoint pos);
    void m_updateRange();
    void m_colorMarker(QImage * image, QColor color);
    bool m_fillRangeLayer(float start,float end, QColor startColor, QColor endColor);

    QComboBox * m_markerSelector;
    int m_markerHeight;
    Marker::Type m_currentMarkerType;
    QColor m_voidColor;
    QColorDialog * m_colorPicker;
    QWidget* m_buttonWidget;
    int m_selectedMarker, m_hoveredMarker;
    QImage * m_rangeLayer, * m_markerLayer,* m_valueLayer;
    QIcon m_solidMarkerIcon,m_dividedMarkerIcon;
    ImageContainer  *m_markerRangeContainer, * m_valueLayerContainer;
    QVBoxLayout * m_layout;
    QList<Marker> m_markers;
    bool m_leftMouseDown, m_rightMouseDown, m_isEditable;
    float m_lowThreshold, m_highThreshold;
    int m_rangeLayerPadding;
};

#endif // COLORRANGEWIDGET_H
