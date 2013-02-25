#pragma once

#include <slug/sim/InputBuffer.h>
#include <slug/sim/InputSimulator.h>
#include <slug/sim/Device.h>
#include <slug/sim/SimulationProperties.h>
#include <slug/sim/DataBuffer.h>
#include <slug/sim/Synchronizer.h>
#include <thread>

namespace slug
{

namespace sim
{

/**
	For a single device, the InputUpdater updates an InputBuffer using
	all InputSimulators residing on this device.

	@param MType The memory type of the device
*/
template<MemoryType MType>
class InputUpdater 
	: public SpecificPublisher<InputBuffer<MType>>
{
public:
	/**
		Constructor

		@param voltageVectorSize The number of elements to allocate to cover
			all neurons on the device
		@param simulators All InputSimulators on the device
		@param numBuffers The number of InputBuffers to maintain
	*/
	InputUpdater(unsigned int voltageVectorSize,
	             std::vector<InputSimulator<MType>*> simulators,
	             unsigned int numBuffers = 3);

	/**
		Initializes all components of the updater.
	*/
	bool init(const SimulationProperties* properties);

	/**
		Executes the InputUpdater for the requisite number of simulation steps.
	*/
	bool run(const SimulationProperties* properties);
private:
	///The number of publishable buffers to maintain
	unsigned int _numBuffers;

	///The number of words to store the state of all local neurons
	unsigned int _voltageVectorSize;

	///All InputSimulators on this device
	std::vector<InputSimulator<MType>*> _simulators;
};

#include <slug/sim/InputUpdater.hpp>

} //namespace sim

} //namespace slug
