template<MemoryType MType>
InputBuffer<MType>::InputBuffer(unsigned int _vectorSize)
	: vectorSize(_vectorSize),
	  voltageClamp(nullptr),
	  clampVoltage(nullptr),
	  inputCurrent(nullptr)
{
	//Compute the number of words for a clamp vector
	numWords = (vectorSize + 31) / 32;

	if (_vectorSize > 0)
	{
		//Allocate each type of data
		voltageClamp = this->template _allocate<unsigned int>(numWords);
		clampVoltage = this->template _allocate<float>(vectorSize);
		inputCurrent = this->template _allocate<float>(vectorSize);
	}

	//Set DataBuffer pins
	_setPin(util::StringID::get("voltageClamp"), voltageClamp, MType);
	_setPin(util::StringID::get("clampVoltage"), clampVoltage, MType);
	_setPin(util::StringID::get("inputCurrent"), inputCurrent, MType);
}

template<MemoryType MType>
void InputBuffer<MType>::clear()
{
	//Zero out all arrays
	this->template _zero<unsigned int>(voltageClamp, numWords);
	this->template _zero<float>(clampVoltage, vectorSize);
	this->template _zero<float>(inputCurrent, vectorSize);
}

template<MemoryType MType>
InputBuffer<MType>::~InputBuffer()
{
	if (voltageClamp)
		this->_deallocate(voltageClamp);

	if (clampVoltage)
		this->_deallocate(clampVoltage);

	if (inputCurrent)
		this->_deallocate(inputCurrent);

	voltageClamp = nullptr;
	clampVoltage = nullptr;
	inputCurrent = nullptr;
}
