#ifndef NCSPROJECT_H
#define NCSPROJECT_H
#include <QVector>
#include "ncsprojectportal.h"
#include "core/plugin-interfaces/ncswidgetplugin.h"
#include "core/command/ncslocalcommandbridge.h"
#include "core/command/ncsremotecommandbridge.h"

class NCSProject : public QObject
{
    Q_OBJECT
public:
    explicit NCSProject(const QString & filepath,QObject *parent = 0);
    QString filepath();
    QString parentDirectory();
    void registerPlugin(NCSWidgetPlugin * plugin);
    void save();
    QString name();
    void saveAs(QString filename);

private:
    void m_new();
    void m_load(QFile * file);
    void m_save(QFile * file);

    QString m_filepath, m_parentDirectory, m_name;
    QDomDocument m_document;
    NCSProject * m_project;
    QMap<NCSWidgetPlugin * ,QDomElement> m_pluginElements;
    QDomElement m_rootElement, m_ncsElement,m_rootPluginElement;
};

#endif // NCSPROJECT_H
