/*
	namespace slug::sim
*/

template<typename T>
SyncBuffer<T>::SyncBuffer(unsigned int _numElements)
	: numElements(_numElements)
{
	size = sizeof(unsigned int);
	dataSize = sizeof(typename util::Storage<T>::elementType) * 
	        util::Storage<T>::numElements(numElements);
	size += dataSize;
	buffer = new char[size];
	unsigned int* state = (unsigned int*)buffer;
	*state = 0;
	data = (typename util::Storage<T>::elementType*)(state + 1);
}

template<typename T>
SyncBuffer<T>::~SyncBuffer()
{
	delete [] buffer;
}

template<typename T>
void SyncBuffer<T>::clear()
{
}

template<typename T>
LocalSyncer<T>::LocalSyncer(const std::vector<ReportableInfo*> infos,
                            const std::string& key)
	: _valid(true),
	  _terminated(false)
{
	unsigned int localOffset = 0;
	for (auto info : infos)
	{
		auto reportable = Reportables::get(info->reportable, key);
		if (nullptr == reportable)
		{
			_valid = false;
			return;
		}
		Extractor<T>* extractor = nullptr;
		switch(reportable->device->memoryType())
		{
		case MemoryType::CUDA:
#ifdef SLUG_CUDA
			extractor = new CUDAExtractor<T>(reportable);
#else
			std::cerr << "Unsupported device type CUDA" << std::endl;
			continue;
#endif
		break;
		case MemoryType::System:
			extractor = new CPUExtractor<T>(reportable);
		break;
		case MemoryType::OpenCL:
		default:
			std::cerr << "Unsupported device type" << std::endl;
			continue;
		break;
		}
		extractor->setIndices(info->indices.data(), info->indices.size());
		extractor->setSkip(1);
		extractor->setKey(util::StringID::get(key));
		ExtractorData* ed = new ExtractorData();
		ed->extractor = extractor;
		ed->offset = localOffset;
		localOffset += util::Storage<T>::pad(info->indices.size());
		_extractorData.push_back(ed);
	}

	unsigned int totalElements = localOffset;
	for (unsigned int i = 0; i < 3; ++i)
	{
		auto buffer = new SyncBuffer<T>(totalElements);
		this->_addBlank(buffer);
	}
}

template<typename T>
bool LocalSyncer<T>::run()
{
	std::vector<std::thread> threads;
	auto synchronizer = new Synchronizer<SyncBuffer<T>>(3);

	for (auto ed : _extractorData)
	{
		auto threadFunc = [=]()
		{
			ed->extractor->deviceInit();
			unsigned int step = 0;

			while (true)
			{
				auto buffer = synchronizer->pull(step);
				if (nullptr == buffer)
					break;
				auto data = 
					buffer->data + util::Storage<T>::numElements(ed->offset);
				if (!ed->extractor->extract(data))
				{
					synchronizer->fail();
					break;
				}

				if (buffer->update())
				{
					synchronizer->clear(step);
					buffer->simulationStep = ed->extractor->lastStep();
					this->publish(buffer);
				}

				++step;
			}
			sim::Device::device->threadDestroy();
			return;
		};
		threads.push_back(std::thread(threadFunc));
	}

	auto masterFunc = [=]()
	{
		unsigned int step = 0;
		while (!this->_terminated)
		{
			auto blank = this->_getBlank();
			blank->updatesNeeded = _extractorData.size();
			blank->clear();

			if (!synchronizer->push(blank))
			{
				blank->release();
				return;
			}
		}
		synchronizer->fail();
		return;
	};

	std::thread masterThread(masterFunc);
	
	for (auto& thread : threads)
		thread.join();

	auto releaser = [=](SyncBuffer<T>* buffer)
	{
		buffer->release();
	};
	synchronizer->map(releaser);
	masterThread.join();
	return true;
}

template<typename T>
bool LocalSyncer<T>::sync(unsigned int target, unsigned int& result)
{
	result = target;
	for (auto ed : _extractorData)
	{
		unsigned int v = 0;
		if (!ed->extractor->sync(target, v))
			return false;
		if (v > result)
			result = v;
	}
	return true;
}

template<typename T>
bool LocalSyncer<T>::valid() const
{
	return _valid;
}

template<typename T>
bool LocalSyncer<T>::terminate()
{
	_terminated = true;
	return _terminated;
}

template<typename T>
LocalSyncer<T>::~LocalSyncer()
{
	for (auto ed : _extractorData)
	{
		delete ed->extractor;
		delete ed;
	}
	_extractorData.clear();
}

template<typename T>
SlaveNetworkSyncer<T>::SlaveNetworkSyncer(Communicator* communicator,
                                       SpecificPublisher<SyncBuffer<T>>* pub)
	: _communicator(communicator),
	  _subscription(nullptr)
{
	_subscription = pub->subscribe();
}

template<typename T>
bool SlaveNetworkSyncer<T>::run()
{
	//Until our subscription fails
	while (true)
	{
		const unsigned int good = 0;
		const unsigned int bad = 1;

		//Check if the master is terminated
		unsigned int state = good;
		MPI_Recv(&state,
		         sizeof(unsigned int),
				 MPI_CHAR,
				 0,
				 0,
				 _communicator->mpiComm,
				 nullptr);

		if (bad == state)
		{
			break;
		}

		//Get a buffer
		auto buffer = _subscription->pull();

		//If the subscription failed
		if (nullptr == buffer)
		{
			//Send the master a 1 to terminate this syncer
			state = bad;
			MPI_Send(&state,
			          sizeof(unsigned int),
					  MPI_CHAR,
					  0,
					  0,
					  _communicator->mpiComm);

			//Break out of the loop
			return true;
		}

		//Send it to the master
		//The first word of buffer is always a zero to denote a valid
		//packet
		MPI_Send(buffer->buffer, 
		         buffer->size,
				 MPI_CHAR,
				 0,
				 0,
				 _communicator->mpiComm);

		unsigned int step = buffer->simulationStep;
		MPI_Send(&step,
		         sizeof(unsigned int),
				 MPI_CHAR,
				 0,
				 0,
				 _communicator->mpiComm);

		//Release the buffer
		buffer->release();
	}
	return true;
}

template<typename T>
SlaveNetworkSyncer<T>::~SlaveNetworkSyncer()
{
	if (_subscription)
		delete _subscription;
	_subscription = nullptr;
}

template<typename T>
MasterNetworkSyncer<T>::
	MasterNetworkSyncer(const std::vector<MachineInfo*>& machines,
                        Communicator* communicator,
						SpecificPublisher<SyncBuffer<T>>* publisher)
	: _machines(machines),
	  _subscription(nullptr),
	  _communicator(communicator),
	  _terminated(false)
{
	if (publisher)
		_subscription = publisher->subscribe();
	unsigned int totalElements = 0;
	for (auto machine : _machines)
		totalElements += machine->numElements;
	for (unsigned int i = 0; i < 3; ++i)
	{
		auto blank = new SyncBuffer<T>(totalElements);
		this->_addBlank(blank);
	}
}

template<typename T>
bool MasterNetworkSyncer<T>::run()
{                    
	std::vector<std::thread> threads;
	auto synchronizer = new Synchronizer<SyncBuffer<T>>(3);
	const unsigned int good = 0;
	const unsigned int bad = 1;

	for (auto machine : _machines)
	{
		int rank = _communicator->machineRanks[machine->id];
		if (0 == rank)
		{
			auto threadFunc = [=]()
			{
				unsigned int step = 0;
				while (true)
				{
					auto buffer = synchronizer->pull(step);
					if (nullptr == buffer)
					{
						break;
					}

					auto localBuffer = _subscription->pull();
					if (nullptr == localBuffer)
					{
						synchronizer->fail();
						break;
					}

					auto data = 
						buffer->data + 
						util::Storage<T>::numElements(machine->offset);
					
					auto size = 
						util::Storage<T>::numElements(machine->numElements);

					std::copy(localBuffer->data, localBuffer->data + size, data);

					if (buffer->update())
					{
						synchronizer->clear(step);
						buffer->simulationStep = localBuffer->simulationStep;
						this->publish(buffer);
					}
					localBuffer->release();
					++step;
				}
			return;
			};
			threads.push_back(std::thread(threadFunc));
			continue;
		}

		auto threadFunc = [=]()
		{
			unsigned int step = 0;
			while (true)
			{
				auto buffer = synchronizer->pull(step);
				if (nullptr == buffer)
				{
					unsigned int state = bad;
					MPI_Send(&state,
					         sizeof(unsigned int),
							 MPI_CHAR,
							 rank,
							 0,
							 _communicator->mpiComm);
					break;
				}

				unsigned int state = 1;
				MPI_Recv(&state, 
				         sizeof(unsigned int), 
				         MPI_CHAR, 
						 rank, 
						 0, 
						 _communicator->mpiComm,
						 nullptr);

				if (1 == state)
				{
					synchronizer->fail();
					break;
				}

				auto data = 
					buffer->data + 
					util::Storage<T>::numElements(machine->offset);
				
				auto size = 
					util::Storage<T>::numElements(machine->numElements);

				MPI_Recv(data,
				         sizeof(typename util::Storage<T>::elementType) * size,
						 MPI_CHAR,
						 rank,
						 0,
						 _communicator->mpiComm,
						 nullptr);

				unsigned int simulationStep = 0;
				MPI_Recv(&simulationStep,
				         sizeof(unsigned int),
						 MPI_CHAR,
						 rank,
						 0,
						 _communicator->mpiComm,
						 nullptr);

				if (buffer->update())
				{
					synchronizer->clear(step);
					buffer->simulationStep = simulationStep;
					this->publish(buffer);
				}
				++step;
			}
			return;
		};
		threads.push_back(std::thread(threadFunc));
	}

	auto masterFunc = [=]()
	{
		unsigned int step = 0;
		while (!this->_terminated)
		{
			auto blank = this->_getBlank();
			blank->updatesNeeded = _machines.size();
			blank->clear();

			if (!synchronizer->push(blank))
				return;
		}
		synchronizer->fail();
	};

	std::thread masterThread(masterFunc);
	
	for (auto& thread : threads)
		thread.join();

	auto releaser = [=](SyncBuffer<T>* buffer)
	{
		buffer->release();
	};
	synchronizer->map(releaser);

	masterThread.join();
	return true;
}

template<typename T>
bool MasterNetworkSyncer<T>::terminate()
{
	_terminated = true;
	return _terminated;
}

template<typename T>
MasterNetworkSyncer<T>::~MasterNetworkSyncer()
{
	if (_subscription)
		delete _subscription;
	_subscription = nullptr;
	for (auto machine : _machines)
		delete machine;
	_machines.clear();
}
