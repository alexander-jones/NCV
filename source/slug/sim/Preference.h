/*
	Preference.h
	Allows plugin preference specification with regards to which components
	to use if multiple exist to handle the same service.
*/

#pragma once
#include <slug/sim/Plugin.h>
#include <iostream>
#include <vector>

namespace slug
{

namespace sim
{

/**
	Abstract base class for any preferences.
*/
class Preference
{
public:
	/**
		Returns true if the first argument is less preferred than
		the second.
	*/
	virtual bool operator()(const Plugin* lesser, const Plugin* greater) = 0;
};

/**
	An ordered list of preferences.
*/
class PreferenceChain : public Preference
{
public:
	/**
		Returns true if the first argument is less preferred than
		the second. Chains through the list until a precedence is found.
	*/
	bool operator()(const Plugin* lesser, const Plugin* greater);

	/**
		Adds a preference to the end of the chain.
	*/
	void push_back(Preference* preference);
private:
	///List of all preferences in this chain
	std::vector<Preference*> _preferences;
};

/**
	Preference based on revision number. Generally the last thing looked at.
*/
class RevisionPreference : public Preference
{
public:
	/**
		Returns true if the first argument is less preferred than
		the second.
	*/
	virtual bool operator()(const Plugin* lesser, const Plugin* greater);
};

/**
	Preference based on whether a keyword is found within the features of
	a plugin.
*/
class FeaturePreference : public Preference
{
public:
	/**
		Creates a preference based on a keyword.

		@param key The key that gives precedence
	*/
	FeaturePreference(const std::string& key);

	/**
		Returns true if the first argument is less preferred than
		the second.
		This occurs if the second contains the keyword but the first does not.
	*/
	virtual bool operator()(const Plugin* lesser, const Plugin* greater);
private:
	///The key that gives precedence
	std::string _key;
};

} //namespace sim

} //namespace slug
