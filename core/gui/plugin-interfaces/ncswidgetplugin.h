#ifndef NCSWIDGETPLUGIN_H
#define NCSWIDGETPLUGIN_H

#include <QWidget>

class NCSWidgetPlugin: public QWidget
{
    Q_OBJECT
public:
    explicit NCSWidgetPlugin( QString projectDir,QWidget *parent = 0):QWidget(parent){}
    virtual QIcon icon() = 0;
    virtual QString title() = 0;

public slots:
    virtual void initialize() = 0;
    virtual void cleanup() = 0;
};


#endif // NCSWIDGETPLUGIN_H
