/*
	Implementation file for NeuronUpdater.h
	namespace slug::sim
*/
#include <chrono>

template<MemoryType MType>
NeuronUpdater<MType>::
    NeuronUpdater(unsigned int bitVectorSize,
                  unsigned int voltageVectorSize,
                  std::vector<NeuronSimulator<MType>*> simulators,
			      unsigned int numBuffers)
	: _bitVectorSize(bitVectorSize),
	  _voltageVectorSize(voltageVectorSize),
	  _simulators(simulators),
	  _numBuffers(numBuffers),
	  _inputBufferSubscription(nullptr),
	  _synapticCurrentSubscription(nullptr)
{
	//Create blanks for publishing
	for (unsigned int i = 0; i < _numBuffers; ++i)
	{
		auto buffer = 
			new NeuronStateBuffer<MType>(_bitVectorSize, 
			                             _voltageVectorSize);
		this->_addBlank(buffer);
	}

	//We need the previous state from ourself
	_neuronStateSubscription = this->subscribe();
}

template<MemoryType MType>
void NeuronUpdater<MType>::
inputPublisher(SpecificPublisher<InputBuffer<MType>>* publisher)
{
	if (_inputBufferSubscription)
		delete _inputBufferSubscription;
	_inputBufferSubscription = publisher->subscribe();
}

template<MemoryType MType>
void NeuronUpdater<MType>::
synapticCurrentPublisher(SynapticCurrentVectorPublisher* publisher)
{
	if (_synapticCurrentSubscription)
		delete _synapticCurrentSubscription;
	_synapticCurrentSubscription = publisher->subscribe();
}

template<MemoryType MType>
bool NeuronUpdater<MType>::init(const SimulationProperties* properties)
{
	//Init all the simulators
	for (auto sim : _simulators)
	{
		if (!sim->init(properties))
			return false;
	}

	//Get the initial set of voltages
	auto buffer = this->_getBlank();
	for (auto sim : _simulators)
	{
		auto voltages = buffer->neuronVoltage + sim->neuronVoltageVectorOffset;
		sim->initVoltages(voltages, properties);
	}

	//Publish it, allowing us to get the previous state
	this->publish(buffer);

	return true;
}

template<MemoryType MType>
bool NeuronUpdater<MType>::run(const SimulationProperties* properties)
{
	//Set up threads
	Device* device = Device::device;
	std::vector<std::thread> threads;

	struct Reqs
	{
		InputBuffer<MType>* inputBuffer;
		NeuronStateBuffer<MType>* previousNeuronStateBuffer;
		SynapticCurrentBuffer<MType>* synapticCurrentBuffer;
		NeuronStateBuffer<MType>* buffer;
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
				typename NeuronSimulator<MType>::UpdateParameters parameters;

				//Extract data and offset accordingly
				unsigned int vectorOffset = 
					simulator->neuronVoltageVectorOffset;
				unsigned int bitOffset = 
					simulator->neuronFireVectorOffset;

				auto inputBuffer = reqs->inputBuffer;
				auto buffer = reqs->buffer;
				auto previousNeuronStateBuffer = 
					reqs->previousNeuronStateBuffer;
				auto synapticCurrentBuffer = reqs->synapticCurrentBuffer;
				parameters.voltageClamp = 
					inputBuffer->voltageClamp + bitOffset;
				parameters.clampVoltage = 
					inputBuffer->clampVoltage + vectorOffset;
				parameters.inputCurrent = 
					inputBuffer->inputCurrent + vectorOffset; 
				parameters.neuronVoltage = 
					buffer->neuronVoltage + vectorOffset;
				parameters.neuronFireVector = 
					buffer->neuronFireVector + vectorOffset;
				parameters.previousNeuronVoltage =
					previousNeuronStateBuffer->neuronVoltage + 
					vectorOffset; 
				parameters.synapticCurrentVector =
					synapticCurrentBuffer->synapticCurrent + 
					vectorOffset;

				simulator->update(parameters);
				if (reqs->buffer->update())
				{
					synchronizer->clear(step);
					buffer->simulationStep = step;
					inputBuffer->release();
					previousNeuronStateBuffer->release();
					synapticCurrentBuffer->release();
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
			_inputBufferSubscription->pull(&(req->inputBuffer), &mailbox);
			_neuronStateSubscription->pull(&(req->previousNeuronStateBuffer), 
			                               &mailbox);
			_synapticCurrentSubscription->pull(&(req->synapticCurrentBuffer),
			                                   &mailbox);
			while (nullptr == req->inputBuffer ||
			       nullptr == req->previousNeuronStateBuffer ||
				   nullptr == req->synapticCurrentBuffer)
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
				_inputBufferSubscription->cancel();
				_neuronStateSubscription->cancel();
				_synapticCurrentSubscription->cancel();
				if (req->inputBuffer)
					req->inputBuffer->release();
				if (req->previousNeuronStateBuffer)
					req->previousNeuronStateBuffer->release();
				if (req->synapticCurrentBuffer)
					req->synapticCurrentBuffer->release();
				req->inputBuffer = nullptr;
				req->previousNeuronStateBuffer = nullptr;
				req->synapticCurrentBuffer = nullptr;
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
NeuronUpdater<MType>::~NeuronUpdater()
{
	delete _inputBufferSubscription;
	delete _neuronStateSubscription;
	delete _synapticCurrentSubscription;
}


