// ---------------------------------------------------
/// State machine for period (which are transformed into bits)
// ---------------------------------------------------

#include "PeriodSM.h"

/// [us] Minimum time between two edges for a "short" part (first or second part of a "1")
#define PART_TIME_SHORT_MIN      52u
/// [us] Maximum time between two edges for a "short" part (first or second part of a "1")
#define PART_TIME_SHORT_MAX      64u
/// [us] Minimum time between two edges for a "long" part (first or second part of a "0")
#define PART_TIME_LONG_MIN       90u
/// [us] Maximum time between two edges for a "long" part (first or second part of a "0")
#define PART_TIME_LONG_MAX    10000u

// ---------------------------------------------------
/// S-9.1: In a "1" bit, the first and last part of a bit shall have the same duration, and that duration shall nominally be 58 microseconds2, giving
/// 15 the bit a total duration of 116 microseconds. Digital Command Station components shall transmit "1" bits with the first and last parts
/// each having a duration of between 55 and 61 microseconds. A Digital Decoder must accept bits whose first and last parts have a
/// duration of between 52 and 64 microseconds, as a valid bit with the value of "1".
/// @param ulTime [us] Time between two edges
// ---------------------------------------------------
static inline bool isShortTick(unsigned long ulTime)
{
  return (ulTime >= PART_TIME_SHORT_MIN) && (ulTime <= PART_TIME_SHORT_MAX);
}

// ---------------------------------------------------
/// In a "0" bit, the duration of the first and last parts of each transition shall nominally be greater than or equal to 100 microseconds. To
/// 20 keep the DC component of the total signal at zero as with the "1" bits, the first and last part of the "0" bit are normally equal to one
/// another. Digital Command Station components shall transmit "0" bits with each part of the bit having a duration of between 95 and
/// 9900 microseconds with the total bit duration of the "0" bit not exceeding 12000 microseconds. A Digital Decoder must accept bits,
/// whose first or last parts have a duration between 90 and 10,000 microseconds, as a valid bit with the value of "0". Figure 1 provides
/// an example of bits encoded using this technique.
// ---------------------------------------------------
static inline bool isLongTick(unsigned long ulTime)
{
  return (ulTime >= PART_TIME_LONG_MIN) && (ulTime <= PART_TIME_LONG_MAX);
}

// ---------------------------------------------------
/// constructor
// ---------------------------------------------------
PeriodStateMachine::PeriodStateMachine()
  : state(STATE_INVALID)
  , stateInvalid(*this)
  , stateShortInit1(*this)
  , stateShortInit2(*this)
  , stateLongInit1(*this)
  , stateLongInit2(*this)
  , stateShort1(*this)
  , stateShort2(*this)
  , stateLong1(*this)
  , stateLong2(*this)
  , aStates {        // array initialization works with C++11 or beyond
    &stateInvalid,
    &stateShortInit1,
    &stateShortInit2,
    &stateLongInit1,
    &stateLongInit2,
    &stateShort1,
    &stateShort2,
    &stateLong1,
    &stateLong2 }
{}

// ---------------------------------------------------
/// destructor
// ---------------------------------------------------
PeriodStateMachine::~PeriodStateMachine()
{}

// ---------------------------------------------------
/// Execute state machine with an event
// ---------------------------------------------------
void PeriodStateMachine::execute(unsigned long ulTimeDiff)
{
  tTickReceivedEnum tickReceived;

  tickReceived = checkTick(ulTimeDiff);
  state = aStates[state]->execFunc(state, tickReceived);
  prevState = state;
}

// ---------------------------------------------------
/// Return tick type (short, long, invalid)
// ---------------------------------------------------
PeriodStateMachine::tTickReceivedEnum PeriodStateMachine::checkTick(unsigned long ulTime)
{
  tTickReceivedEnum tickReceived;

  if (isShortTick(ulTime))
  {
    tickReceived = SHORT_TICK;
  }
  else if (isLongTick(ulTime))
  {
    tickReceived = LONG_TICK;
  }
  else
  {
    tickReceived = INVALID_TICK;
  }

  return tickReceived;
}

// ---------------------------------------------------
/// execute state function STATE_INVALID
// ---------------------------------------------------
PeriodStateMachine::eState PeriodStateMachine::StateInvalid::execFunc(PeriodStateMachine::eState curState, tTickReceivedEnum tickReceived)
{
  // transition map: next state for a received tick
  static const eState transitionMap[] =
  {
    STATE_INVALID,      // INVALID_TICK received
    STATE_SHORT_INIT_1, // SHORT_TICK received
    STATE_LONG_INIT_1   // LONG_TICK received
  };
  eState nextState = transitionMap[tickReceived];
  unTickCnt++;
  PSM.DccIntp.invalid();
  return nextState;
}

// ---------------------------------------------------
/// State function: STATE_SHORT_INIT_1
// ---------------------------------------------------
PeriodStateMachine::eState PeriodStateMachine::StateShortInit1::execFunc(PeriodStateMachine::eState curState, tTickReceivedEnum tickReceived)
{
  // transition map: next state for a received tick
  static const eState transitionMap[] = 
  {
    STATE_INVALID,        // INVALID_TICK received
    STATE_SHORT_INIT_2,   // SHORT_TICK received
    STATE_LONG_1          // LONG_TICK received
  };
  eState nextState = transitionMap[tickReceived];
  unTickCnt++;
  return nextState;
}

// ---------------------------------------------------
/// State function: STATE_SHORT_INIT_2
// ---------------------------------------------------
PeriodStateMachine::eState PeriodStateMachine::StateShortInit2::execFunc(PeriodStateMachine::eState curState, tTickReceivedEnum tickReceived)
{
  // transition map: next state for a received tick
  static const eState transitionMap[] =
  {
    STATE_INVALID,        // INVALID_TICK received
    STATE_SHORT_INIT_1,   // SHORT_TICK received
    STATE_LONG_1          // LONG_TICK received
  };
  eState nextState = transitionMap[tickReceived];
  PSM.DccIntp.one();
  unTickCnt++;
  return nextState;
}

// ---------------------------------------------------
/// State function: STATE_LONG_INIT_1
// ---------------------------------------------------
PeriodStateMachine::eState PeriodStateMachine::StateLongInit1::execFunc(PeriodStateMachine::eState curState, tTickReceivedEnum tickReceived)
{
  // transition map: next state for a received tick
  static const eState transitionMap[] = 
  {
    STATE_INVALID,        // INVALID_PART received
    STATE_SHORT_1,        // SHORT_PART received
    STATE_LONG_INIT_2     // LONG_PART received
  };
  eState nextState = transitionMap[tickReceived];
  unTickCnt++;
  return nextState;
}

// ---------------------------------------------------
/// State function: STATE_LONG_INIT_2
// ---------------------------------------------------
PeriodStateMachine::eState PeriodStateMachine::StateLongInit2::execFunc(PeriodStateMachine::eState curState, tTickReceivedEnum tickReceived)
{
  // transition map: next state for a received tick
  static const eState transitionMap[] = 
  {
    STATE_INVALID,      // INVALID_PART received
    STATE_SHORT_1,      // SHORT_PART received
    STATE_LONG_INIT_1   // LONG_PART received
  };
  eState nextState = transitionMap[tickReceived];
  PSM.DccIntp.zero();
  unTickCnt++;
  return nextState;
}

// ---------------------------------------------------
/// State function: STATE_SHORT_1
// ---------------------------------------------------
PeriodStateMachine::eState PeriodStateMachine::StateShort1::execFunc(PeriodStateMachine::eState curState, tTickReceivedEnum tickReceived)
{
  // transition map: next state for a received tick
  static const eState transitionMap[] =
  {
    STATE_INVALID,      // INVALID_TICK received
    STATE_SHORT_2,      // SHORT_TICK received
    STATE_INVALID       // LONG_TICK received
  };
  eState nextState = transitionMap[tickReceived];
  unTickCnt++;
  return nextState;
}

// ---------------------------------------------------
/// State function: STATE_SHORT_2
// ---------------------------------------------------
PeriodStateMachine::eState PeriodStateMachine::StateShort2::execFunc(PeriodStateMachine::eState curState, tTickReceivedEnum tickReceived)
{
  // transition map: next state for a received tick
  static const eState transitionMap[] =
  {
    STATE_INVALID,      // INVALID_TICK received
    STATE_SHORT_1,      // SHORT_TICK received
    STATE_LONG_1        // LONG_TICK received
  };
  eState nextState = transitionMap[tickReceived];
  PSM.DccIntp.one();
  unTickCnt++;
  return nextState;
}

// ---------------------------------------------------
/// State function: STATE_LONG_1
// ---------------------------------------------------
PeriodStateMachine::eState PeriodStateMachine::StateLong1::execFunc(PeriodStateMachine::eState curState, tTickReceivedEnum tickReceived)
{
  // transition map: next state for a received tick
  static const eState transitionMap[] =
  {
    STATE_INVALID,      // INVALID_PART received
    STATE_INVALID,      // SHORT_PART received
    STATE_LONG_2        // LONG_PART received
  };
  eState nextState = transitionMap[tickReceived];
  unTickCnt++;
  return nextState;
}

// ---------------------------------------------------
/// State function: STATE_LONG_2
// ---------------------------------------------------
PeriodStateMachine::eState PeriodStateMachine::StateLong2::execFunc(PeriodStateMachine::eState curState, tTickReceivedEnum tickReceived)
{
  // transition map: next state for a received tick
  static const eState transitionMap[] =
  {
    STATE_INVALID,      // INVALID_PART received
    STATE_SHORT_1,      // SHORT_PART received
    STATE_LONG_1        // LONG_PART received
  };
  eState nextState = transitionMap[tickReceived];
  PSM.DccIntp.zero();
  unTickCnt++;
  return nextState;
}

/// For debugging: return number of calls for a state (< STATE_MAX_COUNT)
unsigned int PeriodStateMachine::getNrCalls(unsigned int unState) const
{
  unsigned int unRet;

  if (unState < (unsigned int)STATE_MAX_COUNT)
  {
    unRet = aStates[unState]->unTickCnt;
  }
  else
  {
    unRet = (unsigned int) -1;
  }

  return unRet;
}

// EOF