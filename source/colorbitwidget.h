#ifndef COLORBITWIDGET_H
#define COLORBITWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QColorDialog>
#include <QEvent>
#include "combowidget.h"
#include "qcustomplot.h"

class ColorBitWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ColorBitWidget(QWidget *parent = 0);
    QColor getOnColor();
    QColor getOffColor();
    
signals:
    void changed();
    void colorsChanged(QColor offColor, QColor onColor);

public slots:
    void setColors(QColor offColor, QColor onColor);
    void setOffColor(QColor color);
    void setOnColor(QColor color);
    
private:

    void m_setOffColor(QColor color);
    void m_setOnColor(QColor color);
    bool eventFilter( QObject* watched, QEvent* event ) ;

    QHBoxLayout * m_layout;
    QColor m_offColor, m_onColor;
    QString m_offColorText, m_onColorText;
    QImage * m_offColorImage, * m_onColorImage;
    QLabel *m_offColorContainer, *m_onColorContainer;
    LabeledWidget * m_offColorLabel, *m_onColorLabel;


};

#endif // COLORBITWIDGET_H
