#pragma once
#include <slug/util/Unique.h>
#include <slug/spec/Device.h>

namespace slug
{

namespace spec
{

/**
	A CUDA-capable device.
*/
struct CUDADevice : public Device
{
	/**
		Returns the type of device
	
		@return The type string "CUDA"
	*/
	virtual const std::string& type() const;

	/**
		Writes the specification of this device out to file

		@param bs The binary stream to write the specification to
		@return true if successful; false otherwise
	*/
	virtual bool writeSpecification(util::OutputStream& bs) const;

	///The CUDA device number for use with cudaSetDevice()
	int deviceNumber;
};

} //namespace spec

} //namespace slug

