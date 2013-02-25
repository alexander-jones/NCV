/*
	FireTable.h
*/

#pragma once
#include <slug/sim/Memory.h>
#include <mutex>
#include <condition_variable>
#include <iostream>

namespace slug
{

namespace sim
{

/**
	Encapsulates the fire event table for all synapses on a particular
	device.

	@param MType The type of memory to allocate the table in
*/
template<MemoryType MType>
class FireTable : public Memory<MType>
{
public:
	/**
		Constructor

		@param vectorSize The number of unsigned ints needed to represent the
			synaptic fire vector
		@param minDelay The minimum delay in milliseconds of all synapses
		@param maxDelay The maximum delay in milliseconds of all synapses
	*/
	FireTable(unsigned int vectorSize,
	          unsigned int minDelay,
			  unsigned int maxDelay);

	/**
		Zeroes out the entire table.
	*/
	void clear();

	/**
		Returns as pointer to the head of the table.

		@return The pointer to the head of the fire table
	*/
	unsigned int* table();

	/**
		Returns a row given a timestep.

		@param index The timestep to return
		@return The pointer to the beginning of the timestep in the table
	*/
	unsigned int* row(unsigned int index);

	/**
		Return the number of rows.

		@return The total number of rows in the table
	*/
	unsigned int rows();

	/**
		Return the number of words in the synaptic fire vector.

		@return The number of 32-bit words in one synaptic fire vector
	*/
	unsigned int vectorSize();

	/**
		Releases a row back for editting.

		@param index The row index to release
	*/
	void release(unsigned int index);

	/**
		Waits for a particular row to be released. Marks the row as busy.

		@param index The index to obtain
	*/
	void wait(unsigned int index);

private:
	///The number of words in a single synaptic fire vector
	unsigned int _vectorSize;

	///The maximal synaptic delay in milliseconds
	unsigned int _maxDelay;

	///The minimal synaptic delay in milliseconds
	unsigned int _minDelay;
	
	///The number of rows in the fire table
	unsigned int _rows;

	///The entire fire table
	unsigned int* _fireTable;

	///Signifies whether a particular row is available
	bool* _freeBuffer;

	///Mutex around the free buffer
	std::mutex _freeMutex;

	///Conditional variable signaled when a row is freed
	std::condition_variable _rowFreed;

	///The total number of words in the table
	unsigned int _tableSize;
};

#include <slug/sim/FireTable.hpp>

} //namespace sim

} //namespace slug
