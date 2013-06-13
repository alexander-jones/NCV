#include "ncsproject.h"
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
}
QString NCSProject::parentDirectory()
{
}

void NCSProject::registerPlugin(NCSWidgetPlugin * plugin)
{
    QDomElement pluginRoot = m_document.documentElement().firstChildElement("plugins");
    QDomElement pluginElement = pluginRoot.firstChildElement(plugin->name());
    NCSProjectPortal portal(&pluginElement,m_parentDirectory);
    if (pluginElement.isNull())
    {
        pluginElement.setTagName(plugin->name());
        pluginElement.setAttribute("version",plugin->version());
    }
    else
    {
        if (pluginElement.attribute("version") != QString::number(plugin->version()))
        {
            pluginElement.clear();
            pluginElement.setTagName(plugin->name());
            pluginElement.setAttribute("version",plugin->version());
        }
    }


}

void NCSProject::save()
{
}

void NCSProject::saveAs(QString filename)
{
}

void NCSProject::setNCSInstallation(QSshSocket * socket,QString installationPath)
{
}

void NCSProject::setNCSInstallation(QString installationPath)
{
}

void NCSProject::m_connectionInvalidated(NCSInternalCommandBridge::ValidationError err)
{
}
void NCSProject::m_localConnectionValidated()
{
}
void NCSProject::m_remoteConnectionValidated()
{
}
void NCSProject::m_load(QFile * file)
{
    file->open( QIODevice::ReadOnly | QIODevice::Text );
    m_document.setContent(file);
    QDomElement root = m_document.documentElement();
    root.firstChildElement("installation");
    file->close();
}

void NCSProject::m_save(QFile  * file)
{
    QDomElement root = m_document.documentElement();
    root.setTagName("project");
    root.setAttribute("version", 1.0);
    QDomElement ncsInstallation;

    if (m_commandBridge != NULL)
    {
        if (m_remoteCommandBridge == m_commandBridge)
        {
        }
    }

    file->open( QIODevice::WriteOnly | QIODevice::Text );
    QTextStream stream( file );
    stream << m_document.toString();
    file->close();
}

