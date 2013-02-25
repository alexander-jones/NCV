#pragma once
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <mutex>

namespace slug
{

namespace util
{

class SocketConnection
{
public:
	SocketConnection(int socket);
	SocketConnection(const std::string& server, int port, float timeout = 1.0f);
	SocketConnection(sockaddr_in* serverInfo, float timeout = 1.0f);
	template<typename T> bool send(const T* data, unsigned int count);
	template<typename T> bool recv(T* data, unsigned int count);
	template<typename T> bool send(const T& data);
	bool send(const char* data);
	template<typename T> bool recv(T& data);
	bool disconnect();
	bool connected() const;
	SocketConnection* clone(bool master);
	~SocketConnection();
private:
	bool _setNonblocking();
	bool _send(const void* data, unsigned int count);
	bool _recv(void* data, unsigned int count);
	bool _connect(sockaddr_in* serverAddress, float timeout);
	int _socket;
	std::mutex _lock;
};

template<>
bool SocketConnection::send(const std::string& data);

template<>
bool SocketConnection::recv(std::string& data);


#include <slug/util/Network.hpp>

} //namespace util

} //namespace slug
