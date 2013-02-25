#pragma once
#include <slug/sim/Parameterized.h>
#include <slug/sim/Manager.h>
#include <slug/sim/Factory.h>
#include <slug/sim/Memory.h>
#include <slug/util/Unique.h>
#include <slug/sim/InputBuffer.h>
#include <slug/sim/SimulationProperties.h>

namespace slug
{

namespace sim
{

/**
	A simulator that can update a particular type of neuronal input. All input
	simulators must inherit from this interface.

	@param MType The primary memory type of the device this simulator can run
		on
*/
template<MemoryType MType>
class InputSimulator : public Parameterized, public Memory<MType>
{
public:
	///Unique ID
	util::Unique id;

	///The total number of inputs for this simulator
	unsigned int numInputs;

	///The device level neuron ID that each input affects - MType memory
	unsigned int* neuronIDs;

	/**
		Reads the data file for this simulator stored in the given directory
	
		@param prefix The directory to look for this simulator's data file
		@return true if successful; false otherwise
	*/
	bool read(const std::string& prefix);

	/**
		Initializes the simulator after all data files have been read in.

		@param properties Simulation properties for this run
		@return true if successful; false otherwise
	*/
	virtual bool init(const SimulationProperties* properties) = 0;

	/**
		Updates the input buffer for a single timestep.

		@param input The InputBuffer to modify
	*/
	virtual void update(InputBuffer<MType>* input) = 0;

	/**
		Destructor.
	*/
	virtual ~InputSimulator();
};

typedef InputSimulator<MemoryType::CUDA> CUDAInputSimulator;
typedef Factory<CUDAInputSimulator> CUDAInputSimulatorFactory;
typedef Manager<CUDAInputSimulatorFactory> CUDAInputSimulators;

typedef InputSimulator<MemoryType::System> CPUInputSimulator;
typedef Factory<CPUInputSimulator> CPUInputSimulatorFactory;
typedef Manager<CPUInputSimulatorFactory> CPUInputSimulators;

typedef InputSimulator<MemoryType::OpenCL> CLInputSimulator;
typedef Factory<CLInputSimulator> CLInputSimulatorFactory;
typedef Manager<CLInputSimulatorFactory> CLInputSimulators;

#include <slug/sim/InputSimulator.hpp>

} //namespace sim

} //namespace slug
