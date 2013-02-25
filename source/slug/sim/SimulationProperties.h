#pragma once
#include <slug/util/Unique.h>

namespace slug
{

namespace sim
{

class StoppingCondition
{
public:
    virtual bool done(unsigned int timestep) const = 0;
};

class IndefiniteCondition
    : public StoppingCondition
{
public:
    virtual bool done(unsigned int timestep) const;
};

class TimedCondition
    : public StoppingCondition
{
public:
    TimedCondition(unsigned int numSteps);
    virtual bool done(unsigned int timestep) const;
private:
    unsigned int _numSteps;
};

class SimulationProperties
{
public:
	///Constructor
	SimulationProperties();

	///The timestep in seconds
	float timestep;

	///The machine ID for this machine
	util::Unique myID;

	///The size of the global neuron fire vector
	unsigned int neuronFireVectorSize;

    ///Returns the StoppingCondition for the simulation
    const StoppingCondition* stoppingCondition() const;

    ///Replaces the StoppingCondition for the simulation
    void stoppingCondition(StoppingCondition* sc);

    /**
        Makes the simulation run indefinitely (overrides preexisting stopping
        conditions)
    */
    void setIndefinite();

    /**
        Makes the simulation run for a specified number of timesteps (overrides
        preexisting stopping conditions)
    */
    void setNumSteps(unsigned int numSteps);
private:
    ///When the simulation should end
    StoppingCondition* _stoppingCondition;
};

} //namespace sim

} //namespace slug
