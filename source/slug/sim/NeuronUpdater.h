#pragma once

#include <slug/sim/NeuronStateBuffer.h>
#include <slug/sim/NeuronSimulator.h>
#include <slug/sim/SynapticCurrentBuffer.h>
#include <slug/sim/Synchronizer.h>
#include <thread>

namespace slug
{

namespace sim
{

//Forward declaration
template<MemoryType MType> class InputUpdater;

/**
	For a single device, this object updates a NeuronStateBuffer using
	all NeuronSimulators residing on this device.

	@param MType The memory type of the device this updater resides on
*/
template<MemoryType MType>
class NeuronUpdater : public SpecificPublisher<NeuronStateBuffer<MType>>
{
public:
	/**
		Constructor

		@param bitVectorSize The number of 32-bit words in the device-level
			neuron fire vector
		@param voltageVectorSize The number of elements to store data about
			every neuron on the device
		@param simulators The set of neuron simulators on this device
		@param numBuffers The number of publications to maintain at a time
	*/
	NeuronUpdater(unsigned int bitVectorSize,
	              unsigned int voltageVectorSize,
	              std::vector<NeuronSimulator<MType>*> simulators,
				  unsigned int numBuffers = 3);

	virtual ~NeuronUpdater();

	/**
		Sets where to receive InputBuffers from

		@param publisher The publisher of InputBuffers
	*/
	void inputPublisher(SpecificPublisher<InputBuffer<MType>>* publisher);

	/**
		Executes the NeuronUpdater for the requisite number of steps

		@param properties Simulation properties for this run
		@return true if successful; false otherwise
	*/
	bool run(const SimulationProperties* properties);

	/**
		Initializes all components of the updater

		@param properties Simulation properties for this run
		@return true if successful; false otherwise
	*/
	bool init(const SimulationProperties* properties);

	typedef SpecificPublisher<SynapticCurrentBuffer<MType>>
		SynapticCurrentVectorPublisher;

	/**
		Sets where to receive SynapticCurrentBuffers from

		@param publisher The publisher of current buffers
	*/
	void synapticCurrentPublisher(SynapticCurrentVectorPublisher* publisher);
private:
	///The number of output buffers to maintain
	unsigned int _numBuffers;

	///The number of 32-bit words to store the fire state of all local neurons
	unsigned int _bitVectorSize;

	///The number of floats to store the voltages of all local neurons
	unsigned int _voltageVectorSize;

	///The set of simulators used to update all local neurons
	std::vector<NeuronSimulator<MType>*> _simulators;

	///Source of InputBuffers
	Subscription<InputBuffer<MType>>* _inputBufferSubscription;

	///Source of previous neuron state (self)
	Subscription<NeuronStateBuffer<MType>>* _neuronStateSubscription;

	///Source of synaptic current buffers
	Subscription<SynapticCurrentBuffer<MType>>* _synapticCurrentSubscription;
};

#include <slug/sim/NeuronUpdater.hpp>

} //namespace sim

} //namespace slug
