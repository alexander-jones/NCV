#ifndef COLORRANGEWIDGET_H
#define COLORRANGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QVector3D>

class ColorRangeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ColorRangeWidget(QWidget *parent = 0, int width = 1, int height = 1);
    void setLowThreshold(float threshold);
    void setHighThreshold(float threshold);
    void setRangeToColor(float start,float end, QRgb color);
    void setRangeToGradient(float start,float end, QColor startColor, QColor endColor);
    void setTitle(QString title);
    void setImageSize(int width, int height);
    QRgb * getData();

signals:
    void colorRangeChanged(QRgb * newRange);

public slots:

private slots:
    void m_gradientButtonClicked();
    void m_colorButtonClicked();

private:
    bool eventFilter( QObject* watched, QEvent* event ) ;
    void m_setAxisLabel();
    void m_resetCursorSection();
    QImage * m_image, * m_cursorImage, *m_startCursorImage, *m_endCursorImage, * m_startCursor, * m_endCursor;
    QLabel * m_imageContainer, *m_cursorImageContainer, *m_rangeAxisLabel,* m_title;
    QVBoxLayout * m_layout;
    QHBoxLayout * m_buttonLayout;
    QWidget * m_buttonWidget;
    QPushButton * m_gradientButton, * m_colorButton;
    bool m_startValueSelected, m_endValueSelected;
    float m_lowThreshold, m_highThreshold;
    int m_spaceWidth;
    float m_startSelectedVal, m_endSelectedVal;
};

#endif // COLORRANGEWIDGET_H
