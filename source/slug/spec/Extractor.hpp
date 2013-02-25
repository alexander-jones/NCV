template<typename T>
bool Extractor::writeSingle(const std::string& name,
                            const T& v,
							std::ostream& os)
{
	writeHeader(name, sizeof(T), os);
	os.write((const char*)&v, sizeof(T));
	return true;
}

template<typename T>
bool Extractor::writeVector(const std::string& name,
                            const std::vector<T>& v,
							std::ostream& os)
{
	writeHeader(name, sizeof(T) * v.size(), os);
	os.write((const char*)v.data(), sizeof(T) * v.size());
	return true;
}

template<typename T, typename F>
UniformExtractor<T,F>:: UniformExtractor(const std::string& n, const F& f)
	: _name(n),
	  _f(f)
{
}

template<typename T, typename F>
bool UniformExtractor<T,F>::serialize(ElementList& el, std::ostream& os)
{
	writeHeader(_name, el.size() * sizeof(T), os);
	for (auto e : el)
	{
		T result = (T)(_f(e));
		os.write((const char*)&result, sizeof(T));
	}
}

template<typename T, typename F>
unsigned int UniformExtractor<T,F>::numParameters() const
{
	return 1;
}
