/// DccInterpreter.cpp
///
/// DCC Interpreter for Arduino

#include <DccInterpreter.h>

namespace Dcc
{
  /// A preamble is valid if this number of "1" is transmitted (at least)
  #define DCCINTERPRETER_PREAMBLE_MIN_NR_ONES    10u

  // ---------------------------------------------------
  /// Returns true if unNr exceeds the minimal number of "1"
  // ---------------------------------------------------
  static inline bool isMinNumberOfOneTransmitted(unsigned int unNr) { return unNr >= DCCINTERPRETER_PREAMBLE_MIN_NR_ONES; }

  // ---------------------------------------------------
  /// constructor
  // ---------------------------------------------------
  DccInterpreter::DccInterpreter()
    : unNrOne(0)
    , unNrZero(0)
    , unPacket(0)
  {
    invalid();
  }

  // ---------------------------------------------------
  /// destructor
  // ---------------------------------------------------
  DccInterpreter::~DccInterpreter()
  {}

  // ---------------------------------------------------
  /// State function: check if a valid preamble is transmitted:
  /// Sequence of at least 10x "1", followed by a "0" 
  // ---------------------------------------------------
  DccInterpreter::eState DccInterpreter::executePreamble(eBit bitRcv)
  {
    eState nextState = state;

    if (bitRcv == BIT_ONE)
    {
      // prevent overflow
      if (unNrOnePreamble < 255u)
      {
        unNrOnePreamble++;
      }
    }
    else // BIT_ZERO
    {
      if (isMinNumberOfOneTransmitted(unNrOnePreamble))
      {
        nextState = STATE_DATA;
      }
      // reset counter because
      // - min number of "1" not reached, invalid or waiting for first "1"
      // - or valid preamble detected, switch to STATE_DATE but prepare next switch to STATE_PREAMBLE
      unNrOnePreamble = 0;
    }

    return nextState;
  }

  // ---------------------------------------------------
  /// State function: Interpret adress or data bytes bit by bit.
  // ---------------------------------------------------
  DccInterpreter::eState DccInterpreter::executeData(eBit bitRcv)
  {
    eState nextState = state;

    if (unNrBitsData < 8u)
    {
      refCurrentPacket().addBit((unsigned int) bitRcv);
      unNrBitsData++;
    }
    else
    {
      unNrBitsData = 0;

      // Bit 0 is expected at the end of a data / address byte
      // If a 1 bit is received instead of a 0 bit, the packet is finished and the next packet is to be received
      if (bitRcv == BIT_ONE)
      {
        nextState = STATE_PREAMBLE;
        nextPacket();
      }
    }

    return nextState;
  }

  // ---------------------------------------------------
  /// trigger state machine
  // ---------------------------------------------------
  void DccInterpreter::execute(eBit bitRcv)
  {
    switch (state)
    {
    case STATE_PREAMBLE: { state = executePreamble(bitRcv); } break;
    case STATE_DATA    : { state = executeData    (bitRcv); } break;
    default            : {} break;
    }
  }

  // ---------------------------------------------------
  /// Switch to the next packet
  // ---------------------------------------------------
  void DccInterpreter::nextPacket()
  {
    unPacket = (unPacket + 1u) % DCCINTERPRETER_MAXPACKETS; 
    aPackets[unPacket].clear();
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
    unNrOnePreamble = 0;
    refCurrentPacket().clear();
  }

} // namespace Dcc

// EOF