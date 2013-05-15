#ifndef NCSAPPLICATIONWIDGETPLUGIN_H
#define NCSAPPLICATIONWIDGETPLUGIN_H
#include "core/gui/plugin-interfaces/ncswidgetplugin.h"
#include "core/networking/command/ncscommandbridge.h"

class NCSApplicationWidgetPlugin: public NCSWidgetPlugin
{
    Q_OBJECT
public:
    explicit NCSApplicationWidgetPlugin( QString projectDir,QWidget *parent = 0):NCSWidgetPlugin(projectDir,parent){}
public slots:
    virtual void setCommandBridge(NCSCommandBridge * bridge) = 0;
};

#endif // NCSAPPLICATIONWIDGETPLUGIN_H
