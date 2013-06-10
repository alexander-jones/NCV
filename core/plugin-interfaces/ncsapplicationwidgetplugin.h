#ifndef NCSAPPLICATIONWIDGETPLUGIN_H
#define NCSAPPLICATIONWIDGETPLUGIN_H
#include "ncswidgetplugin.h"
#include "core/ncscommandbridge.h"

class NCSApplicationWidgetPlugin: public NCSWidgetPlugin
{
    Q_OBJECT
public:
    explicit NCSApplicationWidgetPlugin( QWidget *parent = 0):NCSWidgetPlugin(parent){}
public slots:
    virtual void setCommandBridge(NCSCommandBridge * bridge) = 0;
};

#endif // NCSAPPLICATIONWIDGETPLUGIN_H
