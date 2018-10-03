#ifndef TIMER_H__
#define TIMER_H__

#include <Arduino.h>
/**
* Declares and defines class MicroTimer
*/

/// Start a timer, get time since a timer start and check a timeout.
/// Is based on micros(), so all units are [us].
/// 70 minutes.
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

class MicroTimer
{
public:
  /// Timer data type
  typedef unsigned long tTimer;

protected:
  /// time stamp at start()
    tTimer ulTimer;

public:
  /// Construct
  MicroTimer() : ulTimer(0) {}

  /// Desctruct
  ~MicroTimer() {}

  /// Start timer, return timer value in [us]
  tTimer start(unsigned long ulTime = 0)   { ulTimer = micros() + ulTime; return ulTimer; }

  /// Get time since start() in [us]
  tTimer getTimeSince(void) const          { return micros() - ulTimer; }

  /// Return true if timer is elapsed; false otherwise
  /// micros() >= ulTimer: positive result: highest bit is 0
  /// micros() <  ulTimer: negative result: highest bit is 1
  boolean timeout(void)                           { return (((tTimer) (micros() - ulTimer)) & (tTimer) 0x80000000u) == 0u; }
};

/**
* Declares and defines class MilliTimer
*/
/// Start a timer, get time since a timer start and check a timeout.
/// Is based on millis(), so all units are [ms].
/// 50 days.
class MilliTimer
{
public:
  /// Timer data type
  typedef unsigned long tTimer;

protected:
  /// time stamp at start()
    tTimer ulTimer;

public:
  /// Construct
  MilliTimer() : ulTimer(0) {}
  /// Desctruct
  ~MilliTimer() {}

  /// Start timer, return timer value in [ms]
  tTimer start(tTimer ulTime = 0)           { ulTimer = millis() + ulTime; return ulTimer; }

  /// Get time since start() in [ms]
  tTimer getTimeSince(void) const           { return millis() - ulTimer; }

  /// Return true if timer is elapsed; false otherwise
  /// millis() >= ulTimer: positive result: highest bit is 0
  /// millis() <  ulTimer: negative result: highest bit is 1
  boolean timeout(void)                           { return (((tTimer) (millis() - ulTimer)) & (tTimer) 0x80000000u) == 0u; }
};

#endif // TIMER_H__
