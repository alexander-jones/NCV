#pragma once

#include <mutex>
#include <condition_variable>

namespace slug
{

namespace util
{

/**
	A System-level synchronization mechanism. Any thread that joins the
	barrier will block until all participates join.
*/
class Barrier
{
public:
	/**
		Constructor.

		@param numParticipants The number of threads that must join the
			barrier before all threads can continue.
	*/
	Barrier(unsigned int numParticipants);

	/**
		Enters the barrier and waits for all other participants to join.
	*/
	void join();
private:
	///Lock for head count
	std::unique_lock<std::mutex> _lock;

	///Signaled when all threads have entered
	std::condition_variable _allPresent;

	///The total number of threads that must enter before releasing
	unsigned int _numParticipants;

	///The number of threads currently in the barrier
	unsigned int _present;
};

} //namespace util

} //namespace slug
