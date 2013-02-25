/**
	Distributor.h
	Handles the distribution of components across a cluster.
*/

#pragma once
#include <slug/spec/Components.h>

namespace slug
{

namespace spec
{

/**
	Distributes simulation elements across a cluster.
*/
class Distributor
{
public:
	/**
		Given a list of neurons and synapses, distributes them across the
		cluster in a hopefully load-balanced manner.

		@param _neurons List of all neurons to simulate
		@param _synapses List of all synapses to simulate
		@param cluster The simulation cluster composed of machines and devices
		@return true if distribution was successful; false otherwise
	*/
	bool distribute(NeuronList& _neurons,
	                SynapseList& _synapses,
					Cluster* cluster);
private:
};

} //namespace spec

} //namespace slug
