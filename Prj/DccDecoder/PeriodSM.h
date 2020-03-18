/**
 * 
 */

#ifndef PERIODSM_H
#define PERIODSM_H

#include <DccInterpreter.h>

// ---------------------------------------------------
// State machine for period detection (half bits)
// ---------------------------------------------------
class PeriodStateMachine
{
protected:
  /// States of this state machine
  typedef enum 
  {
    STATE_INVALID       = 0,      ///< No bit detected, phase not detected; this is the default state after startup
    STATE_SHORT_INIT_1  = 1,      ///< One short tick detected, no long tick detected since last INVALID
    STATE_SHORT_INIT_2  = 2,      ///< Two subsequent short ticks detected (bit value 1), no long tick detected since last INVALID
    STATE_LONG_INIT_1   = 3,      ///< One long tick detected, no short tick detected since last INVALID
    STATE_LONG_INIT_2   = 4,      ///< Two subsequent long ticks detected (bit value 0), no short tick detected since last INVALID
    STATE_SHORT_1       = 5,      ///< One short tick detected
    STATE_SHORT_2       = 6,      ///< Two subsequent short ticks detected (bit value 1)
    STATE_LONG_1        = 7,      ///< One long tick detected
    STATE_LONG_2        = 8,      ///< Two subsequent long ticks detected (bit value 0)
    STATE_MAX_COUNT     = 9       ///< Maximum number of states
  } eState;

  /// Type for received tick
  typedef enum 
  { 
    INVALID_TICK  = 0, 
    SHORT_TICK    = 1, 
    LONG_TICK     = 2 
  } tTickReceivedEnum;

  /// Representation of a single state
  class StateBase
  {
  protected:
    PeriodStateMachine& PSM;
  public:
    /// Constructor
    StateBase(PeriodStateMachine& psm) : PSM(psm), unTickCnt(0u) {}
    /// For dubugging purposes: count number of ticks
    unsigned int unTickCnt;
    /// entry function
    virtual void entryFunc(void)  {}
    /// execute state function
    virtual eState execFunc(eState curState, tTickReceivedEnum tickReceived) = 0;
    /// exit function
    virtual void exitFunc(void)   {}
  };

  /// Helper macro to declare a state class which is derived from a base class
  #define PSM_DECLARE_STATE(ClassName, BaseClass)                                 \
    class ClassName : public BaseClass                                            \
    {                                                                             \
    public:                                                                       \
      ClassName(PeriodStateMachine& psm) : BaseClass(psm) {}                      \
      virtual eState execFunc(eState curState, tTickReceivedEnum tickReceived);   \
    }
  
  PSM_DECLARE_STATE(StateInvalid, StateBase);
  PSM_DECLARE_STATE(StateShortInit1, StateBase);
  PSM_DECLARE_STATE(StateShortInit2, StateBase);
  PSM_DECLARE_STATE(StateLongInit1, StateBase);
  PSM_DECLARE_STATE(StateLongInit2, StateBase);
  PSM_DECLARE_STATE(StateShort1, StateBase);
  PSM_DECLARE_STATE(StateShort2, StateBase);
  PSM_DECLARE_STATE(StateLong1, StateBase);
  PSM_DECLARE_STATE(StateLong2, StateBase);

  /// Current state
  eState state;

  /// Previous state
  eState prevState;

  /// The states
  StateInvalid    stateInvalid;
  StateShort1     stateShortInit1;
  StateShort2     stateShortInit2;
  StateLong1      stateLongInit1;
  StateLong2      stateLongInit2;
  StateShort1     stateShort1;
  StateShort2     stateShort2;
  StateLong1      stateLong1;
  StateLong2      stateLong2;

  /// Array of pointers to the states
  const StateBase * aStates[STATE_MAX_COUNT];

  /// The DCC interpreter
  DccInterpreter DccIntp;

  /// Return tick type (short, long, invalid)
  static tTickReceivedEnum checkTick(unsigned long ulTime);

public:
  /// constructor
  PeriodStateMachine();
  /// destructor
  ~PeriodStateMachine();
  /// Execute state machine with an event
  void execute(unsigned long ulTimeDiff);
  /// For debugging: return number of calls for a state (< STATE_MAX_COUNT)
  unsigned int getNrCalls(unsigned int unState) const;
  unsigned int getNrOne() const { return DccIntp.getNrOne(); }
  unsigned int getNrZero() const { return DccIntp.getNrZero(); }
  unsigned int getByte(unsigned int idx) const { return DccIntp.getByte(idx); }
};

#endif // PERIODSM_H