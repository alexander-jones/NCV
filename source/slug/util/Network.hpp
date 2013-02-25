//namespace slug::util

template<typename T> 
bool SocketConnection::send(const T* data, unsigned int count)
{
	return _send(data, sizeof(T) * count);
}

template<typename T>
bool SocketConnection::recv(T* data, unsigned int count)
{
	return _recv(data, sizeof(T) * count);
}

template<typename T>
bool SocketConnection::send(const T& data)
{
	return _send(&data, sizeof(T));
}

template<typename T>
bool SocketConnection::recv(T& data)
{
	return _recv(&data, sizeof(T));
}


