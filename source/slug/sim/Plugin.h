/*
	Plugin.h
	Abstract base class for all plugin types.
		Neuron Simulators.
		Synapse Simulators.
		Dependent Components.
		Services.
*/

#pragma once
#include <string>
#include <set>

namespace slug
{

namespace sim
{

/**
	Base interface for run-time loaded objects.
*/
class Plugin
{
public:
	/**
		Loads a directory of plugins.

		@param path The directory to load
		@return true if successful; false otherwise
	*/
	static bool loadDirectory(const std::string& path);

	/**
		Loads a single plugin.

		@param path Full path to the plugin file
		@return true if successful; false otherwise
	*/
	static bool loadPlugin(const std::string& path);

	/**
		Constructor

		@param __type The type of service offered by the plugin (i.e. ncs5)
		@param __name The full name of the plugin for differentiation purposes
		@param __revision The version of this plugin
	*/
	Plugin(const std::string& __type,
	       const std::string& __name,
		   unsigned int __revision);

	/**
		Returns the service this plugin offers.

		@return The type of service offered by the plugin
	*/
	virtual const std::string& type();

	/**
		Returns the full name of this plugin. Can be used to differentiate
		between similar plugins.

		@return The full name of the plugin
	*/
	virtual const std::string& name() const;

	/**
		Returns a revision number. Higher revision numbers are preferred over
		older revision numbers.

		@return The revision number of the plugin
	*/
	virtual unsigned int revision() const;

	/**
		Returns the set of features offered by this plugin.
	
		@return The set of features offered by the plugin
	*/
	const std::set<std::string>& features() const;
protected:
	/**
		Utility function for derived classes to add features to the feature
		set. Should be called during construction.

		@param f The feature to add
	*/
	void addFeature(const std::string& f);

private:
	///Set of all features
	std::set<std::string> _features;

	///Type of service offered
	std::string _type;

	///Full name of plugin
	std::string _name;

	///Version
	unsigned int _revision;
};

} //namespace sim

} //namespace slug
