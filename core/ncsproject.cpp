#include "ncsproject.h"
#include "core/utilities/simplecrypt.h"
#include <QFile>

NCSProject::NCSProject(const QString & filepath,QObject *parent ):
    QObject(parent)
{
    m_filepath = filepath;
    m_parentDirectory = m_filepath;
    QStringList fileSegments = filepath.split("/");
    m_parentDirectory.remove(fileSegments.last());
    QFile file(filepath);
    if (file.exists())
        m_load(&file);
    else
        m_new();

}
QString NCSProject::parentDirectory()
{
    return m_parentDirectory;
}

void NCSProject::registerPlugin(NCSWidgetPlugin * plugin)
{
    QDomElement pluginElement = m_rootPluginElement.firstChildElement(plugin->name());

    if (pluginElement.isNull())
    {
        pluginElement = m_document.createElement(plugin->name());
        pluginElement.setAttribute("version",plugin->version());
        m_rootPluginElement.appendChild(pluginElement);
    }
    plugin->loadProject(NCSProjectPortal(pluginElement,m_document,m_parentDirectory));

}

void NCSProject::save()
{
    QFile file(m_filepath);
    m_save(&file);
}

void NCSProject::saveAs(QString filename)
{
    QFile file(filename);
    m_save(&file);
}
QString NCSProject::filepath()
{
    return m_filepath;
}


void NCSProject::m_new()
{
    m_rootElement = m_document.createElement("ncv-project");
    m_rootElement.setAttribute("version", 1.0);
    m_document.appendChild(m_rootElement);
    m_rootPluginElement = m_document.createElement("plugins");
    m_rootElement.appendChild(m_rootPluginElement);
}

void NCSProject::m_load(QFile * file)
{
    file->open( QIODevice::ReadOnly | QIODevice::Text );
    m_document.setContent(file);
    m_rootElement = m_document.documentElement();
    m_rootPluginElement = m_rootElement.firstChildElement("plugins");
    file->close();
}

void NCSProject::m_save(QFile  * file)
{
    file->open( QIODevice::WriteOnly | QIODevice::Text );
    QTextStream stream( file );
    stream << m_document.toString();
    file->close();
}

