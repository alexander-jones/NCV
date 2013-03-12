#include <QtGui>
#include <QwwLoginBox>
#include <QtGui>
#include <connectionwidget.h>

class NCSClusterEditor : public QWidget
{
    Q_OBJECT

public:
    NCSClusterEditor();
    void saveHostFile( QString filename);
    void saveClusterFile(QString filename);
    void loadClusterFile(QString filename);
    int enabledMachines();
    void clear();

private slots:
	void machineSelected(QListWidgetItem* current, QListWidgetItem* previous);
	void machineEnabled(QListWidgetItem* item);
	void deviceSelected(QListWidgetItem* current, QListWidgetItem* previous);
	void deviceEnabled(QListWidgetItem* item);

private:
    struct Device
    {
        QString type;
        float power;
        bool enabled;
        int number;

    };

    struct Machine
    {
        QString name;
        QVector<Device> devices;
        bool enabled;
    };

    struct Cluster
    {
        QVector<Machine> machines;
    };



	typedef std::pair<QLabel*, QLabel*> LabelSet;
    LabelSet m_infoParam(const QString & key, const QString & value);

    QVBoxLayout* m_layout;
    QPushButton * m_detectDevicesButton;
    QHBoxLayout* m_mainLayout;
    QWidget * m_machineWidgdet;

    QListWidget* m_machineList;
    QGroupBox* m_machineBox;

    QListWidget* m_deviceList;
    QGroupBox* m_deviceListBox;
    QGroupBox* m_deviceBox;
    Cluster m_cluster;
    Machine m_machineSelection;
};

