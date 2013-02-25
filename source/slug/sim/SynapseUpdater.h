#pragma once

#include <slug/sim/SynapticCurrentBuffer.h>
#include <slug/sim/SynapseSimulator.h>
#include <slug/sim/NeuronStateBuffer.h>
#include <slug/sim/SynapticFireVectorBuffer.h>
#include <slug/sim/DataBuffer.h>

namespace slug
{

namespace sim
{

/**
	For a single device, this object updates a SynapticCurrentBuffer using
	all SynapseSimulators on that device.

	@param MType The memory type of the device this updater resides on
*/
template<MemoryType MType>
class SynapseUpdater 
	: public SpecificPublisher<SynapticCurrentBuffer<MType>>
{
public:
	/**
		Constructor

		@param numNeurons The number of neurons on the device
		@param simulators All synapse simulators on this device
		@param numBuffers The number of current buffers that should be
			allocated
	*/
	SynapseUpdater(unsigned int numNeurons,
	               std::vector<SynapseSimulator<MType>*> simulators,
				   unsigned int numBuffers = 3);

	/**
		Executes the SynapseUpdater for the requisite number of steps

		@param properties Simulation properties for this run
		@return true if successful; false otherwise
	*/
	bool run(const SimulationProperties* properties);

	typedef SpecificPublisher<NeuronStateBuffer<MType>>
		NeuronStatePublisher;

	/**
		Sets the publisher of the neuron state to subscribe to

		@param publisher Publisher of the neuron state
	*/
	void neuronStatePublisher(NeuronStatePublisher* publisher);

	typedef SpecificPublisher<SynapticFireVectorBuffer<MType>>
		SynapticFireVectorPublisher;

	/**
		Sets the publisher of the synaptic fire state to subscribe to

		@param publisher The publisher of the current synaptic firing state
	*/
	void synapticStatePublisher(SynapticFireVectorPublisher* publisher);

	/**
		Initializes all components of the updater

		@param properties Simulation properties for this run
		@return true if successful; false otherwise
	*/
	bool init(const SimulationProperties* properties);

	virtual ~SynapseUpdater();
private:
	///The number of buffers to maintain
	unsigned int _numBuffers;

	///The number of elements to store a synaptic variable
	unsigned int _numNeurons;

	///The set of simulators used to update all local synapses
	std::vector<SynapseSimulator<MType>*> _simulators;

	///Subscription to the neuron fire vector
	Subscription<NeuronStateBuffer<MType>>* _neuronStateSubscription;

	///Subscription to the synapse fire vector
	Subscription<SynapticFireVectorBuffer<MType>>* _synapticStateSubscription;
};

#include <slug/sim/SynapseUpdater.hpp>

} //namespace sim

} //namespace slug
