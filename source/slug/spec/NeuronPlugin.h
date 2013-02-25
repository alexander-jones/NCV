#pragma once
#include <slug/spec/ElementPlugin.h>
#include <slug/spec/Neuron.h>

namespace slug
{

namespace spec
{

/**
	Specialization for an ElementPlugin that handles Neurons.
*/
struct NeuronPlugin : public ElementPlugin
{
	///Constructor
	NeuronPlugin(util::Unique _type);

	virtual util::Unique genre() const;

	///All neurons handled by this plugin
	NeuronList neurons;

	/**
		Adds a neuron to this plugin

		@param n Neuron to add
		@return true if adding was successful; false otherwise
	*/
	bool addNeuron(Neuron* n);

	/**
		The size of the portion of the neuron fire vector this plugin is
		responsible for updating.
	*/
	unsigned int neuronFireVectorSize;

	/**
		The word offset to the first word of the neuron fire vector that this
		plugin is responsible for updating.
	*/
	unsigned int neuronFireVectorOffset;

	/**
		The word offset to the first word of the neuron voltage vector that
		this plugin is responsible for updating.
	*/
	unsigned int neuronVoltageVectorOffset;

	virtual unsigned int count() const;

	virtual Element* element(unsigned int index);

	virtual bool write(util::OutputStream& bs);
};

} //namespace spec

} //namespace slug

