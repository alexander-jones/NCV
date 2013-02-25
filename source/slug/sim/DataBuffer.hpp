template<class PublicationType, class PublisherType>
Subscription<PublicationType, PublisherType>::
Subscription(PublisherType* publisher)
	: _publisher(publisher),
	  _mailbox(nullptr),
	  _mailslot(nullptr)
{
}

template<class PublicationType, class PublisherType>
PublicationType* Subscription<PublicationType, PublisherType>::pull()
{
    //Lock around us so only we can change our internal state
	std::unique_lock<std::mutex> lock(_mutex);

	//wait until a pub is available
	while (true)
	{
        //Do we have something in our mailbox?
        if (!_deliveries.empty())
        {
            //Then return the oldest delivery
            auto result = _deliveries.front();
            _deliveries.pop();
            return result;
        }

        //Otherwise, make sure the publisher is still alive
        if (nullptr == _publisher)
            return nullptr;

		//otherwise, wait for something to happen, either a publication
        //or invalidation
        _stateChanged.wait(lock);
	}
}

template<class PublicationType, class PublisherType>
void Subscription<PublicationType, PublisherType>::pull(PublicationType** slot,
                                                        Mailbox* mailbox)
{
	*slot = nullptr;
	std::unique_lock<std::mutex> lock(_mutex);
	if (!_deliveries.empty())
	{
		*slot = _deliveries.front();
		_deliveries.pop();
		return;
	}

	if (nullptr == _publisher)
	{
		mailbox->failed = true;
		return;
	}

	_mailbox = mailbox;
	_mailslot = slot;
}

template<class PublicationType, class PublisherType>
void Subscription<PublicationType, PublisherType>::cancel()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_mailbox = nullptr;
	_mailslot = nullptr;
}

template<class PublicationType, class PublisherType>
void Subscription<PublicationType, PublisherType>::push(PublicationType* data)
{
    //Make sure we're the only one modifying the queue
	std::unique_lock<std::mutex> lock(_mutex);

	if (_mailbox)
	{
		std::unique_lock<std::mutex> lock(_mailbox->mutex);
		*_mailslot = data;
		_mailbox->arrival.notify_all();
		_mailbox = nullptr;
		_mailslot = nullptr;
		return;
	}

    //Modify the queue with the new data
	_deliveries.push(data);

    //Inform any threads waiting for a pub that a new one is ready
	_stateChanged.notify_all();
}

template<class PublicationType, class PublisherType>
void Subscription<PublicationType, PublisherType>::invalidate()
{
    //Make sure no one else is trying to access the publisher variable
    std::unique_lock<std::mutex> lock(_mutex);

    //Set the publisher to null
    _publisher = nullptr;

	//If a mailbox exists, fail it
	if (_mailbox)
	{
		std::unique_lock<std::mutex> lock(_mailbox->mutex);
		_mailbox->failed = true;
		_mailbox->arrival.notify_all();
	}

    //Inform waiting threads of the invalidation
    _stateChanged.notify_all();
}

template<class PublicationType, class PublisherType>
Subscription<PublicationType, PublisherType>::~Subscription()
{
    //Make sure a publisher can't invalidate itself before we unsubscribe
	std::unique_lock<std::mutex> lock(_mutex);
	auto publisher = _publisher;
	_publisher = nullptr;
	lock.unlock();

    //If the publisher still exists
    if (publisher)
    {
        //Make sure we don't get any more publications to an unallocated sub
	    publisher->unsubscribe(this);
    }

	lock.lock();
	//Release all remaining deliveries
	while (!_deliveries.empty())
	{
		_deliveries.front()->release();
		_deliveries.pop();
	}
}


template<typename T>
SpecificPublisher<T>::SpecificPublisher()
    : _numBlanks(0)
{
}

template<typename T>
typename SpecificPublisher<T>::Subscription* SpecificPublisher<T>::subscribe()
{
    //Generate a subscription
	Subscription* sub = new Subscription(this);

    //Make sure no one else is messing with the subscriber list
	std::unique_lock<std::mutex> lock(_mutex);

    //Add the new subscription to the list
	_subscriptions.push_back(sub);

    //Return the new subscription
	return sub;
}

template<typename T>
void SpecificPublisher<T>::publish(T* data)
{
    //Make sure no one else is messing with the subscriber list
	std::unique_lock<std::mutex> lock(_mutex);

	//Increment the sub count by the number of specific subscriptions
	data->subscriptionCount += _subscriptions.size();

	//Then publish to the generic subscribers first, which adds its own count
	//and pushes data out
	unsigned int subpubs = Publisher::publish(data);

	//Push data out to our subscribers
	for (auto sub : _subscriptions)
		sub->push(data);

	//If no one is listening, just release the buffer
	if (subpubs == 0 && _subscriptions.empty())
	{
		if (data->prereleaseFunction)
			data->prereleaseFunction();
		_blanks.push(data);
		_blankAvailable.notify_all();
	}
}

template<typename T>
void SpecificPublisher<T>::unsubscribe(Subscription* sub)
{
    //Make sure we're the only ones messing with the subscriber list
	std::unique_lock<std::mutex> lock(_mutex);

	//Find the subscription and remove it
    _subscriptions.remove(sub);
}

template<typename T>
SpecificPublisher<T>::~SpecificPublisher()
{
	//Clear the generics
	Publisher::clearSubscriptions();
    {
        //Make sure no one else is touching the subscriber list
        std::unique_lock<std::mutex> lock(_mutex);

        //Invalidate all subscribers
        for (auto sub : _subscriptions)
            sub->invalidate();

		_subscriptions.clear();
    }

    //Destroy all blanks
    for (unsigned int i = 0; i < _numBlanks; ++i)
    {
        delete _getBlank();
    }
}

template<typename T>
void SpecificPublisher<T>::_addBlank(T* blank)
{
	//Make sure the blank knows how to add itself back to the pile
	blank->releaseFunction = 
		[&, blank]()
		{
			if (blank->subscriptionCount == 0)
			{
				std::unique_lock<std::mutex> lock(_mutex);
				_blanks.push(blank);
				this->_blankAvailable.notify_all();
				return;
			}

			//Decrement the sub count
			unsigned int remaining =
				std::atomic_fetch_sub(&(blank->subscriptionCount), 1u);
			
			//If this call is the last one
			if (remaining == 1)
			{
				//Do any prerelease stuff
				if (blank->prereleaseFunction)
					blank->prereleaseFunction();

				//Add ourself back onto the blank stack of the publisher
				std::unique_lock<std::mutex> lock(_mutex);
				_blanks.push(blank);
				this->_blankAvailable.notify_all();
			}
		};

	//Blanks have no subscribers to start
	blank->subscriptionCount = 0;

	//Add the blank to the stack
	std::unique_lock<std::mutex> lock(_mutex);
	_blanks.push(blank);

    ++_numBlanks;
}

template<typename T>
T* SpecificPublisher<T>::_getBlank()
{
	T* result = nullptr;
	std::unique_lock<std::mutex> lock(_mutex);
	//We must have a blank to return
	while (nullptr == result)
	{
		//If no blanks are available, wait and try again
		if (_blanks.empty())
			_blankAvailable.wait(lock);
		else
		{
			//otherwise, grab the first one and get out
			result = _blanks.top();
			_blanks.pop();
		}
	}
	return result;
}

template<typename T>
bool DataBuffer::_setPin(util::Unique key, 
                         const T* data, 
						 MemoryType memoryType)
{
	_pins[key] = Pin(data, memoryType);

	return true;
}

