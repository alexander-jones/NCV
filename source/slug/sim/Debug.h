#pragma once
#ifdef DEBUG

#include <stdexcept>
#include <iostream>

#ifdef SLUG_CUDA
#include <cuda.h>
#include <cuda_runtime.h>
#endif //SLUG_CUDA

class Debug
{
public:
	static void lock();
	static void unlock();
};

#ifdef SLUG_CUDA

#define cudaSafeCall(X)\
{\
	X;\
	cudaError_t result = cudaDeviceSynchronize();\
	if (cudaSuccess != result)\
	{\
		std::cerr << __FILE__ << ":" <<__LINE__ << ": " << #X << " : " << cudaGetErrorString(result) << std::endl;\
		exit(0);\
		throw std::runtime_error(cudaGetErrorString(result));\
	}\
}

#define cudaPrintPointer(X)\
{\
	cudaPointerAttributes att;\
	cudaPointerGetAttributes(&att, X);\
	Debug::lock();\
	std::cerr << "Pointer: " #X " " << (unsigned long)X << " Type: " <<\
	    ((att.memoryType == cudaMemoryTypeHost)? "Host" : "Device") <<\
		" Allocated on device " << att.device << std::endl;\
	Debug::unlock();\
}

#define cudaErrorCheck()\
{\
	cudaError_t result = cudaDeviceSynchronize();\
	if (cudaSuccess != result)\
	{\
		std::cerr << __FILE__ << ":" <<__LINE__ << ": " << " : " << cudaGetErrorString(result) << std::endl;\
		exit(0);\
		throw std::runtime_error(cudaGetErrorString(result));\
	}\
}

#else //#ifndef SLUG_CUDA

#define cudaSafeCall(X) X;
#define cudaPrintPointer(X) 
#define cudaErrorCheck() ;

#endif //#ifdef SLUG_CUDA

#else //#ifndef DEBUG

#define cudaSafeCall(X) X;
#define cudaPrintPointer(X) 
#define cudaErrorCheck() ;

#endif
