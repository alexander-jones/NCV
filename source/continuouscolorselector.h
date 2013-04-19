#ifndef CONTINUOUSCOLORSELECTOR_H
#define CONTINUOUSCOLORSELECTOR_H

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
public:
    explicit ImageContainer(QWidget * parent = 0):QLabel(parent){}

signals:
    void doubleClicked(Qt::MouseButton button, QPoint pos);
    void exited();


protected:

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

    Marker(Type type = Solid, Size size = Small);
    QColor leftColor();
    QColor rightColor();
    QImage image();
    int position();
    bool solid();
    bool divided();
    Type type();
    float value();
    Size size() ;
    void setPosition(int position);
    void setSize(Size size);
    void setValue(float value);
    void setRightColor(QColor color);
    void setLeftColor(QColor color);
    void setColor(QColor color);
    bool operator< (const Marker& other) const;
    bool operator == (const Marker& other) const;
    bool operator != (const Marker& other) const;

private:

    void m_setColors(QColor leftColor, QColor rightColor);
    void m_drawImage();
	static QColor m_leftColorTemplate(){return QColor(255,0,0);}
	static QColor m_leftColorBorderTemplate(){return QColor(0,255,255);}
	static QColor m_rightColorTemplate(){return QColor(0,255,0);}
	static QColor m_rightColorBorderTemplate(){ return QColor(255,0,255);}

    QWidget * m_parent;
    Type m_type;
    int m_position;
    float m_value;
    QColor m_leftColor,m_rightColor;
    QColor m_leftColorBorder, m_rightColorBorder;
    QImage m_image;
    Size m_size;
};

class ContinuousColorSelector : public QWidget
{
    Q_OBJECT
public:
    explicit ContinuousColorSelector(QWidget *parent = NULL);
    void setLowThreshold(float threshold);
    void setHighThreshold(float threshold);
    QVector<QRgb> getData();
    QVector<Marker> & getMarkers();
    float lowThreshold();
    float highThreshold();

signals:
    void colorRangeChanged();

public slots:
    void setData(QVector<QColor> data);
    void setData(QVector<QRgb> data);
    void setMarkers( QVector<Marker> &markers) ;
    void addMarker(float value, QColor color);
	void addMarker(float value, QColor leftColor, QColor rightColor);

private slots:
    void m_valueLayerDoubleClicked(Qt::MouseButton,QPoint);
    void m_markerRangeDoubleClicked(Qt::MouseButton,QPoint);
    void m_handleExitRange();
    void m_markerTypeSelected(QString name);

protected:
    void resizeEvent(QResizeEvent *);
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
    int m_markerHeight,m_markerWidth;
    Marker::Type m_currentMarkerType;
    QColor m_voidColor;
    QColorDialog * m_colorPicker;
    QWidget* m_buttonWidget;
    int m_selectedMarker, m_hoveredMarker;
    QImage * m_rangeLayer, * m_markerLayer,* m_valueLayer;
    QIcon m_solidMarkerIcon,m_dividedMarkerIcon;
    ImageContainer  *m_markerRangeContainer, * m_valueLayerContainer;
    QVBoxLayout * m_layout;
    QVector<Marker> m_markers;
    bool m_leftMouseDown, m_rightMouseDown, m_isEditable;
    float m_lowThreshold, m_highThreshold;
    int m_rangeLayerPadding;
};

#endif // CONTINUOUSCOLORSELECTOR_H
