#include <QtGui>
#include <QwwLoginBox>
#include <QtGui>
#include <iostream>
#include <slug/spec/Cluster.h>
#include <slug/spec/Cluster.h>

class NCSClusterEditor : public QWidget
{
    Q_OBJECT

public:
    NCSClusterEditor();

private slots:
	void open();
	void save();
	void hostfile();
	void machineSelected(QListWidgetItem* current, QListWidgetItem* previous);
	void machineEnabled(QListWidgetItem* item);
	void deviceSelected(QListWidgetItem* current, QListWidgetItem* previous);
	void deviceEnabled(QListWidgetItem* item);

private:
	typedef std::pair<QLabel*, QLabel*> LabelSet;
	LabelSet infoParam(const std::string& key, const std::string& value);

    void m_updateCluster(slug::spec::Cluster* cluster);
    QAction* m_openAction;
    QAction* m_saveAction;
    QAction* m_exitAction;
    QAction* m_hostfileAction;

    QwwLoginBox * m_loginWidget;

    QMenu* m_fileMenu;

    QVBoxLayout* m_layout;
    QHBoxLayout* m_mainLayout;
    QToolBar * m_toolbar;
    QWidget * m_machineWidgdet;

    QListWidget* m_machineList;
    QGroupBox* m_machineBox;
    slug::spec::Machine* m_machine;

    QListWidget* m_deviceList;
    QGroupBox* m_deviceListBox;
    QGroupBox* m_deviceBox;
    slug::spec::Device* m_device;

    slug::spec::Cluster* m_cluster;
};

