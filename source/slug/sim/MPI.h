#pragma once

namespace slug
{

namespace sim
{

/**
	Access control to MPI for cases where MPI is not thread-safe.
*/
class MPI
{
public:
	/**
		Obtain control of MPI. Only one thread can have access at a time.
	*/
	static void lock();

	/**
		Release control of MPI.
	*/
	static void unlock();
};

} //namespace sim

} //namespace slug
