/*
	Implementation file for SpecificDevice.h
	namespace slug::sim
*/

template<MemoryType MType>
MemoryType SpecificDevice<MType>::memoryType()
{
	return MType;
}

template<MemoryType MType>
bool SpecificDevice<MType>::read(const std::string& prefix)
{
	//Make sure the thread is using this device
	threadInit();

	//Open the device file
	std::string filename = prefix + "/" + std::to_string(id) + "/Device";
	util::InputStream bs(filename);
	if (bs.fail())
		return false;

	//Extract specific data
	bs >> _synapseFireVectorSize;
	bs >> _synapticVectorSize;
	bs >> _minDelay;
	bs >> _maxDelay;
	bs >> _neuronVectorSize;

	//Read in common synaptic data
	if (_synapticVectorSize > 0)
	{
		_presynapticNeuronIDs = new unsigned int[_synapticVectorSize];
		_synapticDelays = new unsigned int[_synapticVectorSize];
		bs.read((char*)_presynapticNeuronIDs, 
		        sizeof(unsigned int) * _synapticVectorSize);
		bs.read((char*)_synapticDelays, 
		        sizeof(unsigned int) * _synapticVectorSize);
	}
	else
	{
		_presynapticNeuronIDs = nullptr;
		_synapticDelays = nullptr;
	}

	//Launching a thread per plugin
	std::string pluginPrefix = prefix + "/" + std::to_string(id);
	std::vector<std::future<bool>> threads;

	bool success = true;

	//For each neuron plugin
	unsigned int numNeuronPlugins;
	bs >> numNeuronPlugins;
	for (unsigned int i = 0; i < numNeuronPlugins; ++i)
	{
		//Determine the neuron type
		std::string type;
		bs >> type;
		
		//Find a plugin for this type
		auto factory = Manager<Factory<NeuronSimulator<MType>>>::get(type);
		if (nullptr == factory)
		{
			std::cerr << "No plugin found for neuron of type " << type <<
				". Aborting device loading." << std::endl;
			success = false;
			break;
		}
		auto ns = factory->generate();
		_neuronSimulators.push_back(ns);

		//Read in data for this plugin
		bs >> ns->id;
		bs >> ns->neuronFireVectorSize;
		bs >> ns->neuronFireVectorOffset;
		bs >> ns->neuronVoltageVectorOffset;

		//Launch a thread to load the rest of the plugin
		auto function = [=]()
		{ 
			threadInit(); 
			bool result = ns->read(pluginPrefix);
			threadDestroy();
			return result;
		};
		threads.push_back(std::async(std::launch::async, function));
	}

	//For each synapse plugin
	unsigned int numSynapsePlugins;
	bs >> numSynapsePlugins;
	for (unsigned int i = 0; i < numSynapsePlugins && success; ++i)
	{
		//Determine the type of synapse
		std::string type;
		bs >> type;

		//Find a suitable plugin to handle this type of synapse
		auto factory = 
			Manager<Factory<SynapseSimulator<MType>>>::get(type);
		if (nullptr == factory)
		{
			std::cerr << "No plugin found for synapse of type " << type <<
				". Aborting Device loading." << std::endl;
			success = false;
			break;
		}
		auto ss = factory->generate();
		_synapseSimulators.push_back(ss);

		//Read in synapse plugin variables
		bs >> ss->id;
		bs >> ss->synapticVectorOffset;
		bs >> ss->synapseFireVectorOffset;

		//Launch a thread to finish loading this plugin
		auto function = [=]()
		{
			threadInit();
			bool result = ss->read(pluginPrefix);
			threadDestroy();
			return result;
		};
		threads.push_back(std::async(std::launch::async, function));
	}

	//For each input type
	unsigned int numInputPlugins;
	bs >> numInputPlugins;
	for (unsigned int i = 0; i < numInputPlugins && success; ++i)
	{
		//Determine the input type
		std::string type;
		bs >> type;

		//Find a plugin to handle this type
		auto factory = Manager<Factory<InputSimulator<MType>>>::get(type);
		if (nullptr == factory)
		{
			std::cerr << "No plugin found for input of type " << type <<
				". Aborting Device loading." << std::endl << std::flush;
			success = false;
			break;
		}
		auto is = factory->generate();
		_inputSimulators.push_back(is);

		//Read in input plugin information
		bs >> is->id;

		//Launch a thread to finish loading this plugin
		auto function = [=]()
		{
			threadInit();
			bool result = is->read(pluginPrefix);
			threadDestroy();
			return result;
		};
		threads.push_back(std::async(std::launch::async, function));
	}
	
	//Wait for all threads to finish
	for (auto& thread : threads)
	{
		bool result = thread.get();
		success &= result;
		if (!result)
		{
			std::cerr << "Plugin load failed. " <<
				" Aborting Device loading." << std::endl;
		}
	}

	return success;
}

template<MemoryType MType>
bool SpecificDevice<MType>::init(const SimulationProperties* properties)
{
	//Setup the input updater
	_inputUpdater = 
		new InputUpdater<MType>(_neuronVectorSize,
		                        _inputSimulators,
								3);

	//Set pins related to the input updater
	Reportables::set(id, "voltageClamp", _inputUpdater);
	Reportables::set(id, "clampVoltage", _inputUpdater);
	Reportables::set(id, "inputCurrent", _inputUpdater);

	//Determine the number of words needed for the local neuron fire vector
	unsigned int localNeuronFireVectorSize = (_neuronVectorSize + 31) / 32;

	//Set up the neuron updater
	_neuronUpdater =
		new NeuronUpdater<MType>(localNeuronFireVectorSize,
		                         _neuronVectorSize,
								 _neuronSimulators,
								 3);

	//Set pins related to the neuron updater
	Reportables::set(id, "neuronVoltage", _neuronUpdater);

	//Setup the vector exchanger
	_vectorExchanger =
		new SpecificDeviceVectorExchanger<MType>
			(properties->neuronFireVectorSize);

	vectorExchanger = _vectorExchanger;

	//Setup the fire table updater
	_fireTableUpdater = new FireTableUpdater<MType>(_presynapticNeuronIDs,
	                                                _synapticDelays,
													_synapticVectorSize,
													_minDelay,
													_maxDelay);

	Reportables::set(id, "synapseFire", _fireTableUpdater);

	//Setup the synaptic updater
	_synapseUpdater = new SynapseUpdater<MType>(_neuronVectorSize,
	                                            _synapseSimulators,
												3);

	Reportables::set(id, "synapticCurrent", _synapseUpdater);
	
	//Link all pieces that need initial states from each other
	_neuronUpdater->synapticCurrentPublisher(_synapseUpdater);
	_synapseUpdater->neuronStatePublisher(_neuronUpdater);

	bool result = true;
	//Initialize all updaters, causing initial states to also publish
	result &= _inputUpdater->init(properties);
	result &= _neuronUpdater->init(properties);
	result &= _synapseUpdater->init(properties);

	//Finish linking the rest of the pieces that don't need initial states
	_neuronUpdater->inputPublisher(_inputUpdater);
	_vectorExchanger->statePublisher(_neuronUpdater);
    _fireTableUpdater->neuronVectorPublisher(_vectorExchanger);
	_synapseUpdater->synapticStatePublisher(_fireTableUpdater);

	return result;
};

template<MemoryType MType>
bool SpecificDevice<MType>::run(const SimulationProperties* properties)
{
	setDevice();
	std::vector<std::future<bool>> threads;

	//start the input updater
	auto inputUpdaterFunc = [=]()
	{
		this->setDevice();
		threadInit();
		bool success = _inputUpdater->run(properties);
		threadDestroy();
		std::cout << "InputUpdater done" << std::endl;
		delete _inputUpdater;
		std::cout << "InputUpdater deleted" << std::endl;
		return success;
	};
	threads.push_back(std::async(std::launch::async, inputUpdaterFunc));

	//Start the neuron updater
	auto neuronUpdaterFunc = [=]()
	{
		this->setDevice();
		threadInit();
		bool success = _neuronUpdater->run(properties);
		threadDestroy();
		std::cout << "NeuronUpdater done" << std::endl;
		delete _neuronUpdater;
		std::cout << "NeuronUpdater deleted" << std::endl;
		return success;
	};
	threads.push_back(std::async(std::launch::async, neuronUpdaterFunc));

	//Start the table updater
	auto tableUpdaterFunc = [=]()
	{
		this->setDevice();
		threadInit();
		bool success = _fireTableUpdater->run(properties);
		threadDestroy();
		std::cout << "FireTableUpdater done" << std::endl;
		delete _fireTableUpdater;
		std::cout << "FireTableUpdater deleted" << std::endl;
		return success;
	};
	threads.push_back(std::async(std::launch::async, tableUpdaterFunc));

	//Start the synapse updater
	auto synapseUpdaterFunc = [=]()
	{
		this->setDevice();
		threadInit();
		bool success = _synapseUpdater->run(properties);
		threadDestroy();
		std::cout << "SynapseUpdater done" << std::endl;
		delete _synapseUpdater;
		std::cout << "SynapseUpdater deleted" << std::endl;
		return success;
	};
	threads.push_back(std::async(std::launch::async, synapseUpdaterFunc));

	bool success = true;
	for (auto &thread : threads)
		success &= thread.get();
	return success;
};

template<MemoryType MType>
void SpecificDevice<MType>::setDevice()
{
	Device::device = this;
}
