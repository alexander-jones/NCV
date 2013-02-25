/*
	CUDA-specific stuff for updating the fire table.
*/

#pragma once

void updateFireTable(unsigned int timeStep,
                     unsigned int* neuronFireVector,
					 unsigned int neuronFireVectorSize,
					 unsigned int* synapseFireTable,
					 unsigned int synapticVectorSize,
					 unsigned int row,
					 unsigned int numRows,
					 unsigned int* presynapticNeuronIDs,
					 unsigned int* synapticDelays,
					 unsigned int numSynapses);

