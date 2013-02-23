#ifndef SIDEBAR_H
#define SIDEBAR_H


#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QSignalMapper>
#include <QSpinBox>
#include <QComboBox>
#include <QHBoxLayout>

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
