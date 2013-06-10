#ifndef QSOCKETCONNECTION_H
#define QSOCKETCONNECTION_H

#include <QHostAddress>
#include <QTcpSocket>
#include <QMutex>
#include <string>


/**
  \class QSocketConnection
  \author Justin Cardoza
  \brief This class wraps the standard Qt TCP socket connection for interoperability with
         NCS's internal report server.
  */
class QSocketConnection : public QObject
{
    Q_OBJECT
public:
    QSocketConnection(QTcpSocket *socket);
    QSocketConnection(const std::string& server, int port, float timeout = 1.0f, int attempts = 10);
    QSocketConnection(const QHostAddress& addr, int port, float timeout = 1.0f, int attempts = 10);
    virtual ~QSocketConnection();

    template<typename T> bool send(const T* data, unsigned int count);
    template<typename T> bool recv(T* data, unsigned int count);
    template<typename T> bool send(const T& data);
    bool send(const char* data);
    template<typename T> bool recv(T& data);
    bool disconnectFromHost();
    bool isConnected() const;
    QSocketConnection* clone(bool master);

private:
    bool _send(const void* data, unsigned int count);
    bool _recv(void* data, unsigned int count);

    QTcpSocket *m_socket;
    QMutex m_mutex;
};



///A specialized implementation of send() that writes a string to the socket.
template<>
bool QSocketConnection::send(const std::string& data);

///A specialized implementation of recv() that reads a string from the connection.
template<>
bool QSocketConnection::recv(std::string& data);



///Sends a data buffer of some arbitrary type over the connection.
template<typename T>
bool QSocketConnection::send(const T* data, unsigned int count)
{
    return _send(data, sizeof(T) * count);
}



///Receives data over the connection into a data buffer of some arbitrary type.
template<typename T>
bool QSocketConnection::recv(T* data, unsigned int count)
{
    return _recv(data, sizeof(T) * count);
}



///Sends a single value of an arbitrary type.
template<typename T>
bool QSocketConnection::send(const T& data)
{
    return _send(&data, sizeof(T));
}



///Receives a single value of an arbitrary type.
template<typename T>
bool QSocketConnection::recv(T& data)
{
    return _recv(&data, sizeof(T));
}


#endif
