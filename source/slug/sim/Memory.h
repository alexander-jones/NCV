/*
	Memory.h
*/
#pragma once
#include <string>

namespace slug
{

namespace sim 
{

///Differentiates between different types of memory
enum class MemoryType
{
	///Standard system memory
	System,
	///CUDA-pinned GPU memory
	CUDA,
	///OpenCL GPU memory
	OpenCL
};

///Allocates size bytes of the appropriate memory type
template<MemoryType MType>
void* allocate(size_t size)
{
	return nullptr;
}

///Allocate page-locked system memory for transfer to/from MType
template<MemoryType MType>
void* allocateHost(size_t size)
{
	return nullptr;
}

/**
	Destroys memory of the appropriate memory type; should only be used
	on memory created with allocate.
*/
template<MemoryType MType>
void deallocate(void* ptr)
{
	return;
}

template<MemoryType MType>
void zero(void* ptr, size_t size)
{
	return;
}

///Specialization for CUDA memory
template<>
void* allocate<MemoryType::CUDA>(size_t size);

template<>
void* allocateHost<MemoryType::CUDA>(size_t size);

///Specialization for CUDA memory
template<>
void deallocate<MemoryType::CUDA>(void* ptr);

///Specialization for CUDA memory
template<>
void zero<MemoryType::CUDA>(void* ptr, size_t size);

///Specialization for system memory
template<>
void* allocate<MemoryType::System>(size_t size);

template<>
void* allocateHost<MemoryType::System>(size_t size);

///Specialization for system memory
template<>
void deallocate<MemoryType::System>(void* ptr);

///Specialization for system memory
template<>
void zero<MemoryType::System>(void* ptr, size_t size);

template<MemoryType DestType, MemoryType SourceType>
bool transfer(void* dst, const void* src, size_t count)
{
	return false;
}

template<>
bool transfer<MemoryType::System, MemoryType::System>
	(void* dst, const void* src, size_t count);

template<>
bool transfer<MemoryType::CUDA, MemoryType::CUDA>
	(void* dst, const void* src, size_t count);

template<>
bool transfer<MemoryType::CUDA, MemoryType::System>
	(void* dst, const void* src, size_t count);

template<>
bool transfer<MemoryType::System, MemoryType::CUDA>
	(void* dst, const void* src, size_t count);

/**
	Inherit from this class to access allocate and deallocate utilities
	for the appropriate memory type.
*/
template<MemoryType MType>
class Memory
{
protected:
	/**
		Allocate count elements.
	*/
	template<typename T>
	T* _allocate(size_t count)
	{
		return (T*)slug::sim::allocate<MType>(sizeof(T) * count);
	}

	template<typename T>
	T* _allocateHost(size_t count)
	{
		return (T*)slug::sim::allocateHost<MType>(sizeof(T) * count);
	}

	/**
		Deallocates memory pointed to by ptr.
	*/
	void _deallocate(void* ptr)
	{
		slug::sim::deallocate<MType>(ptr);
	}

	template<MemoryType DestType, MemoryType SourceType, typename T>
	bool _transfer(T* dst, const T* src, size_t count)
	{
		return slug::sim::transfer<DestType, SourceType>
			(dst, src, count * sizeof(T));
	}

	template<typename T>
	void _zero(T* ptr, size_t size)
	{
		slug::sim::zero<MType>(ptr, sizeof(T) * size);
	}
private:
};

} //namespace sim

} //namespace slug
