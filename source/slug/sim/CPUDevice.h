#pragma once
#include <slug/sim/SpecificDevice.h>

namespace slug
{

namespace sim
{

/**
	A Device specialization to run simulation on CPUs.
*/
class CPUDevice : public SpecificDevice<MemoryType::System>
{
public:
	virtual void threadInit();

	virtual void threadDestroy();

	virtual void readHeader(util::InputStream& bs);
private:
};

} //namespace sim

} //namespace slug

