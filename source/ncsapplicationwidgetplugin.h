#ifndef NCSAPPLICATIONWIDGETPLUGIN_H
#define NCSAPPLICATIONWIDGETPLUGIN_H
#include "ncswidgetplugin.h"
#include "ncscommandbridge.h"

class NCSApplicationWidgetPlugin: public NCSWidgetPlugin
{
public:
    explicit NCSApplicationWidgetPlugin( QString projectDir,QWidget *parent = 0):NCSWidgetPlugin(projectDir,parent){}
public slots:
    virtual void initialize(NCSCommandBridge * bridge) = 0;
};

#endif // NCSAPPLICATIONWIDGETPLUGIN_H
