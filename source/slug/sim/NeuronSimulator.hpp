/*
	Implementation file for NeuronSimulator.h
	namespace slug::sim
*/

template<MemoryType MType>
bool NeuronSimulator<MType>::read(const std::string& prefix)
{
	//Construct the full filename and open it
	std::string filename = prefix + "/" + std::to_string(id);
	util::InputStream bs(filename);
	if (bs.fail())
		return false;

	//unsigned int
	bs >> numNeurons;

	//Read the generic parameterized interface
	Parameterized::read(bs);

	//Read in the number of segments per neuron
	unsigned int* ns = new unsigned int[numNeurons];
	bs.read((char*)ns, numNeurons * sizeof(unsigned int));
	numSegments = this->template _allocate<unsigned int>(numNeurons);
	this->template _transfer<MType, MemoryType::System>
		(numSegments, ns, numNeurons);
	
	//Determine the segment offsets for each neuron
	unsigned int* so = new unsigned int[numNeurons + 1];
	std::partial_sum(ns, ns + numNeurons, so + 1);
	so[0] = 0;
	segmentOffsets = this->template _allocate<unsigned int>(numNeurons + 1);
	this->template _transfer<MType, MemoryType::System>
		(segmentOffsets, so, numNeurons + 1);

	unsigned int totalSegments = so[numNeurons];
	delete [] ns;
	delete [] so;

	if (totalSegments > 0)
	{
		//Read in the segment starts
		unsigned int* ss = new unsigned int[totalSegments];
		bs.read((char*)ss, totalSegments * sizeof(unsigned int));
		segmentStarts = 
			this->template _allocate<unsigned int>(totalSegments);
		this->template _transfer<MType, MemoryType::System>
			(segmentStarts, ss, totalSegments);

		//Read in the segment ends
		unsigned int* se = new unsigned int[totalSegments];
		bs.read((char*)se, totalSegments * sizeof(unsigned int));
		segmentEnds = 
			this->template _allocate<unsigned int>(totalSegments);
		this->template _transfer<MType, MemoryType::System>
			(segmentEnds, se, totalSegments);

		delete [] ss;
		delete [] se;
	}
	else
	{
		segmentStarts = nullptr;
		segmentEnds = nullptr;
	}

	return true;
}

template<MemoryType MType>
NeuronSimulator<MType>::~NeuronSimulator()
{
	this->template _deallocate(numSegments);
	this->template _deallocate(segmentOffsets);
	this->template _deallocate(segmentStarts);
	this->template _deallocate(segmentEnds);
}
