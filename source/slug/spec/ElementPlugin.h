#pragma once
#include <slug/util/Unique.h>
#include <slug/spec/Element.h>
#include <slug/util/Specification.h>
#include <slug/util/BinaryStream.h>

namespace slug
{

namespace spec
{

struct Machine;
struct Device;

/**
	Specification for a plugin that handles a particular type of element.
*/
struct ElementPlugin
{
	///Constructor
	ElementPlugin(util::Unique _type);

	///Returns a Unique specifying the element primary type
	virtual util::Unique genre() const = 0;

	///The exact type this plugin handles
	util::Unique type;

	///Global plugin specification
	util::Specification* specification;

	///A unique identifier for this plugin
	util::Unique id;

	///Base offsets on every level
	struct Offset
	{
		///Global base ID
		unsigned int global;

		///Machine base ID
		unsigned int machine;

		///Device base ID
		unsigned int device;
	};

	///Set of base IDs for elements in this plugin
	Offset offset;

	/**
		Specifies the location of the plugin at various resolutions.
	*/
	struct Location
	{
		///Device level location
		Device* device;

		///Machine level location
		Machine* machine;
	};

	///Location of this plugin in the system
	Location location;

	///Gets the number of elements
	virtual unsigned int count() const = 0;

	///Gets the element at an index
	virtual Element* element(unsigned int index) = 0;

	///Writes out data specific to this type of plugin
	virtual bool write(util::OutputStream& bs) = 0;
};


} //namespace spec

} //namespace slug

