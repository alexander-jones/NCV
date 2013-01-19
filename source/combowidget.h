#ifndef SIDEBAR_H
#define SIDEBAR_H


#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QSignalMapper>
#include <QSpinBox>
#include <QComboBox>
#include <QHBoxLayout>

class LabeledWidget:public QWidget
{
    Q_OBJECT
public:
    LabeledWidget(const QString & label, QWidget * widget, QWidget * parent =0)
        :QWidget(parent)
    {
        m_layout = new QHBoxLayout();
        m_layout->setAlignment(Qt::AlignCenter);
        m_label = new QLabel(label);
        m_layout->addWidget(m_label);
        m_widget = widget;
        m_layout->addWidget(widget);
        this->setLayout(m_layout);
    }
    ~LabeledWidget()
    {

    }

    QLabel * getLabel() {  return m_label;}
    QWidget * getWidget(){ return m_widget;}
private:
    QHBoxLayout * m_layout;
    QLabel * m_label;
    QWidget * m_widget;

};

class ComboWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ComboWidget(QWidget *parent = 0);
    void addTool(QWidget * tool);
    void removeTool(QWidget * tool);
    void addWidget(QWidget * panel,const QString& name);
    void removeWidget(QString name);
    int count();
    bool containsWidget(QString name);
    bool containsTool(QWidget * tool);
    QWidget * currentWidget();
    QWidget * voidWidget();
    QString currentWidgetName();
    void setVoidWidget(QWidget * panel);

signals:
    void widgetChanged(QString name);
public slots:
    void setWidget( QString name);

private:
    QVector<QWidget *> m_tools;
    QWidget * m_currentWidget, *m_toolbar;
    QComboBox * m_panelSelector;
    QHBoxLayout * m_toolbarLayout;
    QVBoxLayout * m_layout;
    QWidget * m_voidWidget;
    QFrame *m_separator;
    QMap<QString, QWidget *> m_panels;
};

#endif // SIDEBAR_H
