#pragma once
#include <slug/util/Unique.h>
#include <slug/spec/NeuronPlugin.h>
#include <slug/spec/SynapsePlugin.h>
#include <slug/spec/InputPlugin.h>

namespace slug
{

namespace spec
{

/**
	A computational device where updates on plugins occur.
*/
struct Device
{
	///Constructor
	Device();

	/**
		Specifies the location of the Device at various resolutions.
	*/
	struct Location
	{
		///The machine the device resides on
		Machine* machine;
	};

	///Location of this device in the system
	Location location;

	///util::Unique identifier for this device
	util::Unique id;

	///Maps a type of synapse to the appropriate plugin
	typedef std::map<util::Unique, SynapsePlugin*> SynapsePluginMap;
	
	///Maps a type of neuron to the appropriate plugin
	typedef std::map<util::Unique, NeuronPlugin*> NeuronPluginMap;

	///Maps a type of input to the appropriate plugin
	typedef std::map<util::Unique, InputPlugin*> InputPluginMap;

	///Map of all synapse plugins on this device
	SynapsePluginMap synapsePlugins;

	///Map of all neuron plugins on this device
	NeuronPluginMap neuronPlugins;

	///Map of all input plugins on this device
	InputPluginMap inputPlugins;

	/**
		Finalizes the set of elements on this device and creates the necessary
		plugins
	*/
	void finalize();

	///Raw list of neurons
	NeuronList neurons;
	
	///Raw list of synapses
	SynapseList synapses;

	///Raw list of inputs
	InputList inputs;

	///Returns the type of device
	virtual const std::string& type() const = 0;

	///Writes the specification of this device out to file
	virtual bool writeSpecification(util::OutputStream& bs) const = 0;

	///The relative power of this device
	double power;

	///True if this device should be used
	bool enabled;

	///The number of synapses on this device
	unsigned int numSynapses;

	///The number of neurons on this device
	unsigned int numNeurons;

	///The number of inputs on this device
	unsigned int numInputs;

	///Adds a neuron and all associated synapses and inputs to this device
	bool addNeuron(Neuron* n);

	///Adds a synapse to this device
	bool addSynapse(Synapse* s);

	///Adds an input to this device
	bool addInput(Input* i);

	/**
		The size of the portion of the neuron fire vector this device is
		responsible for updating.
	*/
	unsigned int neuronFireVectorSize;

	/**
		The word offset to the first word of the neuron fire vector that this
		device is responsible for updating.
	*/
	unsigned int neuronFireVectorOffset;

	/**
		The size of the neuron voltage vector on this device.
	*/
	unsigned int neuronVoltageVectorSize;

	/**
		The size in words of the synaptic weight vector for this device.
	*/
	unsigned int synapticVectorSize;

	/**
		The size of the synapse fire vector for this device in words.
	*/
	unsigned int synapseFireVectorSize;

	/**
		The minimum synaptic delay on this device.
	*/
	unsigned int minimumSynapticDelay;

	/**
		The maximum synaptic delay on this device.
	*/
	unsigned int maximumSynapticDelay;
	
	///Destructor
	virtual ~Device();
};

///A list of Device pointers
typedef std::vector<Device*> DeviceList;

///A map from Unique ID to Device pointer
typedef std::map<util::Unique, Device*> DeviceMap;

} //namespace spec

} //namespace slug

