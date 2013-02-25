#pragma once
#include <string>
#include <vector>

namespace slug
{

namespace util
{

class DataSink
{
public:
	virtual bool begin(const std::vector<unsigned int>& indices) = 0;
	virtual bool accept(const void* data, 
	                    unsigned int size, 
						unsigned int step) = 0;
	virtual bool end() = 0;
	virtual bool error(const std::string& message) = 0;
private:
};

} //namespace util

} //namespace slug
