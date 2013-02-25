#pragma once

namespace slug
{

namespace sim
{

template<typename T>
class Synchronizer
{
public:
	Synchronizer(unsigned int numBuffers);
	T* pull(unsigned int step);
	void clear(unsigned int step);
	bool push(T* buffer);
	bool failed() const;
	void fail();
	void map(std::function<void(T*)> f);
private:
	std::vector<T*> _buffers;
	std::mutex _lock;
	std::condition_variable _stateChanged;
	unsigned int _maxStep;
	bool _failed;
};

#include <slug/sim/Synchronizer.hpp>

} //namespace sim

} //namespace slug
