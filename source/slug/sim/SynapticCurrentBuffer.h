#pragma once

#include <slug/sim/DataBuffer.h>

namespace slug
{

namespace sim
{

/**
	Encapsulates the device level total synaptic current to each neuron.
*/
template<MemoryType MType>
class SynapticCurrentBuffer
	: public DataBuffer,
	  public Memory<MType>
{
public:
	/**
		Constructor

		@param _numNeurons The number of neurons on the device
	*/
	SynapticCurrentBuffer(unsigned int _numNeurons);

	/**
		Resets the buffer for reuse
	*/
	void clear();

	/**
		Destructor
	*/
	~SynapticCurrentBuffer();

	///The number of neurons on the device
	unsigned int numNeurons;

	///The total synaptic current per neuron
	float* synapticCurrent;
};

#include <slug/sim/SynapticCurrentBuffer.hpp>

} //namespace sim

} //namespace slug
