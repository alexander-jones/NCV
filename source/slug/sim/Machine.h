#pragma once
#include <slug/util/Unique.h>
#include <string>
#include <map>
#include <slug/sim/Device.h>
#include <slug/sim/SimulationProperties.h>
#include <slug/sim/VectorExchanger.h>

namespace slug
{

namespace sim
{

//Forward Declarations
class MachineVectorExchanger;

/**
	Simulation-time encapsulation of a Machine. Machines are responsible for
	a number of devices under them and are also responsible for inter-machine
	communication across the cluster.
*/
class Machine
{
public:
	/**
		Reads the input for this machine given the prefix

		@param prefix The folder which will contain the machine file
		@return true if successful; false otherwise
	*/
	bool read(const std::string& prefix);

	/**
		Initializes the machine and all devices

		@param properties Simulation properties for this run
		@return true if successful; false otherwise
	*/
	bool init(const SimulationProperties* properties);

	/**
		Executes the simulation on this machine for the requisite number of
		steps.

		@param properties Simulation properties for this run
		@return true if successful; false otherwise
	*/
	bool run(const SimulationProperties* properties);

	///The unique ID of this machine
	util::Unique id;

	///The hostname of this machine
	std::string name;

	///The size of the neuron fire vector for all neurons on this machine
	unsigned int neuronFireVectorSize;

	///The offset into the global neuron fire vector for this machine
	unsigned int neuronFireVectorOffset;

	///The MPI Rank for this machine
	int mpiRank;

	///All machines in the system
	std::vector<Machine*> machines;
private:
	///Set of all devices on this machine
	std::map<util::Unique, Device*> _devices;

	///Cluster-wide vector exchanger
	MachineVectorExchanger* _vectorExchanger;

	bool _cudaPresent;
};

///A list of Machine pointers
typedef std::vector<Machine*> MachineList;

} //namespace sim

} //namespace slug
