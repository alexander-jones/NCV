#pragma once
#include <slug/spec/ElementPlugin.h>
#include <slug/spec/Input.h>
#include <slug/util/Unique.h>

namespace slug
{

namespace spec
{

/**
	Specialization for an ElementPlugin that handles Inputs.
*/
struct InputPlugin : public ElementPlugin
{
	///Constructor
	InputPlugin(util::Unique _type);

	virtual util::Unique genre() const;

	///All inputs handled by this plugin
	InputList inputs;

	/**
		Adds an input to this plugin

		@param i Input to be added
		@return true if input was added; false otherwise
	*/
	bool addInput(Input* i);

	virtual unsigned int count() const;

	virtual Element* element(unsigned int index);

	virtual bool write(util::OutputStream& bs);
};

} //namespace spec

} //namespace slug

