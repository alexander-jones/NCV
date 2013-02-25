/**
	Template implementation file for VectorExchanger.h
	namespace slug::sim
*/

template<MemoryType MType>
SpecificDeviceVectorExchanger<MType>::
SpecificDeviceVectorExchanger(unsigned int bitVectorSize,
                              unsigned int numBuffers)
	: _numBuffers(numBuffers)
{
	//Allocate blanks 
	for (unsigned int i = 0; i < numBuffers; ++i)
	{
		auto buffer = new NeuronFireVectorBuffer<MType>(bitVectorSize);
		this->_addBlank(buffer);
	}
}

template<MemoryType MType>
bool SpecificDeviceVectorExchanger<MType>::pull(unsigned int* dst)
{
	auto* buffer = _stateSubscription->pull();
	if (nullptr == buffer)
		return false;
	this->template _transfer<MemoryType::System, MType, unsigned int>
		(dst, buffer->neuronFireVector, buffer->bitVectorSize);
	buffer->release();
	return true;
}

template<MemoryType MType>
bool SpecificDeviceVectorExchanger<MType>::
run(const SimulationProperties* properties)
{
	while (true)
	{
		auto input = _vectorSubscription->pull();
		if (nullptr == input)
			return true;
		auto output = this->_getBlank();
		this->template 
			_transfer<MType, MemoryType::System, unsigned int>
				(output->neuronFireVector, 
				 input->neuronFireVector,
				 input->bitVectorSize);
		this->publish(output);
		input->release();
	}
	return true;
}

template<MemoryType MType>
void SpecificDeviceVectorExchanger<MType>::
statePublisher(SpecificPublisher<NeuronStateBuffer<MType>>* pub)
{
	_stateSubscription = pub->subscribe();
}

template<MemoryType MType>
SpecificDeviceVectorExchanger<MType>::~SpecificDeviceVectorExchanger()
{
	if (_stateSubscription)
	{
		delete _stateSubscription;
		_stateSubscription = nullptr;
	}

	if (_vectorSubscription)
	{
		delete _vectorSubscription;
		_vectorSubscription = nullptr;
	}
}

template<MemoryType MType>
void SpecificDeviceVectorExchanger<MType>::
vectorPublisher(NeuronFireVectorBufferPublisher* pub)
{
	_vectorSubscription = pub->subscribe();
}
