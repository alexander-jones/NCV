#pragma once
#include <slug/sim/DataBuffer.h>
#include <slug/sim/SimulationProperties.h>
#include <thread>

namespace slug
{

namespace sim
{

/**
	A wrapper around a execution pattern in which a master thread waits for
	all slave threads to complete work on a shared buffer, at which point the
	master publishes said buffer.

	@param Publication The type of buffer that is updated
*/
template<typename Publication>
class Updater : public SpecificPublisher<Publication>
{
public:
	/**
		Spawns a thread for each element to be updated against the Publication.
		Each element is initialized, updated, and published through the same
		function.

		@param Element The type of element per slave
		@param elements The set of elements to fork to slave threads
		@param threadElementInitializer The function to call when a thread is
			created
		@param updateFunction The function to call to update the publication
			using an Element
		@param publishFunction The function to call when all slaves finish
			updating for a single step
		@param threadElementDestructor The function to call when a slave thread
			terminates
		@param numBuffers The number of blanks available to the publisher
		@param properties Simulation properties for this run
		@return true if successful; false otherwise
	*/
	template<typename Element>
	bool run(std::vector<Element*>& elements, 
	         std::function<void(Element*)> threadElementInitializer,
	         std::function<void(Element*, Publication*)> updateFunction, 
			 std::function<void(Publication*)> publishFunction,
			 std::function<void(Element*)> threadElementDestructor,
			 unsigned int numBuffers,
			 const SimulationProperties* properties);

	/**
		Spawns a thread for each element to be updated against the Publication.
		Each element is initialized and updated using different functions but
		published using the same one.

		@param Element The type of element per slave
		@param elements The set of elements to fork to slave threads
		@param initializers Thread initializer for each thread
		@param updaters Update for each slave thread
		@param publishFunction The function to call when all slaves finish
			updating for a single step
		@param destructors Thread destructor for each slave thread
		@param numBuffers The number of blanks available to the publisher
		@param properties Simulation properties for this run
		@return true if successful; false otherwise
	*/
	template<typename Element>
	bool run(std::vector<Element*>& elements, 
	         std::vector<std::function<void(Element*)>> initializers,
	         std::vector<std::function<void(Element*, Publication*)>> updaters,
			 std::function<void(Publication*)> publishFunction,
			 std::vector<std::function<void(Element*)>> destructors,
			 unsigned int numBuffers,
			 const SimulationProperties* properties);
	
	/**
		Spawns a thread for each element to be updated against the Publication.
		Each element is initialized and updated using different functions but
		published using the same one.

		@param Element The type of element per slave
		@param Spargs A container for any extra data that must be collected
			to update an Element
		@param elements The set of elements to fork to slave threads
		@param initializer The function to call when a thread is
			created
		@param updater The function to call to update the publication
			using an Element
		@param publishFunction The function to call when all slaves finish
			updating for a single step
		@param destructor The function to call when a slave thread
			terminates
		@param spargsPopulator Function to call to initialize values in the 
			Sparg per time step
		@param spargsClearer Function to call to clean up values in the Sparg
			after an update step
		@param numBuffers The number of blanks available to the publisher
		@param properties Simulation properties for this run
		@return true if successful; false otherwise
	*/
	template<typename Element, typename Spargs>
	bool run(std::vector<Element*>& elements, 
	         std::function<void(Element*)> initializer,
	         std::function<void(Element*, Publication*, Spargs*)> updater,
			 std::function<void(Publication*)> publishFunction,
			 std::function<void(Element*)> destructor,
			 std::function<void(Spargs*)> spargsPopulator,
			 std::function<void(Spargs*)> spargsClearer,
			 unsigned int numBuffers,
			 const SimulationProperties* properties);

	/**
		Spawns a thread for each element to be updated against the Publication.
		Each element is initialized and updated using different functions but
		published using the same one.

		@param Element The type of element per slave
		@param Spargs A container for any extra data that must be collected
			to update an Element
		@param elements The set of elements to fork to slave threads
		@param initializers Thread initializer for each thread
		@param updaters Update for each slave thread
		@param publishFunction The function to call when all slaves finish
			updating for a single step
		@param destructors Thread destructor for each slave thread
		@param spargsPopulator Function to call to initialize values in the 
			Sparg per time step
		@param spargsClearer Function to call to clean up values in the Sparg
			after an update step
		@param numBuffers The number of blanks available to the publisher
		@param properties Simulation properties for this run
		@return true if successful; false otherwise
	*/
	template<typename Element, typename Spargs>
	bool run(std::vector<Element*>& elements, 
	         std::vector<std::function<void(Element*)>> initializers,
	         std::vector<std::function<void(Element*, 
			                                Publication*, 
											Spargs*)>> updaters,
			 std::function<void(Publication*)> publishFunction,
			 std::vector<std::function<void(Element*)>> destructors,
			 std::function<void(Spargs*)> spargsPopulator,
			 std::function<void(Spargs*)> spargsClearer,
			 unsigned int numBuffers,
			 const SimulationProperties* properties);
};

#include <slug/sim/Updater.hpp>

} //namespace sim

} //namespace slug
