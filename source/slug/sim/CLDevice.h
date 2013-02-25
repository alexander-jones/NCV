#pragma once
#include <slug/sim/SpecificDevice.h>

namespace slug
{

namespace sim
{

/**
	A Device that executes simulation on a OpenCL capable device.

	TODO: While the hooks are here, the setup code is not yet implemented
*/
class CLDevice : public SpecificDevice<MemoryType::OpenCL>
{
public:
	virtual void threadInit();

	virtual void threadDestroy();

	virtual void readHeader(util::InputStream& bs);
private:
};

} //namespace sim

} //namespace slug
