#include "ncslocalcommandbridge.h"
#include <QMessageBox>
#include <QStack>

NCSLocalCommandBridge::NCSLocalCommandBridge(QObject *parent):NCSInternalCommandBridge(parent)
{
    m_rootPath = "";
    m_valid = false;
    m_applicationBridge = NULL;
}

void NCSLocalCommandBridge::validate(QString path)
{
    m_rootPath = path;
    m_buildPath = m_rootPath + "/build";
    m_valid = true;

    // attempt to find mpi
    QProcess mpi;
    mpi.start("mpirun");
    if (!mpi.waitForStarted())
        m_invalidate(NCSInternalCommandBridge::MissingMPI);
    mpi.close();

    // make sure directory supplied exists
    QDir ncsDirectory(m_rootPath);
    if (!ncsDirectory.exists())
        m_invalidate(NCSInternalCommandBridge::MissingRootDirectory);
    else
    {
        // make sure ncs has build directory and application directory
        if (!QDir(m_buildPath).exists())
            m_invalidate(NCSInternalCommandBridge::MissingBuildDirectory);
        else if (!QDir(m_buildPath + "/applications").exists())
            m_invalidate(NCSInternalCommandBridge::MissingApplicationDirectory);
        else if (!QDir(m_buildPath + "/plugins").exists())
            m_invalidate(NCSInternalCommandBridge::MissingPluginDirectory);

    }
    if (m_valid == true)
        validated();
}

bool NCSLocalCommandBridge::valid()
{
    return m_valid;
}


void NCSLocalCommandBridge::launchApplicationBridge(QString application, NCSCommandArguments arguments)
{
    if (!m_valid)
        return;

    m_applicationBridge = new NCSLocalApplicationBridge(application,m_buildPath,this);
    applicationBridgeLaunched(m_applicationBridge);
    m_applicationBridge->start("applications/" + application  + "/" + application,arguments);
}


void NCSLocalCommandBridge::launchApplicationBridge(QString application, NCSCommandArguments arguments,int numProcesses, QString hostFile )
{

    if (!m_valid)
        return ;

    arguments.insert(0,"--np");
    arguments.insert(1,QString::number(numProcesses));
    arguments.insert(2,"--hostfile");
    arguments.insert(3,hostFile);
    arguments.insert(4,"applications/" + application  + "/" + application);

    m_applicationBridge = new NCSLocalApplicationBridge(application,m_buildPath,this);
    applicationBridgeLaunched(m_applicationBridge);
    m_applicationBridge->start("mpirun",arguments);
}

QString NCSLocalCommandBridge::hostname()
{
    return "localhost";
}

void NCSLocalCommandBridge::queryApplication(QString applicationName)
{
    if ( QFile(m_rootPath + "/build/applications/" + applicationName + "/" +applicationName).exists() )
        applicationQueried(applicationName,Ready);

    else if ( QFile(m_rootPath + "/applications/" + applicationName + "/" +applicationName).exists() )
        applicationQueried(applicationName,Unbuilt);

    else
        applicationQueried(applicationName,Missing);

}


void NCSLocalCommandBridge::queryPlugin(NCSCommandBridge::PluginType type,QString pluginName)
{
    QString pluginSubDir;
    if (type == NeuronPlugin)
        pluginSubDir = "neurons";
    else if (type == SynapsePlugin)
        pluginSubDir = "synapses";
    else
        pluginSubDir = "inputs";

    if ( m_fileExists(m_rootPath + "/build/plugins/" +  pluginSubDir+ "/" + pluginName, pluginName + "*.o") )
        pluginQueried(pluginName,Ready);

    else if (m_fileExists(m_rootPath + "/plugins/" +  pluginSubDir+ "/" + pluginName, pluginName + "*.o"))
        pluginQueried(pluginName,Unbuilt);

    else
        pluginQueried(pluginName,Missing);
}

void NCSLocalCommandBridge::queryReader(QString readerName)
{

    if ( m_fileExists(m_rootPath + "/build/readers/" +  readerName+ "/" + readerName, readerName + "*.o") )
        readerQueried(readerName,Ready);

    else if ( m_fileExists(m_rootPath + "/readers/" +  readerName+ "/" + readerName, readerName + "*.o") )
        readerQueried(readerName,Unbuilt);

    else
        readerQueried(readerName,Missing);
}

void NCSLocalCommandBridge::m_invalidate(NCSInternalCommandBridge::ValidationError err)
{
    m_valid = false;
    validationError(err);
}

bool NCSLocalCommandBridge::m_fileExists(QDir rootDir, QString pattern)
{
   QStack<QString> stack;
   stack.push(rootDir.absolutePath());
   while (!stack.isEmpty()) {
      QString sSubdir = stack.pop();
      QDir subdir(sSubdir);

      // Check for the files.
      QStringList entries = subdir.entryList(QStringList() << pattern, QDir::Files);
      if (entries.size() > 0)
          return true;

      QFileInfoList infoEntries = subdir.entryInfoList(QStringList(), QDir::AllDirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
      for (int i = 0; i < infoEntries.size(); i++) {
         QFileInfo& item = infoEntries[i];
         stack.push(item.absoluteFilePath());
      }
   }
   return false;
}
