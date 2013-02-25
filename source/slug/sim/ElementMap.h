#pragma once
#include <map>
#include <vector>
#include <string>
#include <slug/util/Unique.h>

namespace slug
{

namespace sim
{

class ElementMap
{
public:
	typedef unsigned int Index;
	typedef util::Unique PluginID;
	typedef util::Unique DeviceID;
	typedef util::Unique MachineID;
	struct Location
	{
		PluginID plugin;
		DeviceID device;
		MachineID machine;

		bool operator<(const Location& rhs) const
		{
			//IDs are unique across the system, so just compare the plugin
			return plugin < rhs.plugin;
		}

		bool operator==(const Location& rhs) const
		{
			return plugin == rhs.plugin;
		}
	};

	struct Offset
	{
		unsigned int device;
		unsigned int machine;
		unsigned int global;
	};

	typedef std::vector<Index> IndexList;

	bool read(const std::string& path);

	enum class Space
	{
		Plugin,
		Device,
		Machine,
		Global
	};

	bool convert(const IndexList& indices, 
	             Space dest, 
				 IndexList& result) const;
	bool getLocation(Location& location, Index index) const;
	bool getMachine(util::Unique reportable, util::Unique& machine) const;
private:
	typedef std::map<Location, IndexList> LocationToIndexMap;
	LocationToIndexMap _locationsToIndex;

	typedef std::map<Location, Offset> LocationToOffsetMap;
	LocationToOffsetMap _locationsToOffset;

	typedef std::map<Index, PluginID> IndexToPluginMap;
	IndexToPluginMap _indicesToPlugin;

	typedef std::map<Index, unsigned int> IndexToPluginOffsetMap;
	IndexToPluginOffsetMap _indicesToPluginOffset;

	typedef std::map<PluginID, Location> PluginToLocationMap;
	PluginToLocationMap _pluginToLocation;

	typedef std::map<util::Unique, util::Unique> ReportableToMachineMap;
	ReportableToMachineMap _reportableToMachine;
};

} //namespace sim

} //namespace slug
