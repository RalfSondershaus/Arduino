/**
 * Declares and defines classes 
 * - MicroTimer based on microseconds 
 * - MilliTimer based on milliseconds
 */

#ifndef TIMER_H__
#define TIMER_H__

#include <Arduino.h>

namespace Util
{
  // --------------------------------------------------------------------------------------------
  /// Start a timer, get time since a timer start and check a timeout.
  /// Is based on micros(), so all units are [us].
  /// Maximal time: 70 minutes.
  /// <br>
  /// Example 1:
  /// <code>
  /// MicroTimer t;
  /// ...
  /// t.start(0);
  /// ...
  /// t.getTimeSince(); // returns the time since t.start(0) in [us]
  /// ...
  /// </code>
  /// Example 2:
  /// <code>
  /// MicroTimer t;
  /// ...
  /// t.start(1000); // 1000 us ahead
  /// ...
  /// t.timeout(); // returns true if timeout() is called at least 1000 us after t.start(1000)
  /// ...
  /// </code>
  // --------------------------------------------------------------------------------------------
  class MicroTimer
  {
  public:
    /// Timer data type
    typedef unsigned long time_type;

  protected:
    /// time stamp for timeout
    time_type ulTimer;

  public:
    /// Construct
    MicroTimer() : ulTimer(0) {}

    /// Desctruct
    ~MicroTimer() {}

    /// Start timer with time ulTime [us], return timer value in [us]
    time_type start(const time_type ulTime = 0) { ulTimer = getCurrentTime() + ulTime; return ulTimer; }

    /// Get time since start() in [us]
    time_type getTimeSince() const { return getCurrentTime() - ulTimer; }

    /// Increment timer with time ulTime [us], return new timer value in [us]
    time_type increment(const time_type ulTime) noexcept { ulTimer += ulTime; return ulTimer; }

    /// Return true if timer is elapsed; false otherwise.
    /// micros() >= ulTimer: positive result: highest bit is 0
    /// micros() <  ulTimer: negative result: highest bit is 1
    bool timeout() const { return ((static_cast<time_type>(getCurrentTime() - ulTimer)) & static_cast<time_type>(0x80000000u)) == static_cast<time_type>(0u); }

    /// Return current time [us]
    static time_type getCurrentTime(void) { return micros(); }
  };

  // --------------------------------------------------------------------------------------------
  /// Start a timer, get time since a timer start and check a timeout.
  /// Is based on millis(), so all units are [ms].
  /// Maximal time: 50 days.
  // --------------------------------------------------------------------------------------------
  class MilliTimer
  {
  public:
    /// Timer data type
    typedef unsigned long time_type;

  protected:
    /// time stamp at start()
    time_type ulTimer;

  public:
    /// Construct
    MilliTimer() : ulTimer(0) {}
    /// Desctruct
    ~MilliTimer() {}

    /// Start timer with ulTime [ms], return timer value in [ms]
    time_type start(time_type ulTime = 0) { ulTimer = getCurrentTime() + ulTime; return ulTimer; }

    /// Get time since start() + ulTime in [ms]
    time_type getTimeSince(void) const { return getCurrentTime() - ulTimer; }

    /// Return true if timer is elapsed; false otherwise
    /// millis() >= ulTimer: positive result: highest bit is 0
    /// millis() <  ulTimer: negative result: highest bit is 1
    bool timeout(void) const { return ((static_cast<time_type>(getCurrentTime() - ulTimer)) & static_cast<time_type>(0x80000000u)) == static_cast<time_type>(0u); }

    /// Return current time [us]
    static time_type getCurrentTime(void) { return millis(); }
  };

} // namespace Util

#endif // TIMER_H__
