#ifndef NCSWIDGETPLUGIN_H
#define NCSWIDGETPLUGIN_H

#include <QWidget>

class NCSWidgetPlugin: public QWidget
{
public:
    explicit NCSWidgetPlugin( QString projectDir,QWidget *parent = 0):QWidget(parent){}
    virtual QIcon icon() = 0;
    virtual QString title() = 0;
};


#endif // NCSWIDGETPLUGIN_H
