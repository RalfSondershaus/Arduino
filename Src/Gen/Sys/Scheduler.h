 /**
  * @file Scheduler.h
  * @author Ralf Sondershaus
  *
  * @brief Schedule a list of Runables
  *
  * @copyright Copyright 2018 - 2022 Ralf Sondershaus
  *
  * SPDX-License-Identifier: Apache-2.0
  */


#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <Sys/Runable.h>
#include <Util/Timer.h>

namespace Sys
{
  // -------------------------------------------------
  /// Schedule runables.
  /// @tparam N Maximal number of runables
  // -------------------------------------------------
  template <int N> class Scheduler
  {
  public:
    /// This class
    typedef Scheduler<N> This;
    /// Timer data type
    typedef Util::MicroTimer::time_type time_type;
    /// size type
    typedef size_t size_type;
    /// Information about a runable
    typedef struct
    {
      time_type         ulStartOffset;      ///< [us] Offset time at start up
      time_type         ulCycleTime;        ///< [us] Cycle time
      Util::MicroTimer  Timer;              ///< The timer for the next call
      Runable*          pRnbl;              ///< Pointer to runable
    } tRunable;
    /// Return the (template parameter) maximal number of runables
    static constexpr size_type getMaxNrRunables() noexcept { return static_cast<size_type>(N); }

  protected:

    /// Array of runables.
    tRunable aRunables[N];
    /// Number of used runables, that is, number of used elements in arrays aRunables and aRunTime. Must not be greater than N.
    unsigned int unNrRunables;
    /// Copy constructor
    Scheduler(const This& s) {}

  public:
    /// Constructor
    Scheduler() : unNrRunables(0) {}
    /// Default destructor
    ~Scheduler() {}
    /// Add a runable
    /// @param ulStartOff [us] offset time at start up (first call of run())
    /// @param ulCycTime [us] Cycle time of runable run()
    /// @p Pointer to Runable object; must not be NULL
    bool add(time_type ulStartOff, time_type ulCycTime, Runable * pRnbl)
    {
      bool bRet;

      if ((unNrRunables < getMaxNrRunables()) && (pRnbl != nullptr))
      {
        aRunables[unNrRunables].ulStartOffset = ulStartOff;
        aRunables[unNrRunables].ulCycleTime = ulCycTime;
        aRunables[unNrRunables].pRnbl = pRnbl;
        unNrRunables++;
        bRet = true;
      }
      else
      {
        bRet = false;
      }

      return bRet;
    }
    /// Initialize after start up. Set time for first call of runables
    /// to current time (in millisec) + 1 millisec + ulStartOff
    void init(void)
    {
      unsigned int i;
      time_type ulTime;

      if (unNrRunables > 0)
      {
        for (i = 0; i < unNrRunables; i++)
        {
          aRunables[i].pRnbl->init();
          (void)aRunables[i].Timer.start(aRunables[i].ulStartOffset);
        }
      }
    }
    /// Schedule runables. Call run() for each runable if its timer
    /// is elapesd.
    void schedule(void)
    {
      unsigned int i;

      for (i = 0; i < unNrRunables; i++)
      {
        if (aRunables[i].Timer.timeout())
        {
          aRunables[i].pRnbl->run();
          (void)aRunables[i].Timer.increment(aRunables[i].ulCycleTime);
        }
      }
    }
  };

} // namespace Sys

#endif /* SCHEDULER_H_ */
