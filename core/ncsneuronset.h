#ifndef NCSNEURONSET_H
#define NCSNEURONSET_H

#include <QObject>
#include <QMap>
#include <QVector>
#include <QVector3D>
#include "ncsattribute.h"

class NCSNeuronSet : public QObject
{
    Q_OBJECT
public:
    NCSNeuronSet(QVector<QVector3D> positions,QObject *parent  = 0);
    QMap<QString,NCSAttribute *> attributes();
    QVector<QVector3D> positions();
    int count();

public slots:
    void addAttribute(QString name, NCSAttribute * attribute);
    void removeAttribute(QString name);

signals:
    void attributeAdded(QString name, NCSAttribute * attribute );
    void attributeRemoved(QString name);

private:
    int m_count;
    QMap<QString,NCSAttribute *> m_attributes;
    QVector<QVector3D> m_positions;
};

#endif // NCSNEURONSET_H
