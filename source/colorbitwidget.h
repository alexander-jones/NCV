#ifndef COLORBITWIDGET_H
#define COLORBITWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QColorDialog>
#include <QEvent>
#include "sidebar.h"

class ColorBitWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ColorBitWidget(QWidget *parent = 0);
    QColor getOnColor();
    QColor getOffColor();
    
signals:
    void colorsChanged(QColor offColor, QColor onColor);

public slots:
    void setColors(QColor offColor, QColor onColor);
    void setOffColor(QColor color);
    void setOnColor(QColor color);
    
private:
    bool eventFilter( QObject* watched, QEvent* event ) ;
    QHBoxLayout * m_layout;
    QImage * m_offColor, * m_onColor;
    QLabel *m_offColorContainer, *m_onColorContainer;
    LabeledWidget * m_offColorLabel, *m_onColorLabel;


};

#endif // COLORBITWIDGET_H
