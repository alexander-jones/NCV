#include "randomdatasource.h"



RandomDataSource::RandomDataSource(int neuronCount, double pFire) : QObject(0)
{
    m_neuronCount = neuronCount;

    if((neuronCount & 31) == 0)
        m_dataSize = (neuronCount / 32);
    else
        m_dataSize = (neuronCount / 32) + 1;

    m_fireThreshold = (int)(pFire * RAND_MAX);
    m_index = 0;
}



RandomDataSource::~RandomDataSource()
{
    //delete[] m_data;
}



void RandomDataSource::updateFirings()
{
    int i, index, bit, rv;
    GLuint *data = new GLuint[m_dataSize];

    for(i = 0; i < m_dataSize; i++)
    {
        data[i] = 0;
    }

    for(i = 0; i < m_neuronCount; i++)
    {
        rv = rand();
        if(rv < m_fireThreshold)
        {
            index = i >> 5;
            bit = i & 31;
            data[index] |= 1 << bit;
        }
    }

    /*index = m_index >> 5;
    bit = m_index & 31;
    data[index] |= 1 << bit;

    m_index++;
    if(m_index >= m_neuronCount)
        m_index = 0;*/

    //qDebug() << "Neuron firings updated.";
    emit neuronFiringsUpdated("firing", data);
}
