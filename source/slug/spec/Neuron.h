#pragma once
#include <slug/spec/Synapse.h>
#include <slug/spec/Element.h>
#include <slug/util/Unique.h>
#include <vector>

namespace slug
{

namespace spec
{

//Forward declarations
struct Synapse;
typedef std::vector<Synapse*> SynapseList;

struct Input;
typedef std::vector<Input*> InputList;

/**
	Specification of a neuron.
*/
struct Neuron : public Element
{
	///Constructor
	Neuron(util::Unique _type);

	///All synapses that contribute current to this neuron
	SynapseList incomingSynapses;

	///All inputs that affect this neuron
	InputList inputs;

	/**
		Returns the number of synapses that go into this neuron

		@return The number of synapses that contribute to this neuron
	*/
	unsigned int numIncomingSynapses() const;
};

typedef std::vector<Neuron*> NeuronList;

} //namespace spec

} //namespace slug


