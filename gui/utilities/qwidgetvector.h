#ifndef QWIDGETVECTOR_H
#define QWIDGETVECTOR_H

#include <QWidget>
#include <QBoxLayout>
class QWidgetVector : public QWidget
{
    Q_OBJECT
public:
    enum Direction
    {
        TopToBottom = QBoxLayout::TopToBottom,
        BottomToTop = QBoxLayout::BottomToTop,
        LeftToRight = QBoxLayout::LeftToRight,
        RightToLeft = QBoxLayout::RightToLeft
    };

    explicit QWidgetVector(QWidget *parent = 0);
    void addWidget(QWidget *widget);
    bool containsWidget(QWidget *widget);
    void insertWidget(int index,QWidget *widget);
    int indexOfWidget(QWidget *widget);
    void removeWidget(QWidget *widget);
    void setDirection(Direction direction);
    void setAlignment(Qt::Alignment alignment);
    
signals:
    
public slots:
    
private:
    QBoxLayout * m_layout;
    Qt::Orientation m_orientation;
    QVector<QWidget *> m_widgets;
};

#endif // QWIDGETVECTOR_H
