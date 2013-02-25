//namespace slug::util

template<typename T>
AtomicWriter<T>::AtomicWriter()
{
}

template<typename T>
void AtomicWriter<T>::write(T* location, const T& value)
{
	_locations.push_back(location);
	_values.push_back(value);
}

template<typename T>
void AtomicWriter<T>::commit(std::function<void(T*,const T&)> op)
{
	for (unsigned int i = 0; i < _locations.size(); ++i)
		op(_locations[i], _values[i]);
	_locations.clear();
	_values.clear();
}
