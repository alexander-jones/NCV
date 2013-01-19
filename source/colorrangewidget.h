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

class ImageContainer: public QLabel
{
    Q_OBJECT
signals:
    void doubleClicked(Qt::MouseButton button, QPoint pos);

public:

    void mouseDoubleClickEvent ( QMouseEvent * event )
    {
        doubleClicked(event->button(),event->pos());
    }

};

struct Cursor
{
    Cursor()
    {
        position = 0;
        value = 0;
        color = QColor(0,0,0);
        cursorImage = NULL;
    }

    int position;
    float value;
    QColor color;
    QImage * cursorImage;

    bool operator< (const Cursor& other) const { return value < other.value; }
};

class ColorRangeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ColorRangeWidget(QWidget *parent = 0, int width = 1, int height = 1);
    void setLowThreshold(float threshold);
    void setHighThreshold(float threshold);
    void setImageSize(int width, int height);
    QSize getImageSize();
    QRgb * getData();
    QList<Cursor> & getMarkers();

    void setEditable(bool editable);

signals:
    void colorRangeChanged();

public slots:
    void setMarkers(const QList<Cursor> &markers);
    void setRangeToColor(float start,float end, QRgb color);
    void setRangeToGradient(float start,float end, QColor startColor, QColor endColor);
    void setColorRange(QRgb * newRange);

private slots:
    void m_updateCursorRangeContainer();
    void m_updateValueLayerContainer();
    void m_valueLayerDoubleClicked(Qt::MouseButton,QPoint);
    void m_cursorRangeDoubleClicked(Qt::MouseButton,QPoint);

private:


    bool eventFilter( QObject* watched, QEvent* event ) ;
    bool m_compareCursorsByValue(Cursor first, Cursor second);
    bool m_valueSelected(QPoint pos);
    bool m_markerSelected(QPoint pos);
    void m_colorCursorWidget(QColor color);
    void m_updateRange();
    void m_colorCursor(QImage * cursorImage, QColor color);

    QColorDialog * m_colorPicker;
    int m_cursorOver;
    QImage * m_rangeLayer, * m_cursorLayer, * m_cursorImage,* m_smallCursorImage,* m_valueLayer;
    ImageContainer  *m_cursorRangeContainer, * m_valueLayerContainer;
    QVBoxLayout * m_layout;
    QList<Cursor> m_cursors;
    bool m_leftMouseDown, m_rightMouseDown, m_isEditable;
    float m_lowThreshold, m_highThreshold;
    int m_rangeLayerPadding;
};

#endif // COLORRANGEWIDGET_H
