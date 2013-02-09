#ifndef NCVDATALOGGER_H
#define NCVDATALOGGER_H
#include <QString>
#include <QMap>

enum ElementType
{
    Neuron,
    Connection
};
enum DataType
{
    Range,
    Flag,
    State
};


struct NCVDataLog
{

    NCVDataLog(){}
    QVector<QByteArray> entries;
    DataType dataType;
    ElementType elementType;
};

class NCVDataLogger
{
public:

    NCVDataLogger();
    int addEntry(QString log, void * data, float timeStamp);
    void addLog(QString name,ElementType type, DataType type);
    void dumpLog(QString log, QString filename);
    void loadLog(QString name, QString filename);
    void removeEntry(int entry);
    void removeLog(QString name);
private:
    QMap<QString, NCVDataLog> m_logs;
};

#endif // NCVDATALOGGER_H
