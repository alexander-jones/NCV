#pragma once
#include <chrono>

namespace slug
{

namespace util
{

/**
	A simple stopwatch.
*/
class Timer
{
public:
	///Starts the timer
	void start();

	///Returns the number of seconds that has passed
	double duration();

	///Returns the number of seconds that has passed and restarts the timer
	double checkpoint();
private:
	///The last time the timer was started
	std::chrono::time_point<std::chrono::high_resolution_clock> _start;
};

} //namespace util

} //namespace slug
