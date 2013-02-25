#pragma once
#include <string>
#include <vector>
#include <slug/util/Specification.h>
#include <slug/util/DataSink.h>

namespace slug
{

namespace util
{

struct ReportRequest
{
	std::string key;
	std::string elementType;
	std::string datatype;
	std::string dataspace;
	unsigned int frequency;
	std::vector<unsigned int> indices;

	std::string name;

	bool serialize(const std::string& path);
	bool read(const std::string& path);

	Specification specification;

	DataSink* dataSink;
};

} //namespace util

} //namespace slug
