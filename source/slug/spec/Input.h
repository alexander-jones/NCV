#pragma once
#include <slug/spec/Element.h>
#include <slug/spec/Neuron.h>
#include <slug/util/Unique.h>

namespace slug
{

namespace spec
{

/**
	Specification of an input.
*/
struct Input : public Element
{
	/**
		Constructor

		@param _type The unique type ID of the input
		@param n The neuron to connect to if any
	*/
	Input(util::Unique _type, Neuron* n = nullptr);

	///Neuron this input affects
	Neuron* neuron;
	
	/**
		Connects this input to a neuron. If it is already connected to a
		neuron, returns false.
	
		@param n The neuron to connect to
		@return true if connecting was successful; false if this input is
			already connected to a neuron
	*/
	bool connect(Neuron* n);
};

} //namespace spec

} //namespace slug
