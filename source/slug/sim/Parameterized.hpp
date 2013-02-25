template<typename T>
T Parameterized::getParameter(const std::string& name, 
                              const T& defaultValue)
{
	auto it = _parameters.find(name);
	if (it == _parameters.end())
		return defaultValue;
	else
		return *(T*)(it->second);
}

template<typename T>
T* Parameterized::getParameterArray(const std::string& name)
{
	auto it = _parameters.find(name);
	if (it == _parameters.end())
		return nullptr;
	else
		return (T*)it->second;
}

