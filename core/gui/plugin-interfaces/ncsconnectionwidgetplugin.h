#ifndef NCSCONNECTIONWIDGETPLUGIN_H
#define NCSCONNECTIONWIDGETPLUGIN_H
#include "ncswidgetplugin.h"
#include "core/networking/command/ncscommandbridge.h"

class NCSConnectionWidgetPlugin: public NCSWidgetPlugin
{
    Q_OBJECT
public:
    explicit NCSConnectionWidgetPlugin( QString projectDir,QWidget *parent = 0):NCSWidgetPlugin(projectDir, parent){}

signals:
    void bridgeEstablished(NCSCommandBridge * bridge);
};
#endif // NCSCONNECTIONWIDGETPLUGIN_H
