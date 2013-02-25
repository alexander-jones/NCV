#pragma once
#include <slug/sim/Device.h>
#include <slug/sim/InputUpdater.h>
#include <slug/sim/NeuronUpdater.h>
#include <slug/sim/NeuronSimulator.h>
#include <slug/sim/SynapseSimulator.h>
#include <slug/sim/InputSimulator.h>
#include <slug/sim/Reportable.h>
#include <slug/sim/VectorExchanger.h>
#include <slug/sim/FireTable.h>
#include <slug/sim/FireTableUpdater.h>
#include <slug/sim/SynapseUpdater.h>

namespace slug
{

namespace sim
{

/**
	A specialized version of a device based on the type of primary memory
	it uses.

	@param MType The primary type of memory the device uses
*/
template<MemoryType MType>
class SpecificDevice : public Device 
{
public:
	/**
		Returns the primary type of memory on this device.

		@return The primary memory type of the device
	*/
	virtual MemoryType memoryType();

	/**
		Reads the device definition file in the input folder.

		@param prefix The folder the file resides in
		@return true if successful; false otherwise
	*/
	virtual bool read(const std::string& prefix);

	/**
		Initializes the device.

		@param properties Simulation properties for this run
		@return true if successful; false otherwise
	*/
	virtual bool init(const SimulationProperties* properties);

	/**
		Runs the brain simulation on this device for the requisite time steps.

		@param properties Simulation properties for this run
		@return true if successful; false otherwise
	*/
	virtual bool run(const SimulationProperties* properties);

	/**
		Sets up the thread-local device to this device.
	*/
	void setDevice();
protected:
	///Updater responsible for all input simulators
	InputUpdater<MType>* _inputUpdater;

	///Updater responsible for all neuron simulators
	NeuronUpdater<MType>* _neuronUpdater;

	///The Neuron VectorExchanger for this device
	SpecificDeviceVectorExchanger<MType>* _vectorExchanger;

	///Updater for the fire table
	FireTableUpdater<MType>* _fireTableUpdater;

	///Updater responsible for all synapses
	SynapseUpdater<MType>* _synapseUpdater;

	///All neuron simulators on this device
	std::vector<NeuronSimulator<MType>*> _neuronSimulators;

	///All synapse simulators on this device
	std::vector<SynapseSimulator<MType>*> _synapseSimulators;

	///All input simulators on this device
	std::vector<InputSimulator<MType>*> _inputSimulators;

	///System: The presynaptic neuron ID for each synapse on this device
	unsigned int* _presynapticNeuronIDs;

	///System: The synaptic delay for each synapse on this device
	unsigned int* _synapticDelays;
};

#include <slug/sim/SpecificDevice.hpp>

} //namespace sim

} //namespace slug
