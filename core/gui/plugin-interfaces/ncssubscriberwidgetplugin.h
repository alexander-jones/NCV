#ifndef NCSSUBSCRIBERWIDGETPLUGIN_H
#define NCSSUBSCRIBERWIDGETPLUGIN_H

#include "ncswidgetplugin.h"
#include "core/ncsneuronset.h"
#include "core/ncsconnectionset.h"

class NCSSubscriberWidgetPlugin: public NCSWidgetPlugin
{
    Q_OBJECT
public:
    explicit NCSSubscriberWidgetPlugin( QWidget *parent = 0):NCSWidgetPlugin(parent){}
    virtual void setNeurons(NCSNeuronSet * neurons) = 0;
    virtual void setConnections(NCSConnectionSet * connections) = 0;
};

#endif //NCSSUBSCRIBERWIDGETPLUGIN_H
