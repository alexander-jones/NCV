/*
	Implementation for SynapticWeightBuffer.h
	namespace slug::sim
*/

template<MemoryType MType>
SynapticWeightBuffer<MType>::
SynapticWeightBuffer(unsigned int vectorSize)
	: numElements(vectorSize),
	  synapticWeightVector(nullptr)
{
	if (numElements > 0)
	{
		synapticWeightVector =
			Memory<MType>::template _allocate<float>(numElements);
	}
}

template<MemoryType MType>
void SynapticWeightBuffer<MType>::clear()
{
}
