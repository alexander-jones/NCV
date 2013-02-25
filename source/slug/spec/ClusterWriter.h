/*
	ClusterWriter.h
	A class to generate all simulation files after distribution.
*/
#pragma once
#include <slug/spec/Components.h>

namespace slug
{

namespace spec
{

/**
	Writes out the binary data files used by the simulator later. Assumes
	that all necessary data has been distributed across the Cluster object.
*/
class ClusterWriter
{
public:
	/**
		Writes out all binary data files used by the simulator later.
		
		@param cluster Cluster with all elements distributed across it.
		@param reports The set of reports that should be produced.
		@param prefix The directory that files should be written to. It will
			be created if it doesn't exist.
		@return true if writing was successful; false otherwise
	*/
	bool write(Cluster* cluster, 
			   const std::string& prefix);
private:
	/**
		Writes out binary data for the specified machine in the folder
			prefix/machine->id

		@param machine Machine to describe
		@param prefix The base directory to write to
		@return true if writing was successful; false otherwise
	*/
	bool _write(Machine* machine, const std::string& prefix);

	/**
		Writes out binary data for the specified device in the folder
			prefix/device->id

		@param device Device to describe
		@param prefix The base directory to write to
		@return true if writing was successful; false otherwise
	*/
	bool _write(Device* device, const std::string& prefix);

	/**
		Writes out binary data for the specified plugin in the file
			prefix/plugin->id

		@param plugin Plugin to describe
		@param prefix The base directory to write the file to
		@return true if writing was successful; false otherwise
	*/
	bool _write(ElementPlugin* plugin, const std::string& prefix);

	/**
		Writes a file containing system-wide simulation information such as
		all machines participating and the data exchanging properties between
		them.

		@param cluster The cluster object containing all machines to be used
		@param path The path to the output file
		@return true if writing was successful; false otherwise
	*/
	bool _systemFile(Cluster* cluster, const std::string& path);

	/**
		Writes a file containing information regarding a single machine,
		mostly regarding how to access each individual device on it.

		@param machine The machine object with all its devices
		@param path The path to the output file
		@return true if writing was successful; false otherwise
	*/
	bool _machineFile(Machine* machine, const std::string& path);

	/**
		Writes a file containing information regarding a single device,
		mostly regarding the plugins that reside on it.

		@param device The device object with all plugins specified
		@param path The path to the output file
		@return true if writing was successful; false otherwise
	*/
	bool _deviceFile(Device* device, const std::string& path);

	/**
		Writes a file containing information regarding a single plugin,
		including all binary data needed to simulate elements for said
		plugin.

		@param plugin The plugin object along with all elements it updates
		@param path The path to the output file
		@return true if writing was successful; false otherwise
	*/
	bool _pluginFile(ElementPlugin* plugin, const std::string& path);

	/**
		Writes a text file mapping the various IDs of a neuron.

		@param cluster The entire cluster object with neurons distributed
		@param path The path to the output file
		@return true if writing was successful; false otherwise
	*/
	bool _neuronMapFile(Cluster* cluster, const std::string& path);

	/**
		Writes a text file mapping the various IDs of a synapse.

		@param cluster The entire cluster object with synapses distributed
		@param path The path to the output file
		@return true if writing was successful; false otherwise
	*/
	bool _synapseMapFile(Cluster* cluster, const std::string& path);

	/**
		Writes a text file mapping the various IDs of a input.

		@param cluster The entire cluster object with inputs distributed
		@param path The path to the output file
		@return true if writing was successful; false otherwise
	*/
	bool _inputMapFile(Cluster* cluster, const std::string& path);
}; //class ClusterWriter

} //namespace spec

} //namespace slug
