/*
	Strings.h
	String to ID Manager.
*/

#pragma once
#include <string>
#include <map>
#include <mutex>
#include <slug/util/Unique.h>

namespace slug
{

namespace util
{

/**
	String to ID Manager.
*/
class StringID
{
public:
	/**
		Returns a unique value for the given string. Subsequent calls with the
		same input return the same value.

		@param s String to lookup
		@return The Unique value corresponding to that string
	*/
	static Unique get(const std::string& s);
	
	/**
		Returns the string represented by the input ID.

		@param id The Unique value to lookup
		@return The string associated with id
	*/
	static const std::string& get(Unique id);

private:
	///Maps strings to Uniques
	static std::map<std::string, Unique> _ids;

	///Maps Uniques to strings
	static std::map<Unique, std::string> _strings;
	
	///Lock around modifying the maps
	static std::mutex _lock;
};

} //namespace util

} //namespace slug
