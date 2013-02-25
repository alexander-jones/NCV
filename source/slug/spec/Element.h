#pragma once
#include <slug/util/Unique.h>
#include <vector>

namespace slug
{

namespace spec
{

/**
	Common structures between neurons, synapses, and inputs.
*/
struct Element
{
	///Constructor
	Element(util::Unique _type);

	/**
		A set of IDs for elements at different resolutions.
	*/

	///User-assigned ID
	unsigned int userID;
	
	///Plugin-level ID
	unsigned int pluginID;
	
	/**
		Returns the Device-level ID of this element. Value is undefined
		before distribution.

		@return the device level ID
	*/
	unsigned int deviceID() const;
	
	/**
		Returns the machine-level ID of this element. Value is undefined
		before distribution.

		@return the machine level ID
	*/
	unsigned int machineID() const;
	
	/**
		Returns the global-level ID of this element. Value is undefined
		before distribution.
	*/
	unsigned int globalID() const;
	
	/**
		Specifies the location of an element at various resolutions.
	*/
	struct Location
	{
		///The plugin this element belongs to
		struct ElementPlugin* plugin;

		bool operator<(const Location& rhs) const;

		/**
			Returns true if the input location is the same as this location.
			Compares plugin pointers.
		*/
		bool operator==(const Location& rhs) const;

		///Returns the ID of the machine the element resides on
		util::Unique machine() const;

		///Returns the ID of the device the element resides on
		util::Unique device() const;
	};

	///Location of the element in the cluster
	Location location;

	///util::Unique type of the element
	util::Unique type;
};

///A list of element pointers
typedef std::vector<Element*> ElementList;

} //namespace spec

} //namespace slug
