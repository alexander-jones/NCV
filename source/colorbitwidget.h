#ifndef COLORBITWIDGET_H
#define COLORBITWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QColorDialog>
#include <QEvent>

class ColorBitWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ColorBitWidget(QWidget *parent = 0);
    void setOffColor(QColor color);
    void setOnColor(QColor color);
    
signals:
    void colorsChanged(QColor offColor, QColor onColor);
public slots:
    
private:
    bool eventFilter( QObject* watched, QEvent* event ) ;
    QGridLayout * m_layout;
    QImage * m_offColor, * m_onColor;
    QLabel *m_offColorContainer, *m_onColorContainer, * m_offColorLabel, *m_onColorLabel;


};

#endif // COLORBITWIDGET_H
