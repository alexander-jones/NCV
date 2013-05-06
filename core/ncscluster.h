#ifndef NCSCLUSTER_H
#define NCSCLUSTER_H
#include <QString>
#include <QVector>

struct NCSDevice
{
    QString type;
    float power;
    bool enabled;
    int number;
    bool operator==(const NCSDevice &rhs);
};

struct NCSMachine
{
    QString name;
    QVector<NCSDevice> devices;
    bool enabled, hasCPU,hasCUDA;
    int listIndex;

    bool operator==(const NCSMachine &rhs);
};

struct NCSCluster
{
    void read(QString filename,bool append = false);
    void write(QString filename);
    void writeHostfile(QString filename);
    QVector<NCSMachine> machines;
};


#endif // NCSCLUSTER_H
