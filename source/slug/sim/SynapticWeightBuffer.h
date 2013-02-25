#pragma once
#include <slug/sim/DataBuffer.h>

namespace slug
{

namespace sim
{

template<MemoryType MType>
class SynapticWeightBuffer
	: public DataBuffer,
	  public Memory<MType>
{
public:
	///Constructor
	SynapticWeightBuffer(unsigned int vectorSize);

	void clear();

	///The number of elements in the vector
	unsigned int numElements;

	///The weight vector
	float* synapticWeightVector;
};

#include <slug/sim/SynapticWeightBuffer.hpp>

} //namespace sim

} //namespace slug
