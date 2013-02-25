#pragma once
#include <slug/sim/Plugin.h>

namespace slug
{

namespace sim
{

/**
	A Factory pattern. Generates some kind of product. In general, some kind of
	simulator is the product.

	@param T The product the Factory generates
*/
template<typename T>
class Factory : public Plugin
{
public:
	/**
		Constructor

		@param __type The base type of the plugin
		@param __name The full name of the plugin
		@param __revision The version number for this instance
	*/
	Factory(const std::string& __type,
	        const std::string& __name,
			unsigned int __revision);

	/**
		Creates a T
	*/
	virtual T* generate() = 0;
};

/**
	A helper class that generates a specific type, but implements the Factory
	interface for the base type.

	@param Base The base type that Specific inherits from
	@param Specific The true type that is produced
*/
template<typename Base, typename Specific>
class SpecificFactory : public Factory<Base>
{
public:
	/**
		Constructor

		@param __type The base type of the plugin
		@param __name The full name of the plugin
		@param __revision The version number for this instance
	*/
	SpecificFactory(const std::string& __type,
	                const std::string& __name,
					unsigned int __revision);

	/**
		Creates the specific type, but returns it as the base
	*/
	virtual Base* generate();
};

#include <slug/sim/Factory.hpp>

} //namespace sim

} //namespace slug
