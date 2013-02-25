/*
	Implementation file for Updater.h
	namespace slug::sim
*/

template<typename Publication>
template<typename Element>
bool Updater<Publication>::run(std::vector<Element*>& elements, 
         std::function<void(Element*)> threadElementInitializer,
         std::function<void(Element*, Publication*)> updateFunction, 
		 std::function<void(Publication*)> publishFunction,
		 std::function<void(Element*)> threadElementDestructor,
		 unsigned int numBuffers,
		 const SimulationProperties* properties)
{
	//Replicate the same functions for each element
	std::vector<std::function<void(Element*)>> initializers;
	std::vector<std::function<void(Element*, Publication*)>> updaters;
	std::vector<std::function<void(Element*)>> destructors;
	size_t size = elements.size();
	for (size_t i = 0; i < size; ++i)
	{
		initializers.push_back(threadElementInitializer);
		updaters.push_back(updateFunction);
		destructors.push_back(threadElementDestructor);
	}

	//Then just call the version with separate functions per element
	return run(elements, 
	           initializers, 
			   updaters, 
			   publishFunction, 
			   destructors, 
			   numBuffers, 
			   properties);
}

template<typename Publication>
template<typename Element>
bool Updater<Publication>::run(std::vector<Element*>& elements, 
         std::vector<std::function<void(Element*)>> initializers,
         std::vector<std::function<void(Element*, Publication*)>> updaters,
		 std::function<void(Publication*)> publishFunction,
		 std::vector<std::function<void(Element*)>> destructors,
		 unsigned int numBuffers,
		 const SimulationProperties* properties)
{
	//Check if we have nothing to really update. If so, move data through
	//the pipe
	if (elements.empty())
	{
        auto stoppingCondition = properties->stoppingCondition();
        for (unsigned int i = 0; !stoppingCondition->done(i); ++i)
		{
			Publication* pub = this->_getBlank();
			pub->clear();
			publishFunction(pub);
		}
		return true;
	}

	//Syncs slave and master threads
	struct Synchronizer
	{
		///Array of publications that can be worked on
		Publication** buffers;

		///The step that we can simulate to due to buffers
		unsigned int step;

		///Signaled whenever the step value is incremented
		std::condition_variable stepAdvanced;

		///Shared lock between threads in this group
		std::mutex mutex;
	};

	//Create the synchronizer
	Synchronizer* synchronizer = new Synchronizer;
	synchronizer->buffers = new Publication*[numBuffers];
	synchronizer->step = 0;

	//Initialize the first blank
	unsigned int numElements = elements.size();
	synchronizer->buffers[0] = this->_getBlank();
	synchronizer->buffers[0]->clear();
	synchronizer->buffers[0]->updatesNeeded = numElements;

    auto stoppingCondition = properties->stoppingCondition();

	//For each element
	std::vector<std::thread> threads;
	for (unsigned int i = 0; i < elements.size(); ++i)
	{
		auto element = elements[i];
		auto initializer = initializers[i];
		auto updater = updaters[i];
		auto destructor = destructors[i];
		auto threadFunction = [=]()
		{
			//Init the thread
			initializer(element);

			Publication* buffer = nullptr;
			unsigned int myStep = 0;

			//For all steps
			for (unsigned int i = 0; !stoppingCondition->done(i); ++i)
			{
				//Grab a blank when available
				unsigned int slot = myStep % numBuffers;
				std::unique_lock<std::mutex> lock(synchronizer->mutex);
				if (myStep <= synchronizer->step)
					buffer = synchronizer->buffers[slot];
				else
				{
					synchronizer->stepAdvanced.wait(lock);
					buffer = synchronizer->buffers[slot];
				}
				lock.unlock();

				//Update the element
				updater(element, buffer);
				unsigned int updatesNeeded = 
					std::atomic_fetch_sub(&(buffer->updatesNeeded), 1u);

				//If we were the last one to finish updating, publish it
				if (updatesNeeded == 1)
				{
					publishFunction(buffer);
				}
				myStep++;
			}

			//Clear the thread
			destructor(element);
		};

		//Launch the thread
		threads.push_back(std::thread(threadFunction));
	}

	//Main thread loop
	for (unsigned int i = 1; !stoppingCondition->done(i); ++i)
	{
		unsigned int nextStep = synchronizer->step + 1;
		unsigned int slot = nextStep % numBuffers;

		//Wait for a blank and fill it in
		synchronizer->buffers[slot] = this->_getBlank();
		synchronizer->buffers[slot]->updatesNeeded = numElements;
		synchronizer->buffers[slot]->clear();
		std::unique_lock<std::mutex> lock(synchronizer->mutex);
		synchronizer->step = nextStep;

		//Inform any waiting threads that a new blank is ready
		synchronizer->stepAdvanced.notify_all();
	}

	//Wait for all slave threads
	for (auto& thread : threads)
		thread.join();
	
	return true;
}

template<typename Publication>
template<typename Element, typename Spargs>
bool Updater<Publication>::run(std::vector<Element*>& elements, 
                               std::function<void(Element*)> initializer,
                               std::function<void(Element*, 
		                                          Publication*, 
		                          				  Spargs*)> updater,
		                       std::function<void(Publication*)> publisher,
		                       std::function<void(Element*)> destructor,
		                       std::function<void(Spargs*)> spargsPopulator,
		                       std::function<void(Spargs*)> spargsClearer,
		                       unsigned int numBuffers,
		                       const SimulationProperties* properties)
{
	//Replicate functions per thread
	std::vector<std::function<void(Element*)>> initializers;
	std::vector<std::function<void(Element*, Publication*, Spargs*)>> updaters;
	std::vector<std::function<void(Element*)>> destructors;
	size_t size = elements.size();
	for (size_t i = 0; i < size; ++i)
	{
		initializers.push_back(initializer);
		updaters.push_back(updater);
		destructors.push_back(destructor);
	}

	//Then just call version with unique function per thread
	return Updater<Publication>::template
	    run<Element, Spargs>(elements, 
	           initializers, 
			   updaters, 
			   publisher, 
			   destructors, 
			   spargsPopulator,
			   spargsClearer,
			   numBuffers, 
			   properties);
}

template<typename Publication>
template<typename Element, typename Spargs>
bool Updater<Publication>::run(std::vector<Element*>& elements, 
         std::vector<std::function<void(Element*)>> initializers,
         std::vector<std::function<void(Element*, 
		                                Publication*,
										Spargs*)>> updaters,
		 std::function<void(Publication*)> publishFunction,
		 std::vector<std::function<void(Element*)>> destructors,
		 std::function<void(Spargs*)> spargsPopulator,
		 std::function<void(Spargs*)> spargsClearer,
		 unsigned int numBuffers,
		 const SimulationProperties* properties)
{
	//Check if we have nothing to really update. If so, move data through
	//the pipe
	if (elements.empty())
	{
        auto stoppingCondition = properties->stoppingCondition();
		auto spargFunc = [=]()
		{
			for (unsigned int i = 0; !stoppingCondition->done(i); ++i)
			{
				Spargs sparg;
				spargsPopulator(&sparg);
				spargsClearer(&sparg);
			}
		};
		std::thread spargsThread(spargFunc);

		for (unsigned int i = 0; !stoppingCondition->done(i); ++i)
		{
			Publication* pub = this->_getBlank();
			pub->clear();
			publishFunction(pub);
		}

		spargsThread.join();
		return true;
	}

	//Syncs slave and master threads
	struct Synchronizer
	{
		///Array of publications that can be worked on
		Publication** buffers;

		///Array of spargs for each blank
		Spargs* spargs;

		///The step that we can simulate to due to buffers
		unsigned int step;

		///Signaled whenever the step value is incremented
		std::condition_variable stepAdvanced;

		///Shared lock between threads in this group
		std::mutex mutex;
	};

	//Create the synchronizer
	Synchronizer* synchronizer = new Synchronizer;
	synchronizer->buffers = new Publication*[numBuffers];
	synchronizer->spargs = new Spargs[numBuffers];
	synchronizer->step = 0;

	//Initialize the first blank
	unsigned int numElements = elements.size();
	synchronizer->buffers[0] = this->_getBlank();
	synchronizer->buffers[0]->clear();
	synchronizer->buffers[0]->updatesNeeded = numElements;
	spargsPopulator(synchronizer->spargs + 0);
	
    auto stoppingCondition = properties->stoppingCondition();

	//For each element
	std::vector<std::thread> threads;
	for (unsigned int i = 0; i < elements.size(); ++i)
	{
		auto element = elements[i];
		auto initializer = initializers[i];
		auto updater = updaters[i];
		auto destructor = destructors[i];
		auto threadFunction = [=]()
		{
			//Init the thread
			initializer(element);

			Publication* buffer = nullptr;
			Spargs* sparg = nullptr;
			unsigned int myStep = 0;

			//For all steps
			for (unsigned int i = 0; !stoppingCondition->done(i); ++i)
			{
				//Grab a blank when available
				unsigned int slot = myStep % numBuffers;
				std::unique_lock<std::mutex> lock(synchronizer->mutex);
				if (myStep <= synchronizer->step)
				{
					buffer = synchronizer->buffers[slot];
					sparg = synchronizer->spargs + slot;
				}
				else
				{
					synchronizer->stepAdvanced.wait(lock);
					buffer = synchronizer->buffers[slot];
					sparg = synchronizer->spargs + slot;
				}
				lock.unlock();

				//Update the element
				updater(element, buffer, sparg);
				unsigned int updatesNeeded = 
					std::atomic_fetch_sub(&(buffer->updatesNeeded), 1u);

				//If we were the last one to finish updating, publish it
				if (updatesNeeded == 1)
				{
					publishFunction(buffer);
					spargsClearer(sparg);
				}
				myStep++;
			}

			//Clear the thread
			destructor(element);
		};

		//Launch the thread
		threads.push_back(std::thread(threadFunction));
	}

	//Main thread loop
	for (unsigned int i = 1; !stoppingCondition->done(i); ++i)
	{
		unsigned int nextStep = synchronizer->step + 1;
		unsigned int slot = nextStep % numBuffers;

		//Wait for a blank and fill it in
		synchronizer->buffers[slot] = this->_getBlank();
		synchronizer->buffers[slot]->updatesNeeded = numElements;
		synchronizer->buffers[slot]->clear();

		//Also populate the corresponding spargs
		spargsPopulator(synchronizer->spargs + slot);
		std::unique_lock<std::mutex> lock(synchronizer->mutex);
		synchronizer->step = nextStep;
		synchronizer->stepAdvanced.notify_all();
	}

	//Wait for all slave threads
	for (auto& thread : threads)
		thread.join();
	
	delete [] synchronizer->buffers;
	delete [] synchronizer->spargs;
	delete synchronizer;
	
	return true;
}
