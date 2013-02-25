#pragma once
#include <slug/util/Network.h>
#include <slug/util/ReportRequest.h>
#include <thread>
#include <mutex>
#include <vector>

namespace slug
{

namespace sim
{

class ReportClient
{
public:
	class Report
	{
	public:
		Report(util::SocketConnection* connection);
		bool pull(std::vector<char>& data, unsigned int& step);
		bool valid() const;
		const std::vector<unsigned int>& permutation() const;

		template<typename T>
		bool permute(const std::vector<char>& data, std::vector<T>& result);
	private:
		std::vector<unsigned int> _permutation;
		util::SocketConnection* _connection;
		bool _valid;
	};

	ReportClient();
	bool connect(const std::string& server, 
	             unsigned int port,
				 float timeout);
	bool disconnect();
	bool connected() const;
	Report* report(const util::ReportRequest& request);

private:
	util::SocketConnection* _clientMessages;
	mutable std::mutex _mutex;
};

#include <slug/sim/ReportClient.hpp>

} //namespace sim

} //namespace slug
