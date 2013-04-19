#include "qsocketconnection.h"
#include <QMutexLocker>
#include <QTcpServer>



QSocketConnection::QSocketConnection(QTcpSocket *socket)
    : m_socket(socket)
{
}



QSocketConnection::QSocketConnection(const std::string &server, int port, float timeout)
{
    m_socket = new QTcpSocket(this);
    m_socket->connectToHost(QString::fromStdString(server), port);
    m_socket->waitForConnected(int(timeout * 1000));
}



QSocketConnection::QSocketConnection(const QHostAddress &addr, int port, float timeout)
{
    m_socket = new QTcpSocket(this);
    m_socket->connectToHost(addr, port);
    m_socket->waitForConnected(int(timeout * 1000));
}



QSocketConnection::~QSocketConnection()
{
    disconnect();
}



bool QSocketConnection::send(const char *data)
{
    return send(std::string(data));
}



template<>
bool QSocketConnection::send(const std::string& data)
{
    unsigned int length = data.length();

    if(send<unsigned int>(length))
    {
        if(length == 0)
            return true;
        return send<char>(data.c_str(), length);
    }

    return false;
}//send()



template<>
bool QSocketConnection::recv(std::string& data)
{
    unsigned int length = 0;

    if (!recv<unsigned int>(length))
        return false;

    if (length == 0)
    {
        data = "";
        return true;
    }

    char* buffer = new char[length];
    if (!recv<char>(buffer, length))
    {
        delete buffer;
        return false;
    }

    data = std::string(buffer, length);
    delete buffer;

    return true;
}//recv()



bool QSocketConnection::disconnect()
{
    QMutexLocker lock(&m_mutex);

    if(m_socket)
    {
        m_socket->disconnectFromHost();
        delete m_socket;
    }

    m_socket = 0;
    return true;
}//disconnect()



bool QSocketConnection::connected() const
{
    return m_socket != 0 && m_socket->isValid();
}



QSocketConnection *QSocketConnection::clone(bool master)
{
    QHostAddress peer = m_socket->peerAddress();

    if(master)
    {
        QTcpServer *server = new QTcpServer(this);
        server->listen();
        int port = server->serverPort();
        send<int>(&port, 1);
        m_socket->flush();

        if(server->waitForNewConnection())
        {
            QTcpSocket *clientSocket = server->nextPendingConnection();
            QSocketConnection *connection = new QSocketConnection(clientSocket);
            //should we close the server socket here?
            return connection;
        }
        else
        {
            server->close();
            return new QSocketConnection(0);
        }
    }
    else
    {
        int port = -1;

        recv<int>(&port, 1);
        return new QSocketConnection(peer, port, 10);
    }
}//clone()



bool QSocketConnection::_send(const void *data, unsigned int count)
{
    unsigned int bytesSent = 0, totalSent = 0;

    while (totalSent < count)
    {
        if (!connected())
            return false;

        bytesSent = m_socket->write((const char*)data + totalSent, count - totalSent);
        totalSent += bytesSent;

        if(bytesSent == 0 && !m_socket->waitForBytesWritten())
            break;
    }

    return totalSent == count;
}//_send()



bool QSocketConnection::_recv(void *data, unsigned int count)
{
    unsigned int bytesRead = 0, totalRead = 0;

    while (totalRead < count)
    {
        if (!connected())
            return false;

        bytesRead = m_socket->read((char*)data + totalRead, count - totalRead);
        totalRead += bytesRead;

        if(bytesRead == 0 && !m_socket->waitForReadyRead())
            break;
    }

    return totalRead == count;
}//_recv()
