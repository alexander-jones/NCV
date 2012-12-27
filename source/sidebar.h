#ifndef SIDEBAR_H
#define SIDEBAR_H


#include <QWidget>
#include <QTreeWidget>
#include <QLabel>
#include <QDirModel>
#include <QCheckBox>
#include <QSignalMapper>
#include <QSpinBox>
#include <QToolBar>
#include <QComboBox>
#include <QGridLayout>
#include <QScrollArea>

class LabeledWidget:public QWidget
{
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

class Sidebar : public QWidget
{
    Q_OBJECT
public:
    explicit Sidebar(QWidget *parent = 0);
    void addTool(QWidget * tool);
    void addPanel(QWidget * panel,const QString& name);
    void removePanel(QWidget * panel);
    void removePanel(QString name);
    int numPanels();
    bool containsPanel(QString name);
    QString currentPanel();
    void setScrollBarPolicy(Qt::ScrollBarPolicy horizontal,Qt::ScrollBarPolicy vertical);
    void setVoidPanel(QWidget * panel);
    
signals:
    void panelChanged(QString name);
public slots:
    void setPanel(const QString& name);

private:
    QScrollArea * m_scrollArea;
    QToolBar * m_toolbar;
    QComboBox * m_panelSelector;
    QHBoxLayout * m_toolbarLayout;
    QVBoxLayout * m_layout;
    QWidget * m_voidPanel;
    QFrame *m_separator;
    QMap<QString, QWidget *> m_panels;
    bool m_firstPanelAdded;
};

#endif // SIDEBAR_H
