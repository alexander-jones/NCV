template<typename T>
bool Manager<T>::add(T* t)
{
	return instance()->_add(t);
}

template<typename T>
bool Manager<T>::initialize()
{
	return instance()->_initialize();
}

template<typename T>
T* Manager<T>::get(const std::string& type)
{
	return instance()->_get(type);
}

template<typename T>
bool Manager<T>::prefer(const std::string& type, Preference* p)
{
	return instance()->_prefer(type, p);
}

template<typename T>
template<typename F>
bool Manager<T>::map(F& f)
{
	return instance()->_map(f);
}

template<typename T>
Manager<T>::Manager()
{
}

template<typename T>
Manager<T>* Manager<T>::instance()
{
	if (nullptr == _instance)
		_instance = new Manager<T>();
	return _instance;
}

template<typename T>
bool Manager<T>::_add(T* t)
{
	_potential[t->type()].push_back(t);
	return true;
}

template<typename T>
bool Manager<T>::_initialize()
{
	//if no preference exists, we resolve purely based on revision number
	RevisionPreference defaultPreference;

	//For each type of plugin service
	for (auto it : _potential)
	{
		//Get the type of service
		const std::string& type = it.first;

		//If a preference was declared, override the default
		Preference* preference = &defaultPreference;
		if (_preferences.find(type) != _preferences.end())
			preference = _preferences[type];

		//Max scan for the best potential candidate
		T* best = *(it.second.begin());
		for (auto jt : it.second)
		{
			if ((*preference)(best, jt))
				best = jt;
		}

		_final[type] = best;
	}
	return true;
}

template<typename T>
T* Manager<T>::_get(const std::string& type)
{
	return _final[type];
}

template<typename T>
bool Manager<T>::_prefer(const std::string& type, Preference* p)
{
	_preferences[type] = p;
	return true;
}

template<typename T>
template<typename F>
bool Manager<T>::_map(F& f)
{
	bool result = true;
	for (auto it : _final)
	{
		result &= f(it->second);
	}
	return result;
}

