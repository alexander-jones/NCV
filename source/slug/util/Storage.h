#pragma once
#include <slug/util/Bit.h>
#include <iostream>
#include <stdio.h>

namespace slug
{

namespace util
{

template<typename T>
struct Storage
{
	typedef T elementType;
	static size_t numElements(size_t count)
	{
		return count;
	}

	static elementType* allocate(size_t count)
	{
		return new elementType[numElements(count)];
	}

	static elementType access(const elementType* array, size_t index)
	{
		return array[index];
	}

	static size_t pad(size_t count)
	{
		return count;
	}
};

template<>
struct Storage<Bit>
{
	typedef unsigned int elementType;
	static size_t numElements(size_t count)
	{
		return (count + 31) / 32;
	}

	static elementType* allocate(size_t count)
	{
		return new elementType[numElements(count)];
	}

	static bool access(const elementType* array, size_t index)
	{
		unsigned int word = array[Bit::word(index)];
		unsigned int bit = Bit::bit(index);
		return (Bit::extract(word, bit) != 0);
	}

	static size_t pad(size_t count)
	{
		return numElements(count) * 32;
	}
};

} //namespace util

} //namespace slug
