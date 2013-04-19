#include "qreportclient.h"
#include <iostream>


/*
  *********************************************************
  ****************** Nested Report class ******************
  *********************************************************
  */
QReportClient::Report::Report(QSocketConnection *connection)
    : m_connection(connection),
      m_valid(true)
{
    unsigned int numIndices = 0;

    if(!m_connection->recv(numIndices))
    {
        m_valid = false;
        return;
    }

    m_permutation.resize(numIndices);
    if(!m_connection->recv(m_permutation.data(), numIndices))
    {
        m_valid = false;
        return;
    }
}



bool QReportClient::Report::pull(std::vector<char>& data, unsigned int &step)
{
    unsigned int size = 0;

    if(!m_connection->recv(step))
        return false;

    if(!m_connection->recv(size))
        return false;

    data.resize(size);
    if(!m_connection->recv(data.data(), size))
        return false;

    return true;
}



bool QReportClient::Report::valid() const
{
    return m_valid;
}



const std::vector<unsigned int>& QReportClient::Report::permutation() const
{
    return m_permutation;
}



/*
  *********************************************************
  ********************* QReportClient *********************
  *********************************************************
  */
QReportClient::QReportClient()
    : m_connection(0)
{
}



bool QReportClient::connect(const std::string &server, unsigned int port, float timeout)
{
    QMutexLocker lock(&m_mutex);
    m_connection = new QSocketConnection(server, port, timeout);
    return m_connection->connected();
}



bool QReportClient::connected() const
{
    QMutexLocker lock(&m_mutex);
    return m_connection != 0;
}



bool QReportClient::disconnect()
{
    QMutexLocker lock(&m_mutex);
    ClientMessage message;

    message.type = ClientMessage::Terminate;
    m_connection->send(&message, sizeof(ClientMessage));
    delete m_connection;
    m_connection = 0;

    return true;
}



QReportClient::Report* QReportClient::report(const ReportRequest &request)
{
    QMutexLocker lock(&m_mutex);
    ClientMessage message;

    message.type = ClientMessage::Report;
    m_connection->send(&message, sizeof(ClientMessage));
    QSocketConnection *reportMessages = m_connection->clone(false);

    if(!reportMessages->send(request.key))
    {
        std::cerr << "Failed to send key" << std::endl;
        delete reportMessages;
        return 0;
    }

    if(!reportMessages->send(request.elementType))
    {
        std::cerr << "Failed to send elementType" << std::endl;
        delete reportMessages;
        return 0;
    }

    if (!reportMessages->send(request.datatype) || !reportMessages->send(request.dataspace) || !reportMessages->send(request.frequency))
    {
        std::cerr << "Failed to setup report" << std::endl;
        delete reportMessages;
        return 0;
    }

    unsigned int numIndices = request.indices.size();
    if (!reportMessages->send(numIndices) || !reportMessages->send(request.indices.data(), numIndices))
    {
        delete reportMessages;
        return 0;
    }

    const unsigned int good = 0;
    const unsigned int bad = 1;
    unsigned int state = good;
    if (!reportMessages->recv(state))
    {
        delete reportMessages;
        return 0;
    }

    if (bad == state)
    {
        std::string message;
        if (reportMessages->recv(message))
        {
            std::cerr << message << std::endl;
        }
        else
        {
            std::cerr << "Failed to retrieve error message" << std::endl;
        }
        delete reportMessages;
        return 0;
    }

    Report* report = new Report(reportMessages);
    if (report->valid())
        return report;

    delete report;
    return 0;
}
