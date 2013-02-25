#pragma once
#include <slug/util/BinaryStream.h>
#include <string>
#include <map>
#include <fstream>

namespace slug
{

namespace util
{

class Specification
{
public:
	///Default constructor
	Specification();

	///Sets the parameter to the given primitive value
	template<typename T> bool set(const std::string& name, const T& v);

	///Sets a parameter to the given string
	bool set(const std::string& name, const std::string& v);

	///Sets a parameter to the given string
	bool set(const std::string& name, const char* v);

	///Sets the parameter to the input array
	template<typename T> 
	bool set(const std::string& name, const T* v, size_t size);

	template<typename T> T get(const std::string& name, T defValue = (T)0);
	std::string get(const std::string& name);
	template<typename T>
	T* getArray(const std::string& name);

	///Returns the number of parameters in this specification
	const size_t numParameters() const;

	bool write(std::ostream& os) const;
	bool write(util::OutputStream& os) const;

	bool read(std::istream& is);
private:
	struct Parameter
	{
		Parameter(char* d, unsigned int s);
		char* data;
		unsigned int size;
		~Parameter();
	};
	std::map<std::string, Parameter*> _parameters;
};

#include <slug/util/Specification.hpp>

} //namespace util 

} //namespace slug
