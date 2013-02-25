#pragma once
#include <slug/util/Unique.h>
#include <slug/spec/Device.h>

namespace slug
{

namespace spec
{

/**
	A machine on the cluster.
*/
struct Machine
{
	///Constructor
	Machine();

	/**
		Adds a device to this machine.

		@param device The device to add
		@return true if adding was successful; false otherwise
	*/
	bool addDevice(Device* device);

	///List of all devices on this machine
	DeviceList devices;

	///Devices in a map
	DeviceMap deviceMap;

	///util::Unique ID for this machine
	util::Unique id;

	/**
		The size of the portion of the neuron fire vector this machine is
		responsible for updating.
	*/
	unsigned int neuronFireVectorSize;

	/**
		The word offset to the first word of the neuron fire vector that this
		machine is responsible for updating.
	*/
	unsigned int neuronFireVectorOffset;

	/**
		The hostname of the machine.
	*/
	std::string name;

	///True if this machine should be used
	bool enabled;

	///Destructor
	~Machine();
};

///A list of machine pointers
typedef std::vector<Machine*> MachineList;

///A map from Machine ID to Machine pointer
typedef std::map<util::Unique, Machine*> MachineMap;

} //namespace spec

} //namespace slug

