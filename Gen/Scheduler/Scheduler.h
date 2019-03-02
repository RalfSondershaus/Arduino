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
        tTimer      ulCycleTime;        ///< [us] Cycle time
        Runable*    runable;            ///< Pointer to runable
    } tRunable;

protected:
    class MyMicroTimer : public MicroTimer
     {
     public:
         MyMicroTimer() {}
         void init(tTimer ulTime)        { ulTimer  = ulTime; }
         void increment(tTimer ulTime)   { ulTimer += ulTime; }
         static tTimer getCurrentTime(void)  { return millis(); }
     };
    typedef struct
    {
        MyMicroTimer NextCall;
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
    /// Initialize after start up. Set time for first call of runables
    /// to current time (in millisec) + 1 millisec + ulStartOff
    void init(void);
    /// Schedule runables. Call run() for each runable if its timer
    /// is elapesd.
    void schedule(void);
};

#endif /* SCHEDULER_H_ */
