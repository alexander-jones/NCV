#pragma once
#include <slug/util/Storage.h>
/*
	CUDA-specific stuff for extracting data
*/

template<typename T>
void extract(const typename slug::util::Storage<T>::elementType* d_source, 
             typename slug::util::Storage<T>::elementType* d_destination, 
			 const unsigned int* d_indices, 
			 unsigned int numIndices);
