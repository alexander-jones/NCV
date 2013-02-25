#pragma once
#include <slug/util/Unique.h>
#include <slug/sim/Memory.h>
#include <slug/util/BinaryStream.h>
#include <slug/sim/SimulationProperties.h>
#include <string>
#include <future>
#include <vector>

namespace slug
{

namespace sim
{

//Forward Declarations
class DeviceVectorExchanger;
template<MemoryType MType> class SpecificDeviceVectorExchanger;

/**
	Abstract base class for all Devices. A Device is used to run simulation
	on a Machine. Multiple devices may exist within a single Machine. A Device
	runs simulation via a number of plugins, and many threads may use a Device
	at the same time.
*/
class Device
{
public:
	///Thread-local self reference so that children know how to use the device
	static __thread Device* device;

	///A default device that does nothing
	static Device nullDevice;

	/**
		Returns the primary memory type of the device.

		@return The primary memory type of this device
	*/
	virtual MemoryType memoryType();

	/**
		Function to call to initialize a thread to use a device properly.
	*/
	virtual void threadInit();

	/**
		Function to call to destroy a thread using a device.
	*/
	virtual void threadDestroy();

	/**
		Reads the device-type-specific header.
	*/
	virtual void readHeader(util::InputStream& bs);

	/**
		Reads the device definition file in the input folder.
	*/
	virtual bool read(const std::string& prefix);

	/**
		Initializes the device and all plugins.
	*/
	virtual bool init(const SimulationProperties* properties);

	/**
		Runs the device for the requisite number of time steps.
	*/
	virtual bool run(const SimulationProperties* properties);

	///Unique ID
	util::Unique id;

	///The size of the neuron fire vector section this device handles
	unsigned int neuronFireVectorSize;

	///The offset to the first word of this device's section of the fire vector
	unsigned int neuronFireVectorOffset;

	///The VectorExchanger for this device
	DeviceVectorExchanger* vectorExchanger;

protected:
	/**
		The number of 32-bit words to store the fire bits for every synapse on
		the device.
	*/
	unsigned int _synapseFireVectorSize;

	/**
		The number of elements needed to store some synaptic variable for each
		synapse on the device.
	*/
	unsigned int _synapticVectorSize;

	/**
		The minimum synaptic delay on this device.
	*/
	unsigned int _minDelay;

	/**
		The maximum synaptic delay on this device.
	*/
	unsigned int _maxDelay;

	/**
		The number of elements needed to store some neuronal variable for each
		neuron on this device.
	*/
	unsigned int _neuronVectorSize;
};

} //namespace sim

} //namespace slug
