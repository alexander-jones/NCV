/*
	namespace slug::sim
*/

template<typename T>
BinaryFileDataSink<T>::BinaryFileDataSink(const std::string& path,
                                          bool reorder)
	: _file(path, std::ios_base::out | std::ios_base::binary),
	  _reorder(reorder)
{
}

template<typename T>
bool BinaryFileDataSink<T>::good() const
{
	return _file.good();
}

template<typename T>
bool BinaryFileDataSink<T>::begin(const std::vector<unsigned int>& indices)
{
	if (_reorder)
	{
		_indices = indices;
		return true;
	}

	unsigned int numIndices = indices.size();
	_file.write((const char*)&numIndices, sizeof(unsigned int));
	_file.write((const char*)indices.data(), sizeof(unsigned int) * numIndices);
	return true;
}

template<typename T> 
bool BinaryFileDataSink<T>::accept(const void* data, 
                                  unsigned int size, 
								  unsigned int step)
{
	const T* d = (const T*)data;
	unsigned int numElements = size / sizeof(T);
	_file.write((const char*)&step, sizeof(unsigned int));

	if (!_reorder)
	{
		unsigned int s = size;
		_file.write((const char*)&s, sizeof(unsigned int));
		_file.write((const char*)data, size);
	}
	else
	{
		unsigned int s = sizeof(T) * _indices.size();
		_file.write((const char*)&s, sizeof(unsigned int));

		for (auto index : _indices)
		{
			T value = d[index];
			_file.write((const char*)&value, sizeof(T));
		}
	}
	return true;
}

template<typename T>
bool BinaryFileDataSink<T>::end()
{
	_file.close();
	return true;
}

template<typename T>
bool BinaryFileDataSink<T>::error(const std::string& message)
{
	std::cerr << message << std::endl;
	return true;
}

template<typename T>
AsciiFileDataSink<T>::AsciiFileDataSink(const std::string& path,
                                        bool reorder)
	: _file(path),
	  _reorder(reorder)
{
}

template<typename T>
bool AsciiFileDataSink<T>::good() const
{
	return _file.good();
}

template<typename T>
bool AsciiFileDataSink<T>::begin(const std::vector<unsigned int>& indices)
{
	if (_reorder)
	{
		_indices = indices;
		return true;
	}

	_file << indices.size() << std::endl;
	for (auto index : indices)
	{
		_file << index << " ";
	}
	_file << std::endl;
	return true;
}

template<typename T> 
bool AsciiFileDataSink<T>::accept(const void* data, 
                                  unsigned int size, 
								  unsigned int step)
{
	const typename util::Storage<T>::elementType* d = 
		(const typename util::Storage<T>::elementType*)data;
	unsigned int numElements = size / sizeof(T);
	_file << step << " ";
	if (!_reorder)
	{
		for (unsigned int i = 0; i < numElements; ++i)
		{
			_file << util::Storage<T>::access(d, i) << " ";
		}
	}
	else
	{
		for (auto index : _indices)
		{
			_file << util::Storage<T>::access(d, index) << " ";
		}
	}
	_file << std::endl;
	return true;
}

template<typename T>
bool AsciiFileDataSink<T>::end()
{
	_file.close();
}

template<typename T>
bool AsciiFileDataSink<T>::error(const std::string& message)
{
	std::cerr << message << std::endl;
	return true;
}
