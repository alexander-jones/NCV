#ifndef QREPORTCLIENT_H
#define QREPORTCLIENT_H

#include "qsocketconnection.h"
#include <vector>
#include <QMutex>



/**
  \class QReportClient
  \author Justin Cardoza
  \brief A rewrite of the NCS ReportClient class which is platform-independent. It uses
         only Qt and STL libraries and provides identical functionality, namely the ability
         to connect to an instance of NCS, create reports, and pull data from those reports.
  */
class QReportClient : public QObject
{
    Q_OBJECT

    public:
        struct ReportRequest
        {
            std::string key;
            std::string elementType;
            std::string datatype;
            std::string dataspace;
            unsigned int frequency;
            std::vector<unsigned int> indices;

            std::string name;

            //bool serialize(const std::string& path);
            //bool read(const std::string& path);
        };


        struct ClientMessage
        {
            enum MessageType
            {
                Terminate = 0,
                Report = 1
            };
            unsigned int type;
            union MessageData
            {
                struct TerminateMessage
                {
                };
                TerminateMessage terminate;
            };
            MessageData data;
        };


        class Report
        {
            public:
                Report(QSocketConnection *connection);
                bool pull(std::vector<char>& data, unsigned int& step);
                bool valid() const;
                const std::vector<unsigned int>& permutation() const;

                template<typename T>
                bool permute(const std::vector<char>& data, std::vector<T>& result);

            private:
                std::vector<unsigned int> m_permutation;
                QSocketConnection *m_connection;
                bool m_valid;
        };


        QReportClient();
        bool connectToSimulator(const std::string& server, unsigned int port, float timeout);
        bool disconnectFromSimulator();
        bool connected() const;
        Report* report(const ReportRequest& request);

    private:
        QSocketConnection *m_connection;
        mutable QMutex m_mutex;
};



///Rearranges a previously received data buffer to produce a vector of elements of the
///correct type in the order they were originally requested.
template<typename T>
bool QReportClient::Report::permute(const std::vector<char>& data, std::vector<T>& result)
{
    const T* src = (const T*)data.data();
    result.resize(m_permutation.size());

    for(unsigned int i = 0; i < m_permutation.size(); ++i)
    {
        result[i] = src[m_permutation[i]];
    }

    return true;
}


#endif
