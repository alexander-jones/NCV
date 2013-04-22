#ifndef QSOCKETCONNECTION_H
#define QSOCKETCONNECTION_H

#include <QHostAddress>
#include <QTcpSocket>
#include <QMutex>
#include <string>


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
        bool disconnect();
        bool connected() const;
        QSocketConnection* clone(bool master);

    private:
        bool _send(const void* data, unsigned int count);
        bool _recv(void* data, unsigned int count);

        QTcpSocket *m_socket;
        QMutex m_mutex;
};



template<>
bool QSocketConnection::send(const std::string& data);

template<>
bool QSocketConnection::recv(std::string& data);



template<typename T>
bool QSocketConnection::send(const T* data, unsigned int count)
{
    return _send(data, sizeof(T) * count);
}



template<typename T>
bool QSocketConnection::recv(T* data, unsigned int count)
{
    return _recv(data, sizeof(T) * count);
}



template<typename T>
bool QSocketConnection::send(const T& data)
{
    return _send(&data, sizeof(T));
}



template<typename T>
bool QSocketConnection::recv(T& data)
{
    return _recv(&data, sizeof(T));
}


#endif
