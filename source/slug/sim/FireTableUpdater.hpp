/*
	Implementation file for FireTableUpdater.h
	namespace slug::sim
*/

#include <chrono>

template<MemoryType MType>
FireTableUpdater<MType>::
FireTableUpdater(unsigned int* presynapticNeuronIDs,
				 unsigned int* synapticDelays,
				 unsigned int synapticVectorSize,
                 unsigned int minDelay,
				 unsigned int maxDelay)
	: _minDelay(minDelay),
	  _maxDelay(maxDelay),
	  _synapticVectorSize(synapticVectorSize),
	  _subscription(nullptr)
{
	//Determine the size of a single bit vector for the synapses
	_fireVectorSize = (synapticVectorSize + 31) / 32;

	//Allocate the fire table
	_fireTable = new FireTable<MType>(_fireVectorSize, _minDelay, _maxDelay);

	if (_synapticVectorSize > 0)
	{
		//Move the pre IDs to the correct memory type
		_presynapticNeuronIDs =
			Memory<MType>::template 
				_allocate<unsigned int>(_synapticVectorSize);
		Memory<MType>::template
			_transfer<MType, MemoryType::System, unsigned int>
				(_presynapticNeuronIDs,
			 	presynapticNeuronIDs,
			 	_synapticVectorSize);
	
		//Do the same with delays
		_synapticDelays =
			Memory<MType>::template 
				_allocate<unsigned int>(_synapticVectorSize);
		Memory<MType>::template
			_transfer<MType, MemoryType::System, unsigned int>
				(_synapticDelays,
			 	synapticDelays,
			 	_synapticVectorSize);
	}
	else
	{
		_presynapticNeuronIDs = nullptr;
		_synapticDelays = nullptr;
	}

	//Allocate a blank for every row in the table
	_numBuffers = _fireTable->rows();
	for (unsigned int i = 0; i < _numBuffers; ++i)
	{
		auto blank = new SynapticFireVectorBuffer<MType>(_fireVectorSize);

		//Make sure the blank releases the row back to the fire table when all
		//readers are done
		blank->prereleaseFunction = [=]()
		{
			_fireTable->release(blank->fireTableRow);
		};

		this->_addBlank(blank);
	}
}

template<MemoryType MType>
bool FireTableUpdater<MType>::run(const SimulationProperties* properties)
{
	unsigned int numRows = _fireTable->rows();

	//The minimum row that can currently be affected by a firing
	unsigned int minRow = 0;

	//Immediately publish minDelay items
	//They'll all be blank
	for (unsigned int i = 0; i < _minDelay; ++i)
	{
		//Reserve the row
		_fireTable->wait(minRow);

		//Attach the row to a blank
		auto buffer = this->_getBlank();
		buffer->synapticFireVector = _fireTable->row(minRow);
		buffer->fireTableRow = minRow;

		//Make sure the data pin in the blank points to the right data
		buffer->updatePin();

		//Publish the row
		buffer->simulationStep = i;
		this->publish(buffer);
		
		//Increment the minimum row
		++minRow;
		if (minRow >= numRows)
			minRow -= numRows;
	}

	//The row we're currently working on
	unsigned int workRow = 0;

	//The last row that can be affected by an update
	unsigned int maxRow = minRow + (_maxDelay - _minDelay);
	maxRow %= numRows;

	unsigned int step = _minDelay;
	while (true)
	{
		//Make sure we can write all the way to the end of the table
		_fireTable->wait(maxRow);

		//zero out the latest row
		Memory<MType>::template
			_zero<unsigned int>(_fireTable->row(maxRow), _fireVectorSize);

		//Wait for a neuron fire vector
		auto neuronFireVectorBuffer = _subscription->pull();
		if (neuronFireVectorBuffer == nullptr)
			break;

		//Extract the requisite data
		auto neuronFireVector = neuronFireVectorBuffer->neuronFireVector;
		auto neuronFireVectorSize = neuronFireVectorBuffer->bitVectorSize;

		//Update the current timestep
		update(step, neuronFireVector, neuronFireVectorSize, workRow, numRows);

		//We're done with the neuron vector now
		neuronFireVectorBuffer->release();

		//Publish the row that is minDelay ahead of us
		auto buffer = this->_getBlank();
		buffer->synapticFireVector = _fireTable->row(minRow);
		buffer->fireTableRow = minRow;
		buffer->updatePin();
		buffer->simulationStep = step;
		this->publish(buffer);

		//Increment row counters
		++minRow;
		if (minRow >= numRows)
			minRow -= numRows;
		++maxRow;
		if (maxRow >= numRows)
			maxRow -= numRows;
		++workRow;
		if (workRow >= numRows)
			workRow -= numRows;
		++step;

	}
	
	return true;
}

template<MemoryType MType>
void FireTableUpdater<MType>::
neuronVectorPublisher(NeuronFireVectorPublisher* publisher)
{
	if (_subscription)
		delete _subscription;
	_subscription = publisher->subscribe();
}

template<MemoryType MType>
FireTableUpdater<MType>::~FireTableUpdater()
{
	delete _subscription;
}

template<> 
void FireTableUpdater<MemoryType::System>::
update(unsigned int timeStep,
	   unsigned int* neuronFireVector,
	   unsigned int neuronFireVectorSize,
	   unsigned int row,
	   unsigned int numRows);

template<> 
void FireTableUpdater<MemoryType::CUDA>::
update(unsigned int timeStep,
	   unsigned int* neuronFireVector,
	   unsigned int neuronFireVectorSize,
	   unsigned int row,
	   unsigned int numRows);

template<> 
void FireTableUpdater<MemoryType::OpenCL>::
update(unsigned int timeStep,
	   unsigned int* neuronFireVector,
	   unsigned int neuronFireVectorSize,
	   unsigned int row,
	   unsigned int numRows);
