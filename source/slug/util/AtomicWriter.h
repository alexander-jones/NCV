#pragma once

namespace slug
{

namespace util
{

template<typename T>
class AtomicWriter
{
public:
	AtomicWriter();
	void write(T* location, const T& value);
	void commit(std::function<void(T*,const T&)> op);
private:
	std::vector<T*> _locations;
	std::vector<T> _values;
};

#include <slug/util/AtomicWriter.hpp>

} //namespace util

} //namespace slug
