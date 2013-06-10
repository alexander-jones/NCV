#include <QtGui>
#include "core/plugin-interfaces/ncsapplicationwidgetplugin.h"
#include "gui/utilities/qwidgetvector.h"
#include "core/ncscluster.h"

class NCSClusterEditor : public NCSApplicationWidgetPlugin
{
    Q_OBJECT

public:
    explicit NCSClusterEditor(QWidget * parent = 0);
    void saveHostFile( QString filename);
    void saveClusterFile(QString filename);
    void loadClusterFile(QString filename, bool append = false);
    int enabledMachines();
    void loadProject(QString projectDir);
    QIcon icon();
    QString title();


public slots:
    void setCommandBridge(NCSCommandBridge * bridge);
    void initialize();
    void cleanup();
    void clear();

private slots:
	void machineSelected(QListWidgetItem* current, QListWidgetItem* previous);
	void machineEnabled(QListWidgetItem* item);
	void deviceSelected(QListWidgetItem* current, QListWidgetItem* previous);
	void deviceEnabled(QListWidgetItem* item);
    void m_onlyCPUSelected(bool set);
    void m_onlyGPUSelected(bool set);
    void m_allCPUSelected(bool set);
    void m_allGPUSelected(bool set);
    void m_loadClusterFilePressed();
    void m_saveClusterFilePressed();
    void m_appendClusterFilePressed();

    void m_clusterSpecifierStarted(NCSApplicationBridge * app);
    void m_destroyClusterSpecifier();
    void m_clusterCompilationFailed(NCSApplicationBridge::ApplicationError err);
    void m_clusterCompilationFinished();
    void m_detectDevicesOverIPRange();
    void m_browseForHostFile();
    void m_hostFileEditChanged(QString newText);
    void m_hostfileDetectAndLoad();
    void m_hostfileDetectAndAppend();
private:

	typedef std::pair<QLabel*, QLabel*> LabelSet;
    LabelSet m_infoParam(const QString & key, const QString & value);
    void m_enableAll(QString type);
    void m_enableOnly(QString type);

    QString m_projectDir;
    QWidgetVector * m_optionVector, * m_clusterFileOptionVector;
    QCheckBox * m_onlyCPUCheckBox, * m_onlyGPUCheckBox, * m_allCPUCheckBox, * m_allGPUCheckBox;
    QVBoxLayout* m_layout;
    QPushButton * m_clearButton, * m_saveClusterFileButton, * m_loadClusterFileButton, * m_appendClusterFileButton;
    QHBoxLayout* m_mainLayout;
    QWidget * m_machineWidgdet;

    QListWidget* m_machineList;
    QGroupBox* m_machineBox;

    QListWidget* m_deviceList;
    QGroupBox* m_deviceListBox;
    QGroupBox* m_deviceBox;
    NCSCluster m_cluster;
    int m_machineSelectionIndex;

    QLineEdit * m_startIP, * m_endIP,*m_hostFileEdit;
    QLabel  *m_machinesLabel, * m_hostfileLabel;
    QWidgetVector * m_ipRangeDetectionVector, * m_hostfileDetectionVector;
    QPushButton * m_ipDetectDevicesButton,*m_hostfileDetectAndLoadButton,*m_hostfileDetectAndAppendButton,*m_hostfileBrowseButton;
    NCSCommandBridge * m_commandBridge;
    NCSApplicationBridge * m_currentApplication;
    bool m_appendDetectedHosts;
};

