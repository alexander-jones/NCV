/*
	Manager.h
	Template class for all plugin manager types.
*/

#pragma once
#include <string>
#include <map>
#include <vector>
#include <slug/sim/Preference.h>
#include <algorithm>

namespace slug
{

namespace sim
{

/**
	Manages objects of a type. Selects which object type to use in case
	multiple can fulfill the same purpose (i.e. two simulators for the same
	type of neuron on the same device)

	@param T The type to manage
*/
template<class T>
class Manager
{
public:
	/**
		Adds a new plugin to the manager.

		@param t The plugin to add
		@return true if successful; false otherwise
	*/
	static bool add(T* t);

	/**
		Determines the final set of plugins to use. Should be called
		after all potentials are added.

		@return true if successful; false otherwise
	*/
	static bool initialize();

	/**
		Gets the plugin for a particular service.

		@param type The service needed (izhikevich, ncs5, etc)
		@return The corresponding plugin if it exists; nullptr otherwise
	*/
	static T* get(const std::string& type);

	/**
		Set the preference for a particular type of plugin.

		@param type The service this preference modifies
		@param p The preference object to apply
		@return true if successful; false otherwise
	*/
	static bool prefer(const std::string& type, Preference* p);

	/**
		Executes the object on each active plugin.

		@param f The functor to map
		@return true if successful; false otherwise
	*/
	template<typename F> static bool map(F& f);

private:
	///Maps the type to a list of potentials
	typedef std::map<std::string, std::vector<T*> > TListMap;

	///Maps the type of a single plugin
	typedef std::map<std::string, T*> TMap;

	///Maps the type to a set of preferences for that type
	typedef std::map<std::string, Preference*> PreferenceMap;
	
	/**
		Contains all potential plugins.
	*/
	TListMap _potential;

	/**
		Map of preferences to use to determine which plugins to actually
		select when multiple exist.
	*/
	PreferenceMap _preferences;

	/**
		Final list of plugins to use.
	*/
	TMap _final;

	/**
		Constructor. Private to prevent instantiation of more than one.
	*/
	Manager();

	/**
		Singleton accessor.

		@return The single instance of this object
	*/
	static Manager<T>* instance();

	///Singleton
	static Manager<T>* _instance;

	///add() implementation
	bool _add(T* t);

	///initialize() implementation
	bool _initialize();

	///get() implementation
	T* _get(const std::string& type);

	///prefer() implementation
	bool _prefer(const std::string& type, Preference* p);
	
	///map() implementation
	template<typename F> bool _map(F& f);

};

#include <slug/sim/Manager.hpp>

} //namespace sim

} //namespace slug


