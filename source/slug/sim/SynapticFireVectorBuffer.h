/*
	SynapticFireVectorBuffer.h
*/

#pragma once
#include <slug/sim/DataBuffer.h>

namespace slug
{

namespace sim
{

/**
	Encapsulates the fire state of every synapse on a device for a single
	timestep.

	Emitted by the FireTableUpdater.
*/
template<MemoryType MType>
class SynapticFireVectorBuffer 
	: public DataBuffer, 
	  public Memory<MType>
{
public:
	/**
		Constructor

		@param vectorSize The number of 32-bit words needed to store the vector
	*/
	SynapticFireVectorBuffer(unsigned int vectorSize);

	/**
		Resets the buffer for reuse
	*/
	void clear();

	/**
		Sets pins to point to the appropriate data after pointers have changed
	*/
	void updatePin();

	///The number of words needed to represent the fire vector
	unsigned int bitVectorSize;

	///The fire vector - if this value is changed, updatePins should be called
	unsigned int* synapticFireVector;

	///The table row this buffer refers to
	unsigned int fireTableRow;

private:
	///Unique for "synapseFire" to reduce string lookups
	util::Unique _synapseFireID;
};

#include <slug/sim/SynapticFireVectorBuffer.hpp>
} //namespace sim

} //namespace slug
