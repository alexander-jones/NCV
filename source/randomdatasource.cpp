#include "randomdatasource.h"



RandomDataSource::RandomDataSource(int neuronCount, double pFire) : QObject(0)
{
    m_neuronCount = neuronCount;

    if((neuronCount & 7) == 0)
        m_dataSize = (neuronCount / 8);
    else
        m_dataSize = (neuronCount / 8) + 1;

    m_fireThreshold = (int)(pFire * RAND_MAX);
}



RandomDataSource::~RandomDataSource()
{
    //delete[] m_data;
}



void RandomDataSource::updateFirings()
{
    int i, index, bit, rv;
    GLubyte *data = new GLubyte[m_dataSize];

    for(i = 0; i < m_dataSize; i++)
    {
        data[i] = 0;
    }

    for(i = 0; i < m_neuronCount; i++)
    {
        rv = rand();
        if(rv < m_fireThreshold)
        {
            index = i >> 3;
            bit = i & 7;
            data[index] |= 1 << bit;
        }
    }

    //qDebug() << "Neuron firings updated.";
    emit neuronFiringsUpdated("firing", data);
}
