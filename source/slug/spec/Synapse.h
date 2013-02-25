#pragma once
#include <slug/util/Unique.h>
#include <slug/spec/Element.h>
#include <slug/spec/Neuron.h>

namespace slug
{

namespace spec
{

struct Neuron;

/**
	Specification of a connector between two neurons.
*/
struct Synapse : public Element
{
	///Constructor
	Synapse(util::Unique _type,
	        unsigned int _delay = 1,
			Neuron* a = nullptr,
			Neuron* b = nullptr);

	///Connects neuron A (pre) to neuron B (post)
	bool connect(Neuron* a, Neuron* b);

	///Synaptic delay in milliseconds
	unsigned int delay;

	///Pointer to the presynaptic neuron
	Neuron* presynapticNeuron;

	///Pointer to postsynaptic neuron
	Neuron* postsynapticNeuron;
};

typedef std::vector<Synapse*> SynapseList;

} //namespace spec

} //namespace slug

