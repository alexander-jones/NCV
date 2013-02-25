/*
	namespace slug::sim
*/

template<typename T>
Synchronizer<T>::Synchronizer(unsigned int numBuffers)
	: _failed(false),
	  _maxStep(0)
{
	for (unsigned int i = 0; i < numBuffers; ++i)
		_buffers.push_back(nullptr);
}

template<typename T>
T* Synchronizer<T>::pull(unsigned int step)
{
	std::unique_lock<std::mutex> lock(_lock);
	while (!failed())
	{
		if (step < _maxStep)
		{
			unsigned int slot = step % _buffers.size();
			return _buffers[slot];
		}
		_stateChanged.wait(lock);
	}
	return nullptr;
}

template<typename T>
void Synchronizer<T>::clear(unsigned int step)
{
	unsigned int slot = step % _buffers.size();
	std::unique_lock<std::mutex> lock(_lock);
	_buffers[slot] = nullptr;
}

template<typename T>
bool Synchronizer<T>::push(T* buffer)
{
	std::unique_lock<std::mutex> lock(_lock);
	if (failed())
		return false;
	unsigned int slot = _maxStep % _buffers.size();
	_buffers[slot] = buffer;
	++_maxStep;
	_stateChanged.notify_all();
	return true;
}

template<typename T>
bool Synchronizer<T>::failed() const
{
	return _failed;
}

template<typename T>
void Synchronizer<T>::fail()
{
	std::unique_lock<std::mutex> lock(_lock);
	_failed = true;
}

template<typename T>
void Synchronizer<T>::map(std::function<void(T*)> f)
{
	for (auto buffer : _buffers)
	{
		if (nullptr != buffer)
			f(buffer);
	}
}
