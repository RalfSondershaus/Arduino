/*
 * Scheduler.cpp
 *
 *  Created on: 01.10.2018
 *      Author: Papi
 */

#include "Scheduler.h"

/// Default constructor
Scheduler::Scheduler() : unNrRunables(0)
{
}

/// Copy constructor
Scheduler::Scheduler(const Scheduler& s) : unNrRunables(s.unNrRunables)
{}

/// Default destructor (virtual)
Scheduler::~Scheduler()
{}

/// Add a runable
/// @param ulStartOff [us] offset time at start up (first call of run())
/// @param ulCycTime [ms] Cycle time of runable run()
/// @runabl Pointer to Runable object; must not be NULL
bool Scheduler::add(tTimer ulStartOff, tTimer ulCycTime, Runable * runabl)
{
    bool bRet;

    if ((unNrRunables < SCHEDULER_MAX_NR_RUNABLES) && (runabl != (Runable *) NULL))
    {
        aRunables[unNrRunables].ulStartOffset = ulStartOff;
        aRunables[unNrRunables].ulCycleTime   = 1000u * ulCycTime;
        aRunables[unNrRunables].runable       = runabl;
        unNrRunables++;
        bRet = true;
    }
    else
    {
        bRet = false;
    }

    return bRet;
}

/// initialize after start up
void Scheduler::init(void)
{
    unsigned int i;

    for (i = 0; i < unNrRunables; i++)
    {
        aRunables[i].runable->init();
    }
    for (i = 0; i < unNrRunables; i++)
    {
        (void) aRunTime[i].NextCall.start(aRunables[i].ulStartOffset);
    }
}

/// schedule runables
void Scheduler::schedule(void)
{
    unsigned int i;

    for (i = 0; i < unNrRunables; i++)
    {
        if (aRunTime[i].NextCall.timeout())
        {
            aRunables[i].runable->run();
            (void) aRunTime[i].NextCall.start(aRunables[i].ulCycleTime);
        }
    }
}

// EOF
