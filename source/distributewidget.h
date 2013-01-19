#ifndef DISTRIBUTEWIDGET_H
#define DISTRIBUTEWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QwwLoginBox>

class DistributeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DistributeWidget(QWidget *parent = 0);
    
signals:
    
public slots:

private:
    QVBoxLayout * m_layout;
    QwwLoginBox * m_loginWidget;
};

#endif // DISTRIBUTEWIDGET_H
