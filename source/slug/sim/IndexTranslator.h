#pragma once
#include <slug/sim/ElementMap.h>

namespace slug
{

namespace sim
{

class IndexTranslator
{
public:
	IndexTranslator();
	bool readMaps(const std::string& path);
	ElementMap neuronMap;
	ElementMap synapseMap;
	ElementMap inputMap;
};

} //namespace sim

} //namespace slug
