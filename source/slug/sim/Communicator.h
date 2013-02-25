#pragma once
#include <mpi.h>
#include <slug/util/Unique.h>

namespace slug
{

namespace sim
{

/**
	Encapsulates MPI communication variables within a MPI communicator
*/
class Communicator
{
public:
	///This machine's rank within the communicator
	int rank;

	///A map of machine uniques to their ranks within the communicator
	std::map<util::Unique, int> machineRanks;

	///The MPI communicator this object describes
	MPI_Comm mpiComm;
};

} //namespace sim

} //namespace slug
