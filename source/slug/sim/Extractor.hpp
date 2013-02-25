/*
	namespace slug::sim
*/

template<typename T>
Extractor<T>::Extractor(sim::Publisher* publisher)
	: _subscription(nullptr),
	  _skip(1),
	  _frameCount(0),
	  _currentBuffer(nullptr),
	  _lastStep(0)
{
	_subscription = publisher->subscribe();
	_publisher = publisher;
}

template<typename T>
bool Extractor<T>::setSkip(unsigned int skip)
{
	if (skip == 0)
		return false;
	_skip = skip;
	return true;
}

template<typename T>
bool Extractor<T>::setKey(util::Unique key)
{
	_key = key;
	return true;
}

template<typename T>
bool Extractor<T>::extract(typename util::Storage<T>::elementType* buffer)
{
	sim::DataBuffer* publication = nullptr;
	if (_currentBuffer)
	{
		publication = _currentBuffer;
		_currentBuffer = nullptr;
	}
	else
		publication = _subscription->pull();
	
	if (nullptr == publication)
		return false;

	for (unsigned int i = 1; i < _skip; ++i)
	{
		publication->release();
		if (_currentBuffer)
		{
			publication = _currentBuffer;
			_currentBuffer = nullptr;
		}
		else
			publication = _subscription->pull();
		if (nullptr == publication)
			return false;
	}

	auto pin = publication->getPin(_key);
	bool result = _extract(buffer, pin);
	_lastStep = publication->simulationStep;
	publication->release();
	return result;
}

template<typename T>
bool Extractor<T>::sync(unsigned int step, unsigned int& resultStep)
{
	if (nullptr == _currentBuffer)
	{
		_currentBuffer = _subscription->pull();
		if (_currentBuffer == nullptr)
			return false;
	}

	if (_currentBuffer->simulationStep > step)
	{
		resultStep = _currentBuffer->simulationStep;
		return true;
	}

	while (_currentBuffer->simulationStep != step)
	{
		_currentBuffer->release();
		_currentBuffer = _subscription->pull();
		if (_currentBuffer == nullptr)
			return false;
	}

	resultStep = step;
	return true;
}

template<typename T>
bool Extractor<T>::deviceInit()
{
	sim::Device::device = _publisher->device;
	_publisher->device->threadInit();
	return true;
}

template<typename T>
unsigned int Extractor<T>::lastStep() const
{
	return _lastStep;
}

template<typename T>
Extractor<T>::~Extractor()
{
	delete _subscription;
}

template<typename T>
CPUExtractor<T>::CPUExtractor(sim::Publisher* publisher)
	: Extractor<T>(publisher)
{
}

template<typename T>
bool CPUExtractor<T>::setIndices(const unsigned int* indices,
                                 unsigned int count)
{
	for (unsigned int i = 0; i < count; ++i)
	{
		_indices.push_back(indices[i]);
	}
	return true;
}

template<typename T>
bool CPUExtractor<T>::_extract(typename util::Storage<T>::elementType* buffer,
                               sim::DataBuffer::Pin& pin)
{
	const auto* data = (const typename util::Storage<T>::elementType*)(pin.data);
	for (unsigned int i = 0; i < _indices.size(); ++i)
	{
		buffer[i] = util::Storage<T>::access(data, _indices[i]);
	}
	return true;
}

#ifdef SLUG_CUDA

template<typename T>
CUDAExtractor<T>::CUDAExtractor(sim::Publisher* publisher)
	: Extractor<T>(publisher)
{
}

template<typename T>
bool CUDAExtractor<T>::setIndices(const unsigned int* indices,
                                  unsigned int count)
{
	_numIndices = count;
	_indices = _allocate<unsigned int>(_numIndices);
	_transfer<sim::MemoryType::CUDA,
	          sim::MemoryType::System,
			  unsigned int>
		(_indices,
		 indices,
		 _numIndices);

	_cudaBuffer = 
		_allocate<typename util::Storage<T>::elementType>
			(util::Storage<T>::numElements(_numIndices));
		
	return true;
}

template<typename T>
bool CUDAExtractor<T>::_extract(typename util::Storage<T>::elementType* buffer,
                                sim::DataBuffer::Pin& pin)
{
	::extract<T>((typename util::Storage<T>::elementType*)pin.data, 
	             _cudaBuffer, 
			     _indices, 
			     _numIndices); 

	_transfer<sim::MemoryType::System,
	          sim::MemoryType::CUDA,
			  typename util::Storage<T>::elementType>
		(buffer, 
		 _cudaBuffer, 
		 util::Storage<T>::numElements(_numIndices));
	return true;
}

#endif //SLUG_CUDA
