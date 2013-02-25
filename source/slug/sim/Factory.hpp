template<typename T>
Factory<T>::Factory(const std::string& __type,
	                const std::string& __name,
					unsigned int __revision)
	: Plugin(__type, __name, __revision)
{
}

template<typename Base, typename Specific>
SpecificFactory<Base, Specific>::
SpecificFactory(const std::string& __type,
                const std::string& __name,
				unsigned int __revision)
	: Factory<Base>(__type, __name, __revision)
{
}

template<typename Base, typename Specific>
Base* SpecificFactory<Base, Specific>::generate() 
{ 
	return new Specific(); 
}
