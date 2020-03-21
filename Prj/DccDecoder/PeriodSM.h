/**
 * 
 */

#ifndef PERIODSM_H
#define PERIODSM_H

#include <DccInterpreter.h>

namespace Dcc
{
  // ---------------------------------------------------
  // State machine for half bit detection
  // ---------------------------------------------------
  class HalfBitStateMachine
  {
  public:
    /// States of this state machine
    typedef enum
    {
      STATE_INVALID      = 0,      ///< No bit detected, phase not detected; this is the default state after startup
      STATE_SHORT_INIT_1 = 1,      ///< One short tick detected, no long tick detected since last INVALID
      STATE_SHORT_INIT_2 = 2,      ///< Two subsequent short ticks detected (bit value 1), no long tick detected since last INVALID
      STATE_LONG_INIT_1  = 3,      ///< One long tick detected, no short tick detected since last INVALID
      STATE_LONG_INIT_2  = 4,      ///< Two subsequent long ticks detected (bit value 0), no short tick detected since last INVALID
      STATE_SHORT_1      = 5,      ///< One short tick detected
      STATE_SHORT_2      = 6,      ///< Two subsequent short ticks detected (bit value 1)
      STATE_LONG_1       = 7,      ///< One long tick detected
      STATE_LONG_2       = 8,      ///< Two subsequent long ticks detected (bit value 0)
      STATE_MAX_COUNT    = 9       ///< Maximum number of states
    } eState;
  protected:
    /// Type for received tick
    typedef enum
    {
      INVALID_HALFBIT = 0,
      SHORT_HALFBIT = 1,
      LONG_HALFBIT = 2
    } eHalfBit;

    /// Current state
    eState state;

    /// Previous state
    eState prevState;

    /// For debugging: number of received interrupts per state (half bits)
    unsigned long aunTickCnt[STATE_MAX_COUNT];

    /// Return half bit type (short, long, invalid)
    static eHalfBit checkTick(unsigned long ulTime);

  public:
    /// constructor
    HalfBitStateMachine(DccInterpreter& DccI);
    /// destructor
    ~HalfBitStateMachine();
    /// Execute state machine with an event
    void execute(unsigned long ulTimeDiff);
    /// For debugging: return number of calls for a state (< STATE_MAX_COUNT)
    unsigned int getNrCalls(unsigned int unState) const;
    /// The DCC interpreter
    DccInterpreter& DccIntp;
  };

} // namespace Dcc

#endif // PERIODSM_H