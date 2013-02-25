#pragma once
#include <slug/spec/Device.h>
#include <slug/util/Unique.h>

namespace slug
{

namespace spec
{

/**
	A Device that uses a CPU to perform simulation.
*/
struct CPUDevice : public Device
{
	/**
		Returns the type of device

		@return The type string "CPU"
	*/
	virtual const std::string& type() const;

	/**
		Writes the specification of this device out to file
	
		@param bs The binary stream to write data out to
		@return true if successful; false otherwise
	*/
	virtual bool writeSpecification(util::OutputStream& bs) const;
};

} //namespace spec

} //namespace slug

