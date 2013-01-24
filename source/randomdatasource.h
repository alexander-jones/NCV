#ifndef RANDOMDATASOURCE_H
#define RANDOMDATASOURCE_H

#include <QObject>
#include "qglxsystem.h"


/*!
  \class RandomDataSource
  \author Justin Cardoza
  \brief A simple example class for pushing random firing data to the visualization.
  */
class RandomDataSource : public QObject
{
    Q_OBJECT
public:
    explicit RandomDataSource(int neuronCount, double pFire);
    virtual ~RandomDataSource();
    
signals:
    void neuronFiringsUpdated(QString attribName, GLubyte *data);

public slots:
    void updateFirings();

private:
    //GLubyte *m_data;
    int m_dataSize;
    int m_fireThreshold;
    int m_neuronCount;
};

#endif // RANDOMDATASOURCE_H
