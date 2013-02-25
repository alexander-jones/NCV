#pragma once

#include <slug/sim/FireTable.h>
#include <slug/sim/SynapticFireVectorBuffer.h>
#include <slug/sim/NeuronFireVectorBuffer.h>
#include <slug/sim/DataBuffer.h>
#include <slug/sim/SimulationProperties.h>

namespace slug
{

namespace sim
{

/**
	An object that can update the synaptic fire table given the global neuron
	fire vector for a given timestep. Publishes the synaptic fire vector up to
	minDelay steps ahead of the current timestep.

	@param MType The type of memory that the fire table resides on
*/
template<MemoryType MType>
class FireTableUpdater
	: public SpecificPublisher<SynapticFireVectorBuffer<MType>>,
	  public Memory<MType>
{
	typedef SpecificPublisher<NeuronFireVectorBuffer<MType>>
	    NeuronFireVectorPublisher;
public:
	/**
		Constructor

		@param presynapticNeuronIDs The global presynaptic neuron ID for every
			synapse on the device
		@param synapticDelays The propagation delay of firings for each synapse
			in milliseconds
		@param synapticVectorSize The number of elements needed to store data
			for every synapse on the device
		@param minDelay The minimum delay of all the synapses on the device
		@param maxDelay The maximum delay of all synapses on the device
	*/
	FireTableUpdater(unsigned int* presynapticNeuronIDs,
					 unsigned int* synapticDelays,
					 unsigned int synapticVectorSize,
	                 unsigned int minDelay,
					 unsigned int maxDelay);

	/**
		Executes the FireTableUpdater for the requisite number of simulation
		steps.

		@param properties The run-time properties of the simulation
		@return true if successful, false otherwise
	*/
	bool run(const SimulationProperties* properties);

	/**
		Sets the publisher of the complete neuron state to subscribe to.

		@param publisher The publisher to subscribe to
	*/
	void neuronVectorPublisher(NeuronFireVectorPublisher* publisher);

	/**
		Updates the fire table for a single timestep.
		
		@param The current timestep
		@param neuronFireVector The complete global neuron fire vector
		@param neuronFireVectorSize The number of 32-bit words that make up the
			neuron fire vector
		@param row The row of the fire table the current step corresponds to
		@param numRows The number of rows in the fire table
	*/
	void update(unsigned int timeStep,
	            unsigned int* neuronFireVector,
				unsigned int neuronFireVectorSize,
				unsigned int row,
				unsigned int numRows);

	~FireTableUpdater();

private:
	///The number of publishable buffers to maintain
	unsigned int _numBuffers;

	///Subscription to neuron fire state
	Subscription<NeuronFireVectorBuffer<MType>>* _subscription;

	///The FireTable 
	FireTable<MType>* _fireTable;

	///The minimum delay of all synapses on the device
	unsigned int _minDelay;

	///The maximum delay of all synapses on the device 
	unsigned int _maxDelay;
	
	///Device: The presynaptic neuron ID for each synapse
	unsigned int* _presynapticNeuronIDs;

	///Device: The synaptic delay for each synapse
	unsigned int* _synapticDelays;

	///The number of synaptic elements to update
	unsigned int _synapticVectorSize;

	///The number of words to store a single timestep of the fire vector
	unsigned int _fireVectorSize;
};

#include <slug/sim/FireTableUpdater.hpp>

} //namespace sim

} //namespace slug
