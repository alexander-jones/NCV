#pragma once
#include <slug/util/ReportRequest.h>
#include <slug/sim/IndexTranslator.h>
#include <mutex>
#include <thread>

namespace slug
{

namespace sim
{

class Reporter
{
public:
	Reporter(struct Communicator* communicator);
	virtual bool run() = 0;
	virtual bool request(util::ReportRequest* request) = 0;
	virtual bool shutdown() = 0;
protected:
	struct Communicator* _communicator;
};

class MasterReporter
	: public Reporter
{
public:
	MasterReporter(struct Communicator* communicator, IndexTranslator* translator);
	virtual bool run();
	virtual bool request(util::ReportRequest* request);
	virtual bool shutdown();
	~MasterReporter();
private:
	bool _active;
	std::mutex _mutex;
	bool _requestFunction(util::ReportRequest* request, struct Communicator* comm);
	IndexTranslator* _translator;
	std::vector<std::thread> _reportThreads;
};

class SlaveReporter
	: public Reporter
{
public:
	SlaveReporter(struct Communicator* communicator);
	virtual bool run();
	virtual bool request(util::ReportRequest* request);
	virtual bool shutdown();
	~SlaveReporter();
private:
	bool _requestFunction(struct Communicator* communicator);
	std::vector<std::thread> _reportThreads;
};

} //namespace sim

} //namespace slug
