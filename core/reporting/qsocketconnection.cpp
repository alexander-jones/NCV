#include "qsocketconnection.h"
#include "sleeper.h"
#include <QMutexLocker>
#include <QTcpServer>
#include <QThread>



QSocketConnection::QSocketConnection(QTcpSocket *socket)
    : m_socket(socket)
{
}



QSocketConnection::QSocketConnection(const std::string &server, int port, float timeout, int attempts)
{
    if(attempts <= 0)
        attempts = 1;

    int delayPerAttempt = int(timeout * 1000) / attempts;
    bool done = false;

    m_socket = new QTcpSocket(this);

    while(attempts > 0 && !done)
    {
        m_socket->connectToHost(QString::fromStdString(server), port);
        if(m_socket->waitForConnected(delayPerAttempt))
            done = true;
        else
            Sleeper::msleep(delayPerAttempt);

        qDebug() << "Attempt completed, done = " << done << " and error = " << m_socket->errorString();
        attempts--;
    }
}



QSocketConnection::QSocketConnection(const QHostAddress &addr, int port, float timeout, int attempts)
{
    if(attempts <= 0)
        attempts = 1;

    int delayPerAttempt = int(timeout * 1000) / attempts;
    bool done = false;

    m_socket = new QTcpSocket(this);

    while(attempts > 0 && !done)
    {
        m_socket->connectToHost(addr, port);
        if(m_socket->waitForConnected(delayPerAttempt))
            done = true;
        else
            Sleeper::msleep(delayPerAttempt);

        qDebug() << "Attempt completed, done = " << done << " and error = " << m_socket->errorString();
        attempts--;
    }
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



bool QSocketConnection::disconnectFromHost()
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



bool QSocketConnection::isConnected() const
{
    return m_socket != 0 && m_socket->isValid() && m_socket->state() == QAbstractSocket::ConnectedState;
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
        if (!isConnected())
        {
            qDebug() << "Not connected: " << m_socket->errorString();
            return false;
        }

        bytesSent = m_socket->write((const char*)data + totalSent, count - totalSent);
        totalSent += bytesSent;

        if(bytesSent <= 0 && !m_socket->waitForBytesWritten())
        {
            qDebug() << "Error writing data: " << m_socket->errorString();
            break;
        }
    }

    return totalSent == count;
}//_send()



bool QSocketConnection::_recv(void *data, unsigned int count)
{
    unsigned int bytesRead = 0, totalRead = 0;

    while (totalRead < count)
    {
        if (!isConnected())
        {
            qDebug() << "Not connected: " << m_socket->errorString() << " host = " << m_socket->peerName();
            return false;
        }

        m_socket->waitForReadyRead();

        //qDebug() << m_socket->bytesAvailable();
        bytesRead = m_socket->read((char*)data + totalRead, count - totalRead);

        if(bytesRead <= 0)
        {
            qDebug() << "Error reading data: " << m_socket->errorString();
            break;
        }
        totalRead += bytesRead;
    }


    return totalRead == count;
}//_recv()
