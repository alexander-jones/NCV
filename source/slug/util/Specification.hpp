/*
	namespace slug::util
*/

template<typename T>
bool Specification::set(const std::string& name, const T& v)
{
	Parameter* original = _parameters[name];
	if (original != nullptr)
		delete original;
	char* data = new char[sizeof(T)];
	*(T*)data = v;
	_parameters[name] = new Parameter(data, sizeof(T));
	return true;
}

template<typename T>
bool Specification::set(const std::string& name, const T* v, size_t size)
{
	Parameter* original = _parameters[name];
	if (original != nullptr)
		delete original;
	unsigned int s = sizeof(T) * size;
	char* data = new char[s];
	T* d = (T*)data;
	for (unsigned int i = 0; i < size; ++i)
	{
		d[i] = v[i];
	}
	_parameters[name] = new Parameter(data, s);
	return true;
}

template<typename T>
T Specification::get(const std::string& name, T defValue)
{
	auto result = _parameters.find(name);
	if (result == _parameters.end())
		return defValue;
	T* v = (T*)(result->second->data);
	return *v;
}

template<typename T>
T* Specification::getArray(const std::string& name)
{
	auto result = _parameters.find(name);
	if (result == _parameters.end())
		return nullptr;
	T* v = (T*)(result->second->data);
	return v;
}
