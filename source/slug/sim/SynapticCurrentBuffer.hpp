/*
	Implementation file for SynapticCurrentBuffer.h
	namespace slug::sim
*/

template<MemoryType MType>
SynapticCurrentBuffer<MType>::SynapticCurrentBuffer(unsigned int _numNeurons)
	: numNeurons(_numNeurons),
	  synapticCurrent(nullptr)
{
	if (numNeurons > 0)
	{
		synapticCurrent = this->template _allocate<float>(numNeurons);
	}

	_setPin(util::StringID::get("synapticCurrent"), synapticCurrent, MType);
}

template<MemoryType MType>
void SynapticCurrentBuffer<MType>::clear()
{
	this->template _zero<float>(synapticCurrent, numNeurons);
}

template<MemoryType MType>
SynapticCurrentBuffer<MType>::~SynapticCurrentBuffer()
{
	if (synapticCurrent)
		this->_deallocate(synapticCurrent);
	synapticCurrent = nullptr;
}
