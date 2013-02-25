#pragma once
#include <slug/util/Network.h>
#include <slug/sim/Reporter.h>
#include <thread>
#include <vector>

namespace slug
{

namespace sim
{

class ReportServer
{
public:
	ReportServer(Reporter* reporter);
	bool run();
	bool disconnect();
private:
	bool _connected;
	int _port;
	bool _serverFunction();
	bool _clientFunction(util::SocketConnection* connection);
	bool _reportFunction(util::SocketConnection* connection);
	std::vector<std::thread> _clientThreads;
	Reporter* _reporter;
	std::thread _serverThread;
};

} //namespace sim

} //namespace slug
