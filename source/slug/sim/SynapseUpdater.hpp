/*
	Implementation for SynapseUpdater.h
	namespace slug::sim
*/
#include <chrono>

template<MemoryType MType>
SynapseUpdater<MType>::
SynapseUpdater(unsigned int numNeurons,
               std::vector<SynapseSimulator<MType>*> simulators,
			   unsigned int numBuffers)
	: _numNeurons(numNeurons),
	  _simulators(simulators),
	  _numBuffers(numBuffers),
	  _neuronStateSubscription(nullptr),
	  _synapticStateSubscription(nullptr)
{
	//Create blanks
	for (unsigned int i = 0; i < _numBuffers; ++i)
	{
		auto buffer = new SynapticCurrentBuffer<MType>(_numNeurons);
		this->_addBlank(buffer);
	}
}

template<MemoryType MType>
bool SynapseUpdater<MType>::init(const SimulationProperties* properties)
{
	//Initialize all simulators
	for (auto sim : _simulators)
	{
		if (!sim->init(properties))
			return false;
	}

	return true;
}

template<MemoryType MType>
void SynapseUpdater<MType>::
neuronStatePublisher(NeuronStatePublisher* publisher)
{
	if (nullptr != _neuronStateSubscription)
		delete _neuronStateSubscription;
	_neuronStateSubscription = publisher->subscribe();
}

template<MemoryType MType>
void SynapseUpdater<MType>::
synapticStatePublisher(SynapticFireVectorPublisher* publisher)
{
	if (nullptr != _synapticStateSubscription)
		delete _synapticStateSubscription;
	_synapticStateSubscription = publisher->subscribe();
}

template<MemoryType MType>
bool SynapseUpdater<MType>::
run(const SimulationProperties* properties)
{
	Device* device = Device::device;
	std::vector<std::thread> threads;
	struct Reqs
	{
		NeuronStateBuffer<MType>* neuronStateBuffer;
		SynapticFireVectorBuffer<MType>* synapticFireBuffer;
		SynapticCurrentBuffer<MType>* buffer;
	};
	auto synchronizer = new Synchronizer<Reqs>(_numBuffers);
	for (auto simulator : _simulators)
	{
		auto threadFunc = [=]()
		{
			device->threadInit();
			unsigned int step = 0;
			while (true)
			{
				auto reqs = synchronizer->pull(step);
				if (nullptr == reqs)
					break;
				
				typename SynapseSimulator<MType>::UpdateParameters params;
				auto neuronStateBuffer = reqs->neuronStateBuffer;
				auto synapticFireBuffer = reqs->synapticFireBuffer;
				auto buffer = reqs->buffer;
				params.neuronFireVector = neuronStateBuffer->neuronFireVector;
				params.neuronFireVectorSize = 
					neuronStateBuffer->bitVectorSize;
				params.synapticFireVector = 
					synapticFireBuffer->synapticFireVector +
					simulator->synapticVectorOffset;
				params.synapticCurrentVector = 
					buffer->synapticCurrent;
				params.neuronVoltageVector = 
					neuronStateBuffer->neuronVoltage;
				params.buffer = buffer;
				simulator->update(params);
				if (reqs->buffer->update())
				{
					synchronizer->clear(step);
					buffer->simulationStep = step;
					neuronStateBuffer->release();
					synapticFireBuffer->release();
					this->publish(buffer);
				}
				++step;
			}
			device->threadDestroy();
			return;
		};
		threads.push_back(std::thread(threadFunc));
	}

	std::vector<Reqs*> reqs;
	reqs.resize(_numBuffers);
	for (auto& r : reqs)
		r = new Reqs();
	auto masterFunc = [=]()
	{
		Mailbox mailbox;
		unsigned int step = 0;
		while (true)
		{
			auto req = reqs[step % _numBuffers];
			std::unique_lock<std::mutex> lock(mailbox.mutex);
			_neuronStateSubscription->pull(&(req->neuronStateBuffer), 
			                               &mailbox);
			_synapticStateSubscription->pull(&(req->synapticFireBuffer),
			                                 &mailbox);
			while (nullptr == req->neuronStateBuffer ||
			       nullptr == req->synapticFireBuffer)
			{
				if (mailbox.failed)
				{
					break;
				}
				mailbox.arrival.wait(lock);
			}
			lock.unlock();
			if (mailbox.failed)
			{
				_neuronStateSubscription->cancel();
				_synapticStateSubscription->cancel();
				if (req->neuronStateBuffer)
					req->neuronStateBuffer->release();
				if (req->synapticFireBuffer)
					req->synapticFireBuffer->release();
				req->neuronStateBuffer = nullptr;
				req->synapticFireBuffer = nullptr;
				synchronizer->push(nullptr);
				break;
			}

			auto blank = this->_getBlank();
			blank->updatesNeeded = _simulators.size();
			blank->clear();
			req->buffer = blank;
			if (!synchronizer->push(req))
			{
				blank->release();
				return;
			}
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

template<MemoryType MType>
SynapseUpdater<MType>::~SynapseUpdater()
{
	delete _neuronStateSubscription;
	delete _synapticStateSubscription;
}
