#pragma once
#include <string>
#include <map>
#include <slug/util/Unique.h>
#include <slug/sim/Machine.h>
#include <slug/sim/SimulationProperties.h>
#include <slug/sim/Reporter.h>

namespace slug
{

namespace sim
{

/**
	An object that can read input directories, create the necessary brain
	structures across a cluster, and then simulate them.
*/
class BrainSimulator
{
public:
	/**
		Constructor.

		@param hostname The hostname of the machine as returned by gethostname
	*/
	BrainSimulator(const std::string& hostname);

	/**
		Destructor.
	*/
	~BrainSimulator();

	/**
		Scans a directory for plugins and loads any that are found into a
		global manager. Plugins of each type should be stored in a 
		subdirectory with the appropriate name.  For example, neuron 
		plugins should be found in <path>/neurons

		@param path The base path to search
		@return true if successful; false otherwise
	*/
	static bool loadPlugins(const std::string& path);

	/**
		Finalizes the set of plugins that should be used during simulation.
		Should be called once all plugin directories have been loaded.

		@return true if successful; false otherwise
	*/
	static bool initializePlugins();

	/**
		Reads the binary brain configuration stored in the given folder.

		@param prefix The base directory of the brain configuration
		@return true if successful; false otherwise
	*/
	bool read(const std::string& prefix);

	/**
		Initializes the simulator. Simulation properties should not be modified
		after calling this.

		@param argc The number of parameters passed into the program (main)
		@param argv The array of arguments passed into the program (main)
		@return true if successful; false otherwise
	*/
	bool init(int argc, char** argv);
	
	///The size of the global neuron fire vector
	unsigned int neuronFireVectorSize;

	/**
		Creates a Communicator for a list of machine IDs

		@param ids An ordered list of machine unique IDs
		@return A newly formed Communicator with rank mappings
	*/
	struct Communicator* createCommunicator(const std::vector<util::Unique>& ids);

	/**
		Runs the simulation using data loaded with read() and the properties
		stored in its SimulationProperties.

		@return true if simulation was successful; false otherwise
	*/
	bool run();

	/**
		Returns a reference to the simulation properties that will be used
		when run() is executed.

		@return The current simulation property settings
	*/
	SimulationProperties& properties();

	/**
		Sets the simulation properties to the specified parameters.

		@param p The parameters that will be copied
		@return true if successful; false otherwise
	*/
	bool properties(SimulationProperties& p);

	/**
		Joins a cluster-wide barrier that waits until every machine in the
		cluster has joined before continuing.
	*/
	void barrier();

	/**
		Returns true if the machine this simulator is running on is the master

		@return true if this machine is the master; false otherwise
	*/
	bool master() const;

	/**
		Returns a pointer to the Reporter

		@return A pointer to a Reporter to request reports from
	*/
	Reporter* reporter();

private:
	///A map of unique IDs to their Machines
	std::map<util::Unique, Machine*> _machines;

	/**
		A pointer to the Machine descriptor for the machine this instance is
		running on
	*/
	Machine* _thisMachine;

	///The hostname of this machine
	std::string _hostname;
	
	///Properties to use to run the simulation
	SimulationProperties _properties;

	///Prefix to the directory we loaded
	std::string _prefix;

	///A pointer to the reporter on this machine
	Reporter* _reporter;

	std::thread _reporterThread;
};

} //namespace sim

} //namespace slug
