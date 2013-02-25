#pragma once

#include <slug/sim/Parameterized.h>
#include <slug/sim/Manager.h>
#include <slug/sim/Factory.h>
#include <slug/sim/Memory.h>
#include <slug/util/Unique.h>
#include <slug/sim/SimulationProperties.h>
#include <slug/sim/NeuronStateBuffer.h>
#include <slug/sim/SynapticCurrentBuffer.h>

namespace slug
{

namespace sim
{

/**
	A simulator that can update a specific type of synapse on a specific type
	of device. All synapse simulators must implement this interface.

	@param MType The memory type the simulator operates on
*/
template<MemoryType MType>
class SynapseSimulator : public Parameterized, public Memory<MType>
{
public:
	///Unique ID
	util::Unique id;

	///Offset into a device-level element vector
	unsigned int synapticVectorOffset;

	///Word offset into a device-level bit vector
	unsigned int synapseFireVectorOffset;

	///The number of synapses this simulator is responsible for
	unsigned int numSynapses;

	///The device-level postsynaptic neuron ID for each synapse
	unsigned int* postsynapticNeuronIDs;
	
	/**
		Reads the plugin specification file for this simulator.

		@param prefix The directory where the file is stored
		@return true if successful; false otherwise
	*/
	bool read(const std::string& prefix);

	/**
		Initializes the simulator after all data has been read in.

		@param properties Simulation properties for this run
		@return true if successful; false otherwise
	*/
	virtual bool init(const SimulationProperties* properties) = 0;

	/**
		Parameters that are used to update synapses.
	*/
	struct UpdateParameters
	{
		///Device neuron fire vector
		const unsigned int* neuronFireVector;
		
		///The size of the neuron fire vector
		unsigned int neuronFireVectorSize;

		///The voltage of every neuron on the device
		const float* neuronVoltageVector;

		///Bit vector - 1 if the corresponding synapse should receive a firing
		const unsigned int* synapticFireVector;

		///The total synaptic current per device neuron
		float* synapticCurrentVector;

		SynapticCurrentBuffer<MType>* buffer;
	};

	/**
		Deprecated
	*/
	virtual void initWeights(float* weights,
	                         const SimulationProperties* properties) = 0;

	/**
		Updates synapses for a single timestep
		
		@param parameters Parameters used to update as well as parameters that
			need to be updated
	*/
	virtual void update(UpdateParameters& parameters) = 0;

	///Destructor
	virtual ~SynapseSimulator();
};

typedef SynapseSimulator<MemoryType::CUDA> CUDASynapseSimulator;
typedef Factory<CUDASynapseSimulator> CUDASynapseSimulatorFactory;
typedef Manager<CUDASynapseSimulatorFactory> CUDASynapseSimulators;

typedef SynapseSimulator<MemoryType::System> CPUSynapseSimulator;
typedef Factory<CPUSynapseSimulator> CPUSynapseSimulatorFactory;
typedef Manager<CPUSynapseSimulatorFactory> CPUSynapseSimulators;

typedef SynapseSimulator<MemoryType::OpenCL> CLSynapseSimulator;
typedef Factory<CLSynapseSimulator> CLSynapseSimulatorFactory;
typedef Manager<CLSynapseSimulatorFactory> CLSynapseSimulators;

#include <slug/sim/SynapseSimulator.hpp>

} //namespace sim

} //namespace slug
