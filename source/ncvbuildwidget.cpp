#include "ncvbuildwidget.h"

NCVBuildWidget::NCVBuildWidget(QWidget *parent) :
    QWidget(parent)
{
}

void NCVBuildWidget::createNetwork(int numNeurons, int numConnections,QVector3D worldSize)
{
    // establish discrete attribute states;
    QVector<QString> firingStates;
    firingStates.append("Off");
    firingStates.append("On");

    QVector<QString> typeStates;
    typeStates.append("Ovoid");
    typeStates.append("Izekivich");
    typeStates.append("Pyramidal");
    typeStates.append("Granule");
    typeStates.append("Double Pyramidal");

    // establish attribute  colorations
    ContinuousColorSelector * voltageRange = new ContinuousColorSelector();
    voltageRange->setLowThreshold(-80);
    voltageRange->setHighThreshold(40);
    voltageRange->addMarker(-80 ,QColor(0,255,0));
    voltageRange->addMarker(-20,QColor(0,0,255));
    voltageRange->addMarker(40,QColor(255,0,0));


    QMap<QString,QColor >firingColors;
    firingColors["Off"] =QColor(0,255,0);
    firingColors["On"] = QColor(255,0,0);

    QMap<QString,QColor >typeColors;
    typeColors["Ovoid"] = QColor(125,125,0);
    typeColors["Izekivich"] = QColor(255,0,125);
    typeColors["Pyramidal"] = QColor(0,125,125);
    typeColors["Granule"] = QColor(75,255,125);
    typeColors["Double Pyramidal"] = QColor(255,255,0);

    // create stub neuron data source
    QVector<QVector3D> neuronPositions;
    QVector<GLfloat> neuronVoltages;
    QVector<GLubyte> neuronFirings;
    QVector<GLubyte> neuronTypes;


    int fireVal;
    float threshold = 25.0f;

    for (int i = 0; i <numNeurons; i ++)
    {
        int x = (abs(rand() * rand() * rand() )% (int)worldSize.x()) - (worldSize.x()/2);
        int y = (abs(rand() * rand() * rand() ) % (int)worldSize.y()) - (worldSize.y()/2);
        int z = (abs(rand() * rand() * rand() ) % (int)worldSize.z()) - (worldSize.z()/2);
        neuronPositions.append(QVector3D(x,y,z));
        neuronVoltages.append((float)(abs(rand() % 120)) -80.0);

        if (neuronVoltages[i] >= threshold)
            fireVal = 1;
        else
            fireVal = 0;

        if(i %8 == 0)
            neuronFirings.append(0);

        neuronFirings[i/8] += fireVal<<(i % 8);

        neuronTypes.append(rand() % 5);
    }


    // create neurons and related attributes
    m_neurons= new NCVNeuronSet(neuronPositions);

    m_continuousNeuronAttributes["voltage"] = new NCVContinuousAttribute(Neuron,voltageRange->lowThreshold(),voltageRange->highThreshold());
    m_continuousNeuronAttributes["voltage"]->attachData(neuronVoltages);
    m_continuousNeuronAttributes["voltage"]->attachColoration(voltageRange->getData());
    m_neurons->addAttribute("voltage",m_continuousNeuronAttributes["voltage"]);

    m_discreteNeuronAttributes["firing"] = new NCVDiscreteAttribute(Neuron,firingStates);
    m_discreteNeuronAttributes["firing"]->attachData(neuronFirings,1);
    m_discreteNeuronAttributes["firing"]->attachColoration(firingColors);
    m_neurons->addAttribute("firing",m_discreteNeuronAttributes["firing"]);


    m_discreteNeuronAttributes["type"] = new NCVDiscreteAttribute(Neuron,typeStates);
    m_discreteNeuronAttributes["type"]->attachData(neuronTypes);
    m_discreteNeuronAttributes["type"]->attachColoration(typeColors);
    m_neurons->addAttribute("type",m_discreteNeuronAttributes["type"]);

    neuronsFinalized(m_neurons);


    // create stub connection data
    QVector<NeuronConnection> connections;
    for (int i = 0; i < numConnections; i++)
    {
        connections.append(NeuronConnection(i % numNeurons,i % numNeurons));
        while (connections[i].outNeuron == connections[i].inNeuron)
            connections[i].outNeuron = (rand() % numNeurons) +1;
    }

    QVector<GLfloat> connectionVoltages;
    QVector<GLubyte> connectionFirings;

    for (int i = 0; i <numConnections; i ++)
    {
        GLfloat inVoltage = neuronVoltages[connections[i].inNeuron];
        GLfloat outVoltage = neuronVoltages[connections[i].outNeuron];

        connectionVoltages.append((outVoltage + inVoltage)/2);
        if (connectionVoltages[i] >= threshold)
            fireVal = 1;
        else
            fireVal = 0;

        if(i %8 == 0)
            connectionFirings.append(0);
        connectionFirings[i/8] += fireVal<<(i % 8);


    }

    //create connections and related attributes
    m_connections= new NCVConnectionSet(m_neurons,connections);

    m_continuousConnectionAttributes["voltage"] = new NCVContinuousAttribute(Connection,voltageRange->lowThreshold(),voltageRange->highThreshold());
    m_continuousConnectionAttributes["voltage"]->attachData(connectionVoltages);
    m_continuousConnectionAttributes["voltage"]->attachColoration(voltageRange->getData());
    m_connections->addAttribute("voltage",m_continuousConnectionAttributes["voltage"]);

    m_discreteConnectionAttributes["firing"] = new NCVDiscreteAttribute(Connection,firingStates);
    m_discreteConnectionAttributes["firing"]->attachData(connectionFirings,1);
    m_discreteConnectionAttributes["firing"]->attachColoration(firingColors);
    m_connections->addAttribute("firing",m_discreteConnectionAttributes["firing"]);

    connectionsFinalized(m_connections);

}
