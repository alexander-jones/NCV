#ifndef NCVDISTRIBUTIONWIDGETPLUGIN_H
#define NCVDISTRIBUTIONWIDGETPLUGIN_H
#include "ncsapplicationwidgetplugin.h"

class NCSDistributionWidgetPlugin: public NCSApplicationWidgetPlugin
{
    Q_OBJECT
public:
    explicit NCSDistributionWidgetPlugin( QString projectDir,QWidget *parent = 0):NCSApplicationWidgetPlugin(projectDir,parent){}

signals:
    void distributed(QString topologyFile);
    void launched();
};

#endif // NCVDISTRIBUTIONWIDGETPLUGIN_H
