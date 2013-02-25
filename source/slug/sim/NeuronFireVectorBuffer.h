/*
	NeuronFireVectorBuffer.h
*/

#pragma once
#include <slug/sim/DataBuffer.h>

namespace slug
{

namespace sim
{

/**
	Encapsulates just the complete neuron fire vector. Emitted from the 
	VectorExchanger.

	@param MType The type of memory to use for allocation of internals
*/
template<MemoryType MType>
class NeuronFireVectorBuffer : public DataBuffer, public Memory<MType>
{
public:
	/**
		Constructor

		@param _bitVectorSize The size of the complete fire vector
		@param lock True if page-locked memory should be used
	*/
	NeuronFireVectorBuffer(unsigned int _bitVectorSize, bool lock = false);

	/**
		Resets the buffer to be used again.
	*/
	void clear();

	///Destructor
	~NeuronFireVectorBuffer();

	///The number of words needed to represent the fire vector
	unsigned int bitVectorSize;

	///The device level neuron fire vector
	unsigned int* neuronFireVector;
};

#include <slug/sim/NeuronFireVectorBuffer.hpp>

} //namespace sim

} //namespace slug
