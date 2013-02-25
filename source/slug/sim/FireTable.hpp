/**
	Implementation file for FireTable.h
	namespace slug::sim
*/

template<MemoryType MType>
FireTable<MType>::FireTable(unsigned int vectorSize,
                            unsigned int minDelay,
							unsigned int maxDelay)
	: _vectorSize(vectorSize),
	  _minDelay(minDelay),
	  _maxDelay(maxDelay),
	  _fireTable(nullptr)
{
	//We can play around with this heuristic
	_rows = _maxDelay + 5;

	//The total number of words
	_tableSize = _rows * _vectorSize;

	//Allocate the table
	if (_tableSize > 0)
		_fireTable = this->template _allocate<unsigned int>(_tableSize);

	//Allocate a bool for each row and mark those rows as all free
	_freeBuffer = new bool[_rows];
	for (unsigned int i = 0; i < _rows; ++i)
		_freeBuffer[i] = true;

	//Clear the table to initialize it
	clear();
}

template<MemoryType MType>
void FireTable<MType>::clear()
{
	//Just zero out the entire table, removing all firings
	this->template _zero<unsigned int>(_fireTable, _tableSize);
}

template<MemoryType MType>
unsigned int* FireTable<MType>::table()
{
	return _fireTable;
}

template<MemoryType MType>
unsigned int* FireTable<MType>::row(unsigned int index)
{
	return _fireTable + index * _vectorSize;
}

template<MemoryType MType>
unsigned int FireTable<MType>::rows()
{
	return _rows;
}

template<MemoryType MType>
unsigned int FireTable<MType>::vectorSize()
{
	return _vectorSize;
}

template<MemoryType MType>
void FireTable<MType>::release(unsigned int index)
{
	_freeBuffer[index] = true;
	_rowFreed.notify_all();
}

template<MemoryType MType>
void FireTable<MType>::wait(unsigned int index)
{
	while (!_freeBuffer[index])
	{
		std::unique_lock<std::mutex> lock(_freeMutex);
		//Wait for some row to free and check if it was the right one
		_rowFreed.wait(lock);
	}
	_freeBuffer[index] = false;
}
