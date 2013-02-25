#pragma once
#ifdef SLUG_CUDA
#include <slug/sim/SpecificDevice.h>

namespace slug
{

namespace sim
{

/**
	A Device specialization for running simulation code on CUDA-capable
	devices.
*/
class CUDADevice : public SpecificDevice<MemoryType::CUDA>
{
public:
	virtual void threadInit();

	virtual void threadDestroy();

	virtual void readHeader(util::InputStream& bs);
private:
	///The CUDA device number to give to cudaSetDevice()
	int _cudaDeviceID;
};

} //namespace sim

} //namespace slug

#endif //SLUG_CUDA
