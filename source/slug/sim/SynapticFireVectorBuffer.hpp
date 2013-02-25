/*
	Implementation file for SynapticFireVectorBuffer.h
	namespace slug::sim
*/

template<MemoryType MType>
SynapticFireVectorBuffer<MType>::
SynapticFireVectorBuffer(unsigned int vectorSize)
	: bitVectorSize(vectorSize)
{
	/*
		Don't do anything here. The fire vector is set on publish along with
		the release function.
	*/
	_synapseFireID = util::StringID::get("synapseFire");
}

template<MemoryType MType>
void SynapticFireVectorBuffer<MType>::updatePin()
{
	_setPin(_synapseFireID, synapticFireVector, MType);
}

template<MemoryType MType>
void SynapticFireVectorBuffer<MType>::clear()
{
}
