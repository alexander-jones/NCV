#ifndef DISTRIBUTEWIDGET_H
#define DISTRIBUTEWIDGET_H

#include <QWidget>
#include "ncsclustereditor.h"
#include "slidingstackedwidget.h"
#include "colorbutton.h"

class DistributeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DistributeWidget(QWidget *parent = 0);
    
signals:
    
public slots:
    void m_mainWidgetChanged(int index);
    void m_finalize();

private:
    NCSClusterEditor * m_clusterEditor;
    QwwLoginBox * m_loginWidget;
    SlidingStackedWidget * m_widgetStack;
    QVBoxLayout * m_layout;
    QHBoxLayout* m_buttonLayout;
    QWidget * m_buttonWidget;
    ColorButton * m_previousButton, * m_nextButton;
    QColor m_activeColor, m_inactiveColor;
    
};

#endif // DISTRIBUTEWIDGET_H
