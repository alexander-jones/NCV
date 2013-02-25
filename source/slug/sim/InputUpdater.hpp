/*
	Implementation file for InputUpdater.h
	namespace slug::sim
*/

template<MemoryType MType>
InputUpdater<MType>::InputUpdater(unsigned int voltageVectorSize,
             std::vector<InputSimulator<MType>*> simulators,
             unsigned int numBuffers)
	: _voltageVectorSize(voltageVectorSize),
	  _simulators(simulators),
	  _numBuffers(numBuffers)
{
	//Allocate blanks
	for (unsigned int i = 0; i < _numBuffers; ++i)
	{
		auto buffer = new InputBuffer<MType>(_voltageVectorSize);
		this->_addBlank(buffer);
	}
}

template<MemoryType MType>
bool InputUpdater<MType>::init(const SimulationProperties* properties)
{
	//Make sure simulators are ready to go
	for (auto sim : _simulators)
	{
		if (!sim->init(properties))
			return false;
	}
	return true;
}

template<MemoryType MType>
bool InputUpdater<MType>::run(const SimulationProperties* properties)
{
	Device* device = Device::device;
	std::vector<std::thread> threads;
	auto stoppingCondition = properties->stoppingCondition();
	auto synchronizer = new Synchronizer<InputBuffer<MType>>(_numBuffers);
	for (auto simulator : _simulators)
	{
		auto threadFunc = [=]()
		{
			device->threadInit();
			unsigned int step = 0;
			while (true)
			{
				auto buffer = synchronizer->pull(step);
				if (nullptr == buffer)
					break;
				simulator->update(buffer);
				if (buffer->update())
				{
					synchronizer->clear(step);
					buffer->simulationStep = step;
					this->publish(buffer);
				}
				++step;
			}
			device->threadDestroy();
			return;
		};
		threads.push_back(std::thread(threadFunc));
	}

	auto masterFunc = [=]()
	{
		unsigned int step = 0;
		while (!stoppingCondition->done(step))
		{
			auto blank = this->_getBlank();
			blank->updatesNeeded = _simulators.size();
			blank->clear();
			if (!synchronizer->push(blank))
				return;
			++step;
		}
		synchronizer->push(nullptr);
		return;
	};

	std::thread masterThread(masterFunc);
	for (auto& thread : threads)
		thread.join();
	masterThread.join();
	return true;
}

