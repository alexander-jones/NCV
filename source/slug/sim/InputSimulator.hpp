template<MemoryType MType>
bool InputSimulator<MType>::read(const std::string& prefix)
{
	//Construct the full path
	std::string filename = prefix + "/" + std::to_string(id);

	//Open the file
	util::InputStream bs(filename);
	if (bs.fail())
		return false;

	//Get the total number of inputs
	bs >> numInputs;

	//Read in the generalized Parameter data
	Parameterized::read(bs);

	//Construct the neuron ID array in the appropriate memory
	unsigned int* nids = new unsigned int[numInputs];
	bs.read((char*)nids, sizeof(unsigned int) * numInputs);
	neuronIDs = this->template _allocate<unsigned int>(numInputs);
	this->template _transfer<MType, MemoryType::System>
		(neuronIDs, nids, numInputs);

	delete [] nids;
	
	return true;
}

template<MemoryType MType>
InputSimulator<MType>::~InputSimulator()
{
	this->template _deallocate(neuronIDs);
}
