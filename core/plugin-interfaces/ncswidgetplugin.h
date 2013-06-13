#ifndef NCSWIDGETPLUGIN_H
#define NCSWIDGETPLUGIN_H

#include <QWidget>

class NCSWidgetPlugin: public QWidget
{
    Q_OBJECT
public:
    explicit NCSWidgetPlugin( QWidget *parent = 0):QWidget(parent){}
    virtual void loadProject(QString projectDir) = 0;
    virtual QIcon icon() = 0;
    virtual QString title() = 0;
    virtual QString name() = 0;
    virtual float version() = 0;

public slots:
    virtual void initialize() = 0;
    virtual void cleanup() = 0;
};


#endif // NCSWIDGETPLUGIN_H
