#pragma once
#include <slug/sim/Parameterized.h>
#include <slug/sim/Manager.h>
#include <slug/sim/Factory.h>
#include <slug/sim/Memory.h>
#include <slug/util/Unique.h>
#include <slug/sim/InputBuffer.h>
#include <slug/sim/NeuronStateBuffer.h>
#include <slug/sim/SynapticWeightBuffer.h>
#include <slug/sim/SynapticFireVectorBuffer.h>
#include <slug/sim/SimulationProperties.h>
#include <algorithm>

namespace slug
{

namespace sim
{

/**
	A simulator that can update a specific type of neuron on a specific type
	of device. All neuron simulators must implement this interface.

	@param MType The memory type the simulator operates on
*/
template<MemoryType MType>
class NeuronSimulator : public Parameterized, public Memory<MType>
{
public:
	///Unique ID
	util::Unique id;
	
	///The size of the fire vector for this simulator
	unsigned int neuronFireVectorSize;

	///The offset into the device level fire vector
	unsigned int neuronFireVectorOffset;

	///The offset into any device level element array
	unsigned int neuronVoltageVectorOffset;

	///The number of neurons this simulator is responsible for
	unsigned int numNeurons;

	///Deprecated
	unsigned int* numSegments;

	///Deprecated
	unsigned int* segmentOffsets;

	///Deprecated
	unsigned int* segmentStarts;
	
	///Deprecated
	unsigned int* segmentEnds;

	/**
		Initializes the simulator after all data has been read in.

		@param properties Simulation properties for this run
		@return true if successful; false otherwise
	*/
	virtual bool init(const SimulationProperties* properties) = 0;

	/**
		Initializes the voltage for each neuron this plugin is responsible for

		@param voltages The array of voltages (aligned)
		@param properties Simulation properties for this run
		@return true if successful; false otherwise
	*/
	virtual void initVoltages(float* voltages,
	                          const SimulationProperties* properties) = 0;

	/**
		Parameters that can be used to update neurons.
	*/
	struct UpdateParameters
	{
		///Bit vector - 1 if the voltage for a neuron should be clamped
		const unsigned int* voltageClamp;

		///The clamp voltage for each neuron - only set if clamping is on
		const float* clampVoltage;

		///The total current from external inputs per neuron
		const float* inputCurrent;

		///The voltage of each neuron - update should set these values
		float* neuronVoltage;

		///Bit vector - 1 if the neuron fired during this update
		unsigned int* neuronFireVector;

		///The voltage of each neuron from the last update
		const float* previousNeuronVoltage;

		///The total current from all incoming synapses for each neuron
		float* synapticCurrentVector;
	};

	/**
		Updates neurons for a single timestep.

		@param parameters Parameters used to update as well as parameter that
			need to be updated.
	*/
	virtual void update(UpdateParameters& parameters) = 0;

	/**
		Reads the plugin file for this simulator.

		@param prefix The directory where the plugin file is stored
		@return true if successful; false otherwise
	*/
	bool read(const std::string& prefix);

	/**
		Destructor.
	*/
	virtual ~NeuronSimulator();
private:
};

typedef NeuronSimulator<MemoryType::CUDA> CUDANeuronSimulator;
typedef Factory<CUDANeuronSimulator> CUDANeuronSimulatorFactory;
typedef Manager<CUDANeuronSimulatorFactory> CUDANeuronSimulators;

typedef NeuronSimulator<MemoryType::System> CPUNeuronSimulator;
typedef Factory<CPUNeuronSimulator> CPUNeuronSimulatorFactory;
typedef Manager<CPUNeuronSimulatorFactory> CPUNeuronSimulators;

typedef NeuronSimulator<MemoryType::OpenCL> CLNeuronSimulator;
typedef Factory<CLNeuronSimulator> CLNeuronSimulatorFactory;
typedef Manager<CLNeuronSimulatorFactory> CLNeuronSimulators;

#include <slug/sim/NeuronSimulator.hpp>

} //namespace sim

} //namespace slug
