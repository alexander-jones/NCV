#pragma once
#include <slug/sim/DataBuffer.h>
#include <slug/util/Storage.h>
#include <slug/sim/Memory.h>
#include <slug/sim/Device.h>
#ifdef SLUG_CUDA
#include <slug/sim/cuda/Extractor.h>
#endif //SLUG_CUDA

namespace slug
{

namespace sim
{

template<typename T>
class Extractor
{
public:
	Extractor(sim::Publisher* publisher);
	bool setSkip(unsigned int skip);
	bool setKey(util::Unique key);
	bool extract(typename util::Storage<T>::elementType* buffer);
	bool sync(unsigned int step, unsigned int& resultStep);
	bool deviceInit();
	unsigned int lastStep() const;
	virtual bool setIndices(const unsigned int* indices,
	                        unsigned int count) = 0;
	virtual ~Extractor();
protected:
	virtual bool _extract(typename util::Storage<T>::elementType* buffer,
	                      sim::DataBuffer::Pin& pin) = 0;
private:
	sim::Publisher* _publisher;
	sim::Publisher::Subscription* _subscription;
	unsigned int _skip;
	unsigned int _frameCount;
	sim::DataBuffer* _currentBuffer;
	util::Unique _key;
	unsigned int _lastStep;
};

template<typename T>
class CPUExtractor
	: public Extractor<T>
{
public:
	CPUExtractor(sim::Publisher* publisher);
	virtual bool setIndices(const unsigned int* indices,
	                        unsigned int count);
protected:
	virtual bool _extract(typename util::Storage<T>::elementType* buffer,
	                      sim::DataBuffer::Pin& pin);
	std::vector<unsigned int> _indices;
};

#ifdef SLUG_CUDA
template<typename T>
class CUDAExtractor
	: public Extractor<T>,
	  public Memory<MemoryType::CUDA>
{
public:
	CUDAExtractor(sim::Publisher* publisher);
	virtual bool setIndices(const unsigned int* indices,
	                        unsigned int count);
private:
	virtual bool _extract(typename util::Storage<T>::elementType* buffer,
	                      sim::DataBuffer::Pin& pin);
	unsigned int _numIndices;
	unsigned int* _indices;
	typename util::Storage<T>::elementType* _cudaBuffer;
};
#endif //SLUG_CUDA

#include <slug/sim/Extractor.hpp>

} //namespace sim

} //namespace slug
