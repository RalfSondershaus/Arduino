/**/
/*
 * Scheduler.h
 *
 *  Created on: 01.10.2018
 *      Author: Papi
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "Runable.h"
#include "Timer.h"

#define SCHEDULER_MAX_NR_RUNABLES   20u

/// Schedule runables
class Scheduler
{
public:
    /// Timer data type
    typedef MicroTimer::tTimer tTimer;
    /// Information about a runable; to be used in const structure
    typedef struct
    {
        tTimer      ulStartOffset;      ///< [us] Offset time at start up
        tTimer      ulCycleTime;        ///< [us] Cycle time, 1 ms = 1000 us
        Runable*    runable;            ///< Pointer to runable
    } tRunable;

protected:
    typedef struct
    {
        MicroTimer NextCall;
    } tRunTime;

    /// Array of runables.
    tRunable aRunables[SCHEDULER_MAX_NR_RUNABLES];
    /// Array of run time information for runables
    tRunTime aRunTime[SCHEDULER_MAX_NR_RUNABLES];
    /// Number of runables, that is, number of elements in arrays aRunables and aRunTime. Must not be greater than SCHEDULER_MAX_NR_RUNABLES.
    unsigned int unNrRunables;
    /// Copy constructor
    Scheduler(const Scheduler& s);

public:
    /// Constructor
    Scheduler();
    /// Default destructor
    ~Scheduler();
    /// Add a runable
    /// @param ulStartOff [us] offset time at start up (first call of run())
    /// @param ulCycTime [ms] Cycle time of runable run()
    /// @runabl Pointer to Runable object; must not be NULL
    bool add(tTimer ulStartOff, tTimer ulCycTime, Runable * runabl);
    /// initialize after start up
    void init(void);
    /// schedule runables
    void schedule(void);
};

#endif /* SCHEDULER_H_ */
