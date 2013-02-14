#include "ncsdatasource.h"
#include <slug/sim/ReportClient.h>


NCSDataSource::NCSDataSource(const std::string& host, int port) : QObject(0)
{
    if(!client.connect(host, port, 3))
        throw std::string("Error connecting to server.");

    slug::util::ReportRequest req;
    req.key = "neuronVoltage";
    req.elementType = "neuron";
    req.datatype = "float";
    req.dataspace = "device";
    req.frequency = 1;
    req.indices.push_back(1);
    req.indices.push_back(0);
    report = client.report(req);
    if(NULL == report)
    {
        client.disconnect();
        throw std::string("Error setting up report.");
    }

    step = 0;
}



NCSDataSource::~NCSDataSource()
{
    client.disconnect();
}



void NCSDataSource::updateFirings()
{
    //TODO: implement firing data.
}



void NCSDataSource::updateVoltages()
{
    if(!report->pull(data, step))
        return;
    if(!report->permute(data, voltages))
        return;

    emit neuronVoltagesUpdated("voltage", voltages.data());
}
