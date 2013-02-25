#pragma once
#include <slug/util/Unique.h>
#include <string>
#include <map>
#include <slug/sim/DataBuffer.h>

namespace slug
{

namespace sim
{

/**
	Singleton class that manages objects that can publish data out.
*/
class Reportables
{
public:
	/**
		Associates the reportable and key pair with a given publisher.

		@param reportableName The unique name of the reportable (device ID, 
			plugin ID, etc)
		@param key The key published by the reportable
		@param publisher The actual publisher object that emits data buffers

		@return true if successful; false otherwise
	*/
	static bool set(util::Unique reportableName,
	                const std::string& key,
			        Publisher* publisher);

	/**
		Get the publisher associated with the reportable and key pair.

		@param reportableName The unique name of the reportable (device ID,
			plugin ID, etc)
		@param key The key published by the reportable
		
		@return A pointer to the associated publisher if it exists; nullptr
			otherwise
	*/
	static Publisher* get(util::Unique reportableName, const std::string& key);
private:
	///Maps keys to publishers
	typedef std::map<std::string, Publisher*> KeyPublisherMap;
	///Maps reportables to keys to publishers
	typedef std::map<util::Unique, KeyPublisherMap> ReportableMap;

	///A map of reportables to keys to publishers
	ReportableMap _reportables;

	/**
		Returns the singleton instance of this object. Creates it if it doesn't
		exist.

		@return The singleton instance
	*/
	static Reportables* instance();

	///The singleton instance
	static Reportables* _instance;

	///set() implementation
	bool _set(util::Unique reportableName, 
	          const std::string& key, 
			  Publisher* publisher);

	///get() implementation
	Publisher* _get(util::Unique reportableName, const std::string& key);
};

} //namespace sim

} //namespace slug
