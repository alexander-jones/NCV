#pragma once

namespace math
{

template<typename Value, typename Min, typename Max>
inline __host__ __device__ 
Value clamp(const Value& v, const Min& min, const Max& max)
{
	return v < min? min : v > max? max : v;
}

inline __host__ __device__ 
unsigned int ceiling(unsigned int v, unsigned int divisor)
{
	return (v + divisor - 1) / divisor * divisor;
}

}
