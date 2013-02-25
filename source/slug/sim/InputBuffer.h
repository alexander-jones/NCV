/*
	InputBuffer.h
*/

#pragma once
#include <slug/sim/DataBuffer.h>

namespace slug
{

namespace sim
{

/**
	Encapsulates device level data of external inputs to neurons. This data
	is published by the Input Update step and consumed by the Neuron Update 
	step.

	@param MType The memory type of the device this is for
*/
template<MemoryType MType>
class InputBuffer : public DataBuffer, public Memory<MType>
{
public:
	/**
		Constructor

		@param _vectorSize The number of elements to represent the
			input for every neuron on the device
	*/
	InputBuffer(unsigned int _vectorSize);

	/**
		Resets the data in the buffer so that it can be used again.
	*/
	void clear();

	///Destructor
	~InputBuffer();

	///The number of neurons this buffer handles
	unsigned int vectorSize;

	///The number of words for the voltage clamp buffer
	unsigned int numWords;

	///Bit vector for whether we should voltage clamp
	unsigned int* voltageClamp;

	///The voltage to clamp to
	float* clampVoltage;

	///The additional current given to a neuron
	float* inputCurrent;
};

#include <slug/sim/InputBuffer.hpp>

} //namespace sim

} //namespace slug
