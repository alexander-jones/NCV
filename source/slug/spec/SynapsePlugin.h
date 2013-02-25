#pragma once
#include <slug/spec/Synapse.h>
#include <slug/spec/ElementPlugin.h>
#include <slug/util/Unique.h>

namespace slug
{

namespace spec
{

/**
	Specialization for an ElementPlugin that handles Synapses.
*/
struct SynapsePlugin : public ElementPlugin
{
	///Constructor
	SynapsePlugin(util::Unique _type);

	///Returns a Unique specifying the element primary type
	virtual util::Unique genre() const;

	///All synapses handled by this plugin
	SynapseList synapses;

	///Adds a synapse to this plugin
	bool addSynapse(Synapse* s);

	/**
		The word offset to the first word of the synaptic weight vector that
		this plugin is responsible for updating.
	*/
	unsigned int synapticWeightVectorOffset;

	/**
		The word offset to the first word of the synapse fire vector that this
		plugin should read from.
	*/
	unsigned int synapseFireVectorOffset;

	virtual unsigned int count() const;

	virtual Element* element(unsigned int index);

	virtual bool write(util::OutputStream& bs);
};

} //namespace spec

} //namespace slug

