#pragma once

#include <slug/sim/NeuronFireVectorBuffer.h>
#include <slug/sim/NeuronStateBuffer.h>
#include <slug/sim/Device.h>
#include <slug/sim/Machine.h>
#include <slug/sim/DataBuffer.h>

namespace slug
{

namespace sim
{

//Forward Declaration
class Device;
class Machine;

/**
	Abstract base class for device level vector exchangers.
*/
class DeviceVectorExchanger
{
	typedef SpecificPublisher<NeuronFireVectorBuffer<MemoryType::System>>
	    NeuronFireVectorBufferPublisher;
public:
	/**
		Waits for an available fire vector and copies it into dst.

		@param dst Destination for the fire vector data
	*/
	virtual bool pull(unsigned int* dst) = 0;

	/**
		Waits for the complete vector to be available and publishes it
		to its subscribers.

		@param properties Simulation properties for this run
		@return true if successful; false otherwise
	*/
	virtual bool run(const SimulationProperties* properties) = 0;

	/**
		Sets the publisher of the complete neuron state. Generally the
		MachineVectorExchanger.

		@param pub The publisher of the fire vector
	*/
	virtual void vectorPublisher(NeuronFireVectorBufferPublisher* pub) = 0;

	virtual ~DeviceVectorExchanger();
};

/**
	Specialization of a DeviceVectorExchanger for each particular device type.
*/
template<MemoryType MType>
class SpecificDeviceVectorExchanger : 
	public SpecificPublisher<NeuronFireVectorBuffer<MType>>,
	public Memory<MType>,
	public DeviceVectorExchanger
{
	typedef SpecificPublisher<NeuronFireVectorBuffer<MemoryType::System>>
	    NeuronFireVectorBufferPublisher;
public:
	/**
		Constructor

		@param bitVectorSize The number of 32-bit words needed to store the
			bit vector
		@param numBuffers The number of blank buffers to start out with
	*/
	SpecificDeviceVectorExchanger(unsigned int bitVectorSize,
	                              unsigned int numBuffers = 5);

	/**
		Sets the publisher to wait for the neuron fire state of only this
		device.

		@param pub The publisher of the neuron state
	*/
	void statePublisher(SpecificPublisher<NeuronStateBuffer<MType>>* pub);

	/**
		Sets the publisher of the complete neuron state. Generally the
		MachineVectorExhanger.

		@param pub The publisher of the complete neuron fire state
	*/
	virtual void vectorPublisher(NeuronFireVectorBufferPublisher* pub);

	/**
		Waits for an available fire vector and copies it into dst.

		@param dst The destination of the fire vector
	*/
	virtual bool pull(unsigned int* dst);

	/**
		Waits for the complete vector to be available and publishes it
		to its subscribers.

		@param properties Simulation properties for this run
		@return true if successful; false otherwise
	*/
	virtual bool run(const SimulationProperties* properties);

	///Destructor
	~SpecificDeviceVectorExchanger();

private:
	///Subscription to partial state
	Subscription<NeuronStateBuffer<MType>>* _stateSubscription;

	///Subscription to complete state
	Subscription<NeuronFireVectorBuffer<MemoryType::System>>* 
		_vectorSubscription;

	///Number of buffers to allocate
	unsigned int _numBuffers;
};

/**
	The machine level exchanger that collects and publishes the complete
	fire state back to the machines.
*/
class MachineVectorExchanger :
	public SpecificPublisher<NeuronFireVectorBuffer<MemoryType::System>>
{
public:
	/**
		Constructor

		@param bitVectorSize The size of the completed vector
		@param devices The list of devices on this machine
		@param machines The list of all machines in the cluster
		@param thisMachine Pointer to the machine this object is on
		@param numBuffers The number of pubs that can be out at any time
	*/
	MachineVectorExchanger(unsigned int bitVectorSize,
	                       const std::vector<Device*> devices,
						   const std::vector<Machine*> machines,
						   Machine* thisMachine,
						   bool cudaPresent,
	                       unsigned int numBuffers = 3);
	
	/**
		Runs the exchanger for the duration of the simulation.

		@param properties Simulation properties for this run
		@return true if successful; false otherwise
	*/
	bool run(const SimulationProperties* properties);

	~MachineVectorExchanger();
private:
	///All devices on this machine
	std::vector<Device*> _devices;

	///Pointer to the machine running
	Machine* _thisMachine;

	///List of all machines in the cluster
	std::vector<Machine*> _machines;

	///The number of buffers to allocate
	unsigned int _numBuffers;
	
	///The number of 32-bit words in the completed vector
	unsigned int _bitVectorSize;

	/*
		A subscription to itself to hold a buffer temporarily while data is
		still being sent.
	*/
	Subscription* _subscription;
};

#include <slug/sim/VectorExchanger.hpp>

} //namespace sim

} //namespace slug

