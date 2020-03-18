/// DccInterpreter.cpp
///
/// DCC Interpreter for Arduino

#include <Arduino.h>
#include <DccInterpreter.h>

/// A preamble is valid if this number of "1" is transmitted (at least)
#define DCCINTERPRETER_PREAMBLE_MIN_NR_ONES    10u

// ---------------------------------------------------
/// Returns true if unNr exceeds the minimal number of "1"
// ---------------------------------------------------
static inline bool isMinNumberOfOneTransmitted(unsigned int unNr) { return unNr >= DCCINTERPRETER_PREAMBLE_MIN_NR_ONES;  }

// ---------------------------------------------------
/// constructor
// ---------------------------------------------------
DccInterpreter::DccInterpreter() 
  : unNrOne(0)
  , unNrZero(0)
  , unPacket(0)
  , statePreamble(*this)
  , stateByte(*this)
  , aStates   // array initialization works with C++11 or beyond
    {
      &statePreamble,
      &stateByte
    }
{
  invalid();
}

// ---------------------------------------------------
/// destructor
// ---------------------------------------------------
DccInterpreter::~DccInterpreter()
{}

/// Execute state function
void DccInterpreter::StatePreamble::entry()
{
  unNrOne = 0;
}

/// State function: check if a valid preamble is transmitted:
/// Sequence of at least 10x "1", followed by a "0" 
DccInterpreter::tState DccInterpreter::StatePreamble::execFunc(tState curState, tBit bitReceived)
{
  tState nextState = curState;

  if (bitReceived == BIT_ONE)
  {
    // prevent overflow
    if (unNrOne < 255u)
    {
      unNrOne++;
    }
  }
  else // BIT_ZERO
  {
    if (isMinNumberOfOneTransmitted(unNrOne))
    {
      nextState = STATE_BYTE;
    }
    else
    {
      // min number of "1" not reached, invalid or waiting for first "1", reset
      unNrOne = 0;
    }
  }

  return nextState;
}

/// Entry function for state STATE_BYTE: Reset counters to start a new interpretation
void DccInterpreter::StateByte::entry()
{
  unNrBits = 0;
  unNrBytes = 0;
}

/// State function: Interpret adress or data bytes bit by bit.
DccInterpreter::tState DccInterpreter::StateByte::execFunc(tState curState, tBit bitReceived)
{
  tState nextState = curState;

  unNrBits++;

  if (unNrBits <= 8)
  {
    if (bitReceived == BIT_ONE)
    {
      //aBytes[unNrBytes] = (aBytes[unNrBytes] << 1u) | 1u;
      DccInterp.addOne();
    }
    else
    {
      //aBytes[unNrBytes] = (aBytes[unNrBytes] << 1u);
      DccInterp.addZero();
    }
  }
  else
  {
    if (bitReceived == BIT_ZERO)
    {
       DccInterp.nextByte();
    }
    else // BIT_ONE
    {
      nextState = STATE_PREAMBLE;
    }
  }

  return nextState;
}

/// trigger state machine
void DccInterpreter::execute(tBit bitReceived)
{
  tState prevState = state;
  state = aStates[state]->execFunc(state, bitReceived);
  if (prevState != state)
  {
    aStates[state]->entry();
  }
}

// ---------------------------------------------------
/// "1" bit received
// ---------------------------------------------------
void DccInterpreter::one()
{
  unNrOne++;
  execute(BIT_ONE);
}

// ---------------------------------------------------
/// "0" bit received
// ---------------------------------------------------
void DccInterpreter::zero()
{
  unNrZero++;
  execute(BIT_ZERO);
}

// ---------------------------------------------------
/// Invalid received, reset
// ---------------------------------------------------
void DccInterpreter::invalid()
{
  state = STATE_PREAMBLE;
  aStates[state]->entry();
}
