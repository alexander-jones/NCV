#ifndef NCVDISTRIBUTIONWIDGETPLUGIN_H
#define NCVDISTRIBUTIONWIDGETPLUGIN_H
#include "ncsapplicationwidgetplugin.h"

class NCSDistributionWidgetPlugin: public NCSApplicationWidgetPlugin
{
    Q_OBJECT
public:
    explicit NCSDistributionWidgetPlugin( QWidget *parent = 0):NCSApplicationWidgetPlugin(parent){}

signals:
    void launchTriggered();
    void distributed(QString topologyFile);
    void launched();
    void launchFailed();
};

#endif // NCVDISTRIBUTIONWIDGETPLUGIN_H
