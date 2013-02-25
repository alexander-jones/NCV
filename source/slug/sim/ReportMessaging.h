#pragma once

namespace slug
{

namespace sim
{

struct ClientMessage
{
	enum MessageType
	{
		Terminate = 0,
		Report = 1
	};
	unsigned int type;
	union MessageData
	{
		struct TerminateMessage
		{
		};
		TerminateMessage terminate;
	};
	MessageData data;
};

} //namespace sim

} //namespace slug
