#pragma once
#ifdef SLUG_CUDA
#include <cuda_runtime.h>

namespace slug
{

namespace sim
{

/**
	Utility functions and data when using CUDA.
*/
class CUDA
{
public:
	///A thread-local variable for the stream that is generated per thread
	static __thread cudaStream_t stream;

	///A wrapper to make sure that all CUDA commands on the stream are finished
	static void synchronize();
};

} //namespace sim

} //namespace slug

#endif //SLUG_CUDA
