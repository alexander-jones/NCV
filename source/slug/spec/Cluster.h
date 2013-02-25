#pragma once
#include <slug/spec/Machine.h>
#include <slug/util/Unique.h>

namespace slug
{

namespace spec
{

/**
	Represents an entire computational cluster composed of computing machines.
*/
struct Cluster
{
	///Constructor
	Cluster();

	///All machines in the cluster
	MachineList machines;

	///Machines in a map
	MachineMap machineMap;

	///The size of the neuron fire vector for the entire cluster in words
	unsigned int neuronFireVectorSize;

	/**
		Constructs a cluster from a binary file

		@param path Path to the file
		@param onlyEnabled If true, Machines and Devices that are disabled are
			left out
		@return A cluster with no elements distributed into it if successful;
			nullptr if reading failed
	*/
	static Cluster* build(const std::string& path, bool onlyEnabled = true);

	/**
		Writes the cluster to a binary file

		@param path Path to the file
		@return true if successful; false otherwise
	*/
	bool write(const std::string& path);

	///Destructor
	~Cluster();
};

} //namespace spec

} //namespace slug

