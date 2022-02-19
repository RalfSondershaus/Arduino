///
/// @file PacketExtractor.h
///

#ifndef DCC_PACKETEXTRACTOR_H
#define DCC_PACKETEXTRACTOR_H

#include <Dcc/Packet.h>

namespace Dcc
{
  // ---------------------------------------------------
  /// Extract a packet from a stream of bits<BR>
  /// Minimum time for a packet (preamble + 2 bytes of data):<BR>
  /// <CODE>
  /// Preamble: 10x "1" + 1x "0"<BR>
  /// Data    :  8x "1" + 1x "0" (resp. + 1x "1" for last byte)<BR>
  ///     10x  52 us <BR>
  /// +    1x  90 us <BR>
  /// + 2x 8x  52 us <BR>
  /// + 1x 1x  90 us <BR>
  /// + 1x 1x  52 us <BR>
  /// =     1.584 us <BR>
  /// =       1.5 ms <BR>
  /// </CODE>
  /// 10 + 1 + 2x 8 + 1 + 1 = 29 bits / 1.5 ms = 58 interrupts / 1.5 ms
  // ---------------------------------------------------
  template<int PreambleMinNrOnes = 10>
  class PacketExtractor
  {
  public:
    /// The class
    typedef PacketExtractor<PreambleMinNrOnes> This;
    /// The Packet type
    typedef Packet<> MyPacket;

    /// Interface for a handler that is called if a packet is received
    class HandlerIfc
    {
    public:
      typedef This::MyPacket MyPacket;
      HandlerIfc() {}
      virtual ~HandlerIfc() {}
      virtual void packetReceived(const MyPacket& pkt) = 0;
    };

  protected:
    /// Interpretation state
    typedef enum
    {
      STATE_PREAMBLE  = 0,   /// Receiving preamble
      STATE_DATA      = 1,   /// Receiving adress or data bytes
      STATE_MAX_COUNT = 2
    } eState;

    /// Bit "0" or Bit "1" bit received
    typedef enum
    {
      BIT_ZERO = 0,
      BIT_ONE = 1
    } eBit;

    /// Current state
    eState state;

    /// A preamble is valid if this number of "1" is transmitted (at least)
    constexpr int getPreambleMinNrOnes() { return PreambleMinNrOnes; }

    /// Returns true if unNr exceeds the minimal number of "1"
    constexpr bool isPreambleValid(unsigned int unNr) { return unNr >= getPreambleMinNrOnes(); }

    /// trigger state machine with an event (received bit)
    void execute(eBit bitRcv);

    /// State machine handle function for STATE_PREAMBLE 
    eState executePreamble(eBit bitRcv);
    /// State machine handle function for STATE_DATA 
    eState executeData(eBit bitRcv);

    /// Number of "1" received
    unsigned int unNrOnePreamble;
    /// Count received data bits for current packet per byte: first 8 bits are stored in a packet, 9th bit defines "packet finished" (1 bit) or "more bytes" (0 bit)
    /// Counts from 0 (nothing received yet) up to 9 (trailing 0 bit)
    unsigned int unNrBitsData;

    /// Reference for HandlerIfc interface (which is called if a packet is received)
    HandlerIfc& mHandlerIfc;

    /// Packet that is processed (received) currently
    MyPacket CurrentPacket;

  public:
    /// constructor
    PacketExtractor(HandlerIfc& hifc)
      : state(STATE_PREAMBLE)
      , unNrBitsData(0u)
      , mHandlerIfc(hifc)
    {
      invalid();
    }

    /// destructor
    virtual ~PacketExtractor() {}
    /// event trigger: "1" bit received
    constexpr void one() { execute(BIT_ONE); }
    /// event trigger: "0" bit received
    constexpr void zero() { execute(BIT_ZERO); }
    /// event trigger: Invalid received, reset
    void invalid()
    {
      state = STATE_PREAMBLE;
      unNrOnePreamble = 0u;
      unNrBitsData = 0u;
      CurrentPacket.clear();
    }
  };

  // ---------------------------------------------------
  /// State function: check if a valid preamble is transmitted:
  /// Sequence of at least 10x "1", followed by a "0" 
  // ---------------------------------------------------
  template<int PreambleMinNrOnes>
  typename PacketExtractor<PreambleMinNrOnes>::eState PacketExtractor<PreambleMinNrOnes>::executePreamble(eBit bitRcv)
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
      if (isPreambleValid(unNrOnePreamble))
      {
        nextState = STATE_DATA;
      }
      // reset counter because
      // - min number of "1" not reached, invalid or waiting for first "1"
      // - or valid preamble detected, switch to STATE_DATA but prepare next switch to STATE_PREAMBLE
      unNrOnePreamble = 0;
    }

    return nextState;
  }

  // ---------------------------------------------------
  /// State function: Interpret adress or data bytes bit by bit.
  // ---------------------------------------------------
  template<int PreambleMinNrOnes>
  typename PacketExtractor<PreambleMinNrOnes>::eState PacketExtractor<PreambleMinNrOnes>::executeData(eBit bitRcv)
  {
    eState nextState = state;

    if (unNrBitsData < 8u)
    {
      CurrentPacket.addBit((unsigned int)bitRcv);
      unNrBitsData++;
    }
    else
    {
      unNrBitsData = 0u;

      // Bit 0 is expected at the end of a data / address byte
      // If a 1 bit is received instead of a 0 bit, the packet is finished and the next packet is to be received
      if (bitRcv == BIT_ONE)
      {
        nextState = STATE_PREAMBLE;
        // notify handler about new packet
        mHandlerIfc.packetReceived(CurrentPacket);
        // prepare next reception
        CurrentPacket.clear();
      }
    }

    return nextState;
  }

  // ---------------------------------------------------
  /// trigger state machine
  // ---------------------------------------------------
  template<int PreambleMinNrOnes>
  void PacketExtractor<PreambleMinNrOnes>::execute(eBit bitRcv)
  {
    switch (state)
    {
    case STATE_PREAMBLE: { state = executePreamble(bitRcv); } break;
    case STATE_DATA: { state = executeData(bitRcv); } break;
    default: {} break;
    }
  }

} // namespace Dcc

#endif // DCC_PACKETEXTRACTOR_H