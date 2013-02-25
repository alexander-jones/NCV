#pragma once
#include <slug/util/Storage.h>
#include <slug/sim/Reportable.h>
#include <slug/sim/Extractor.h>
#include <slug/sim/Device.h>
#include <slug/sim/Communicator.h>
#include <slug/sim/Synchronizer.h>
#include <slug/sim/DataBuffer.h>

namespace slug
{

namespace sim
{

template<typename T>
struct SyncBuffer
	: public DataBuffer
{
	SyncBuffer(unsigned int _numElements);

	~SyncBuffer();

	unsigned int numElements;
	
	typename util::Storage<T>::elementType* data;
	unsigned int dataSize;

	char* buffer;

	unsigned int size;

	void clear();
};

template<typename T>
class LocalSyncer
	: public SpecificPublisher<SyncBuffer<T>>
{
public:
	struct ReportableInfo
	{
		util::Unique reportable;
		std::vector<unsigned int> indices;
	};

	LocalSyncer(const std::vector<ReportableInfo*> info,
	            const std::string& key);

	bool run();
	bool sync(unsigned int target, unsigned int& result);
	bool valid() const;
	bool terminate();

	~LocalSyncer();
private:
	struct ExtractorData
	{
		Extractor<T>* extractor;
		unsigned int offset;
	};
	std::vector<ExtractorData*> _extractorData;
	bool _valid;
	bool _terminated;
};

template<typename T>
class SlaveNetworkSyncer
{
public:
	SlaveNetworkSyncer(Communicator* communicator,
	                   SpecificPublisher<SyncBuffer<T>>* publisher);

	bool run();

	~SlaveNetworkSyncer();
private:
	Communicator* _communicator;
	Subscription<SyncBuffer<T>>* _subscription;
};

template<typename T>
class MasterNetworkSyncer
	: public SpecificPublisher<SyncBuffer<T>>
{
public:
	struct MachineInfo
	{
		util::Unique id;
		unsigned int numElements;
		unsigned int offset;
	};
	MasterNetworkSyncer(const std::vector<MachineInfo*>& machines,
	                    Communicator* communicator,
	                    SpecificPublisher<SyncBuffer<T>>* publisher);

	bool run();
	bool terminate();

	virtual ~MasterNetworkSyncer();
private:
	bool _terminated;
	Communicator* _communicator;
	Subscription<SyncBuffer<T>>* _subscription;
	std::vector<MachineInfo*> _machines;
};

#include <slug/sim/ReportSyncer.hpp>

} //namespace sim

} //namespace slug

