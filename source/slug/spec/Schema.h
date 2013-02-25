#pragma once
#include <slug/util/Unique.h>
#include <slug/spec/Extractor.h>

namespace slug
{

namespace spec
{

/**
	An object that knows how to extract all necessary data from a certain type
	of Element.
*/
class Schema
{
public:
	///Default constructor
	Schema();

	///Returns the number of parameters this schema will output
	unsigned int size() const;

	/**
		Adds an Extractor to the schema. Extractors do the actual work of
		analyzing and extracting data.

		@param e Extractor to add
		@return true if the Extractor was added; false otherwise
	*/
	bool add(Extractor* e);

	/**
		Writes out all parameter data for the elements given.
	
		@param el List of Elements to analyze
		@param os The binary output stream to write data to
		@return true if successful; false otherwise
	*/
	bool write(ElementList& el, util::OutputStream& os);

	bool write(ElementList& el, std::ostream& os);

	/**
		Adds an Extractor to a Schema specified by the genre and type. Genres
		are generally the parent type such as "neuron" or "synapse" while type
		is the specific kind such as "ncs5" or "izhikevich"

		@param e Extractor to add
		@param genre Genre to add the Extractor to
		@param type Specific type within the genre to add the Extractor to
		@return true if the Extractor was added; false otherwise
	*/
	static bool add(Extractor* e, util::Unique genre, util::Unique type);

	/**
		Retrieves a Schema of a particular type within a specified genre.

		@param genre The base genre to search
		@param type The specific type within the genre to search for
		@return The Schema if found; otherwise, a blank Schema
	*/
	static Schema* get(util::Unique genre, util::Unique type);
	static Schema* get(const std::string& genre, const std::string& type);
private:
	///List of all extractors in the Schema
	ExtractorList _extractors;

	///A map from Genre to Type to Schema
	static std::map<util::Unique, std::map<util::Unique, Schema*>> _schemas;

	///A blank schema to always return if one isn't found
	static Schema _emptySchema;

	///The number of parameters this schema will generate
	unsigned int _numParameters;
};

} //namespace spec

} //namespace slug
