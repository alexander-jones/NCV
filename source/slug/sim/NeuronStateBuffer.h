/*
	NeuronStateBuffer.h
*/

#pragma once
#include <slug/sim/DataBuffer.h>

namespace slug
{

namespace sim
{

/**
	Encapsulates the device level neuron fire vector along with the neuron
	voltages. Emitted from a NeuronUpdater.

	@param MType The type of memory to allocate internals with
*/
template<MemoryType MType>
class NeuronStateBuffer : public DataBuffer, public Memory<MType>
{
public:
	/**
		Constructor

		@param _bitVectorSize The number of 32-bit words to hold the neuron
			fire vector
		@param _voltageVectorSize The number of elements to store the votlage
			for each neuron
	*/
	NeuronStateBuffer(unsigned int _bitVectorSize, 
	                  unsigned int _voltageVectorSize);

	///Destructor
	~NeuronStateBuffer();

	/**
		Clears the buffer so that it can be used again
	*/
	void clear();

	///The number of words needed to represent the fire vector
	unsigned int bitVectorSize;

	///The number of floats in the neuronVoltage vector
	unsigned int voltageVectorSize;

	///The voltage of each neuron
	float* neuronVoltage;

	///The device level neuron fire vector
	unsigned int* neuronFireVector;
};

#include <slug/sim/NeuronStateBuffer.hpp>

} //namespace sim

} //namespace slug
