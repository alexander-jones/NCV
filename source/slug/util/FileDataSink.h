#pragma once
#include <slug/util/DataSink.h>
#include <slug/util/Storage.h>
#include <fstream>

namespace slug
{

namespace util
{

template<typename T>
class BinaryFileDataSink
	: public DataSink
{
public:
	BinaryFileDataSink(const std::string& path, bool reorder = false);
	bool good() const;
	virtual bool begin(const std::vector<unsigned int>& indices);
	virtual bool accept(const void* data, unsigned int size, unsigned int step);
	virtual bool end();
	virtual bool error(const std::string& message);
private:
	std::ofstream _file;
	std::vector<unsigned int> _indices;
	bool _reorder;
};

template<>
bool BinaryFileDataSink<Bit>::accept(const void* data, 
                                     unsigned int size, 
									 unsigned int step);

template<typename T>
class AsciiFileDataSink
	: public DataSink
{
public:
	AsciiFileDataSink(const std::string& path, bool reorder = false);
	bool good() const;
	virtual bool begin(const std::vector<unsigned int>& indices);
	virtual bool accept(const void* data, unsigned int size, unsigned int step);
	virtual bool end();
	virtual bool error(const std::string& message);
private:
	std::ofstream _file;
	bool _reorder;
	std::vector<unsigned int> _indices;
};

#include <slug/util/FileDataSink.hpp>

} //namespace util

} //namespace slug
