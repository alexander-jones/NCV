#ifndef COLLAPSIBLEWIDGET_H
#define COLLAPSIBLEWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QToolBar>
#include <QToolButton>
#include <QLabel>

class CollapsibleWidget : public QFrame
{
    Q_OBJECT
public:
    explicit CollapsibleWidget(QWidget *widget = NULL, const QString& text = "",QWidget *parent = 0);
    ~CollapsibleWidget();
    void setHeader(const QString& text);
    void setWidget(QWidget *widget);
    void underlineHeader(bool on);
    void collapse();
    void expand();

    
signals:
    void collapsed();
    void expanded();
    
public slots:


private slots:
    void m_collapseToolPressed();

private:
    QVBoxLayout * m_layout;
    QLabel * m_header;
    QToolButton * m_collapseToolOne, * m_collapseToolTwo;
    QWidget * m_widget;
    bool m_collapsed;
    QFrame * m_headerUnderline;
    QWidget * m_headerBar;
    QHBoxLayout * m_headerBarLayout;

};

#endif // COLLAPSIBLEWIDGET_H
