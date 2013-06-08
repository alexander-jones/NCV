#include "ncscluster.h"
#include <QFile>
#include <QDataStream>

bool NCSDevice::operator==(const NCSDevice &rhs)
{
    return (type == rhs.type) && (power == rhs.power) && (number = rhs.number);
}

bool NCSMachine::operator==(const NCSMachine &rhs)
{
    bool allDevicesSame = rhs.devices.count() == devices.count();
    if (allDevicesSame)
        for (int i = 0; i < devices.count(); i ++)
            if (rhs.devices.count(devices[i]) == 0)
            {
                allDevicesSame = false;
                break;
            }

    return  allDevicesSame && (name == rhs.name);
}


void NCSCluster::read(QString filename, bool append)
{

    if (!append)
        machines.clear();

    int numMachines,numDevices;

    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QDataStream dataStream(&file);
    dataStream.setByteOrder(QDataStream::LittleEndian);
    dataStream >> numMachines;

    for (int i = 0;i < numMachines; i ++)
    {
        NCSMachine machine;
        QByteArray utfString;
        dataStream >>  utfString;
        machine.name = QString::fromUtf8(utfString.data());
        dataStream >> machine.enabled;
        dataStream >> numDevices;
        machine.hasCPU = false;
        machine.hasCUDA = false;

        for (int j = 0; j < numDevices; j ++)
        {
            NCSDevice device;

            QByteArray typeUtfString;
            dataStream >> typeUtfString;
            device.type = QString::fromUtf8(typeUtfString.data());

            if (device.type == "CUDA")
            {
                dataStream >> device.number;
                machine.hasCUDA = true;
            }
            if (device.type == "CPU")
                machine.hasCPU = true;
            dataStream >> device.power;
            dataStream >> device.enabled;
            machine.devices.append(device);

        }

        if (machines.count(machine) == 0)
            machines.append(machine);
    }


    file.close();
}

void NCSCluster::write(QString filename)
{

    // open file to house binary distribution file
    QFile file(filename);
    file.open(QIODevice::WriteOnly);

    // setup data streaming object
    QDataStream dataStream(&file);
    dataStream.setByteOrder(QDataStream::LittleEndian);

    dataStream << machines.count();
    for (int i = 0; i < machines.count(); i ++)
    {
        // for each machine write name, machine, and type and number of devices
        NCSMachine machine = machines[i];
        QByteArray utfString = machine.name.toUtf8();
        dataStream << utfString;
        dataStream << machine.enabled;
        dataStream << machine.devices.count();


        for (int j = 0; j < machine.devices.count(); j ++)
        {
            // for each device write type
            NCSDevice device = machine.devices[j];

            QByteArray typeUtfString = device.type.toUtf8();
            dataStream <<  typeUtfString ;

            // for each device write number of devices
            if (device.type == "CUDA")
                dataStream << device.number;

            // for each device write power, whether it's enabled
            dataStream << device.power;
            dataStream << device.enabled;

        }
    }
    // close file
    file.close();
}
QString NCSCluster::reportHost()
{
    QString host;
    int i = 0;

    while (host == "" && i < machines.count())
    {
        for (int j = 0; j < machines[i].devices.count(); j ++)
            if (machines[i].devices[j].type == "CPU" && machines[i].devices[j].enabled)
            {
                host = machines[i].name;
                break;
            }
        i++;
    }
    return host;
}

void NCSCluster::writeHostfile(QString filename)
{
    // open  host file
    QFile file(filename);
    file.open(QIODevice::WriteOnly);

    for (int i = 0; i < machines.count(); i ++)
    {
        QString line = machines[i].name + " slots=1 \n";
        file.write(line.toAscii());
    }
    // close file
    file.close();
}
