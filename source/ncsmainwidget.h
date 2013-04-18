#ifndef NCSMAINWIDGET_H
#define NCSMAINWIDGET_H
#include "qwidgetvector.h"
#include "ncsprojectwidget.h"
#include "ncsclustereditor.h"
#include "ncsmodeldistributionwidget.h"
#include "ncvwidget.h"
#include "ncvbuildwidget.h"

#include <QListWidget>

class NCSPluginButton:public QListWidgetItem
{
    Q_OBJECT
public:
    explicit NCSPluginButton(QWidget *parent = 0)
    {
    }

private:

};

class NCSMainWidget : public QWidgetVector
{
    Q_OBJECT
public:
    explicit NCSMainWidget(QWidget *parent = 0);
    void addPlugin(NCSWidgetPlugin * widget);
    void addPlugin(NCSApplicationWidgetPlugin * widget);
    void addPlugin(NCSDistributionWidgetPlugin* widget);

    
signals:
    
public slots:

private:
    QListWidget * m_sidebar;
    QMap<QListWidgetItem *,NCSWidgetPlugin *> m_widgets;

    
};

#endif // NCSMAINWIDGET_H
