template<MemoryType MType>
NeuronFireVectorBuffer<MType>::
NeuronFireVectorBuffer(unsigned int _bitVectorSize, bool lock)
	: bitVectorSize(_bitVectorSize)
{
	if (!lock)
		neuronFireVector = 
			this->template _allocate<unsigned int>(bitVectorSize);	
	else
		neuronFireVector =
			this->template _allocateHost<unsigned int>(bitVectorSize);

	_setPin(util::StringID::get("neuronFire"), neuronFireVector, MType);
}

template<MemoryType MType>
void NeuronFireVectorBuffer<MType>::clear()
{
	//Nothing happens here since everything will be overwritten anyways
}

template<MemoryType MType>
NeuronFireVectorBuffer<MType>::~NeuronFireVectorBuffer()
{
	this->_deallocate(neuronFireVector);
}

