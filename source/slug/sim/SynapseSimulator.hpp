template<MemoryType MType>
bool SynapseSimulator<MType>::read(const std::string& prefix)
{
	//Construct the path and open the file
	std::string filename = prefix + "/" + std::to_string(id);
	util::InputStream bs(filename);
	if (bs.fail())
		return false;
	
	//Read in the total number of synapses
	bs >> numSynapses;

	//Read in the generalized parameter interface
	Parameterized::read(bs);

	//Read in the postsynaptic IDs
	unsigned int* nid = new unsigned int[numSynapses];
	bs.read((char*)nid, sizeof(unsigned int) * numSynapses);
	postsynapticNeuronIDs =
		this->template _allocate<unsigned int>(numSynapses);
	this->template _transfer<MType, MemoryType::System>
		(postsynapticNeuronIDs, nid, numSynapses);

	delete [] nid;

	return true;
}

template<MemoryType MType>
SynapseSimulator<MType>::~SynapseSimulator()
{
	this->template _deallocate(postsynapticNeuronIDs);
}
