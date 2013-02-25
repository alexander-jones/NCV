#pragma once
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <slug/util/BinaryStream.h>

namespace slug
{

namespace sim
{

/**
	A helper class to generalize passing data from specification to simulation.
*/
class Parameterized
{
public:
	/**
		Associates data with a certain key

		@param name The key
		@param values The data to associate
		@return true if successful; false otherwise
	*/
	bool setParameter(const std::string& name, void* values);
	
	/**
		Indicates that a certain key must be specified in the set of
		parameters in order to pass validation. Validation occurs whenever
		validate() is called.

		@param s The key that must be present
		@return true if successful; false otherwise
	*/
	bool require(const std::string& s);

	/**
		Asserts that all required keys are present in the data set.

		@return true if all keys are present; false otherwise
	*/
	bool validate();

	/**
		Reads data from a binary stream and constructs parameter specifications
		with it.

		@param bs The binary stream to read
		@return true if successful; false otherwise
	*/
	bool read(util::InputStream& bs);

	/**
		Reads data from memory and constructs parameter specifications
		with it.

		@param _p Pointer to binary data
		@return true if successful; false otherwise
	*/
	bool read(const void* _p);

private:
	///Maps keys to parameter data
	std::map<std::string, void*> _parameters;

	///A list of keys that are required
	std::vector<std::string> _required;
protected:
	/**
		Retrieves the value associated with the given key.

		@param T The type of the value
		@param name The key to search for
		@param defaultValue The value to return if the key is not found
		@return The value associated with the key if found; defaultValue
			otherwise
	*/
	template<typename T>
	T getParameter(const std::string& name, const T& defaultValue = (T)0);

	/**
		Retrieves an array associated with the given key.
		
		@param T The type that composes the array
		@param name The key to search for
		@return A pointer to an array of T if the key is found; nullptr
			otherwise
	*/
	template<typename T>
	T* getParameterArray(const std::string& name);

	///Specialization for retrieving string values
	std::string	getParameter(const std::string& name, 
	                         const std::string& defaultValue = "");
};

#include <slug/sim/Parameterized.hpp>

} //namespace sim

} //namespace slug
