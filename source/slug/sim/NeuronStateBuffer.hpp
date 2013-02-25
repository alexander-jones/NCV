template<MemoryType MType>
NeuronStateBuffer<MType>::NeuronStateBuffer(unsigned int _bitVectorSize, 
                                           unsigned int _voltageVectorSize)
	: bitVectorSize(_bitVectorSize),
	  voltageVectorSize(_voltageVectorSize)
{
	neuronFireVector = 
		this->template _allocate<unsigned int>(bitVectorSize);	
	neuronVoltage = this->template _allocate<float>(voltageVectorSize);

	_setPin(util::StringID::get("neuronVoltage"), neuronVoltage, MType);

	//The complete state of the neuron fire vector is not known when this
	//is emitted, so the pin is not set here, but rather in the
	//NeuronFireVectorBuffer.
}

template<MemoryType MType>
NeuronStateBuffer<MType>::~NeuronStateBuffer()
{
	this->_deallocate(neuronFireVector);
	this->_deallocate(neuronVoltage);
}

template<MemoryType MType>
void NeuronStateBuffer<MType>::clear()
{
}

