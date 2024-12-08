/**
 * @file PacketExtractor.h
 */

#ifndef DCC_PACKETEXTRACTOR_H
#define DCC_PACKETEXTRACTOR_H

#include <Std_Types.h>
#include <Dcc/Packet.h>

namespace dcc
{
  // ---------------------------------------------------
  /// Extract a packet from a bit stream.<BR>
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
    /// This class
    typedef PacketExtractor<PreambleMinNrOnes> This;
    /// The Packet type
    typedef Packet<> PacketType;
    /// size type
    typedef size_t size_type;

    /// Interface for a handler. Such a handler is called when a new packet is available
    class HandlerIfc
    {
    public:
      /// The packets
      typedef This::PacketType PacketType;
      /// Construct and destruct
      HandlerIfc() {}
      virtual ~HandlerIfc() {}
      /// If a new packet is available, this function is called
      /// The parameter pkt is not const to enable the handler to
      /// modify the received packet, e.g. to call decode() for
      /// address calculation.
      virtual void packetReceived(PacketType& pkt) = 0;
    };

  protected:
    /// Interpretation state
    typedef enum
    {
      STATE_PREAMBLE  = 0,   ///< Receiving preamble
      STATE_DATA      = 1,   ///< Receiving adress or data bytes
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
    static constexpr size_type getPreambleMinNrOnes() { return static_cast<size_type>(PreambleMinNrOnes); }

    /// Returns true if unNr exceeds the minimal number of "1"
    constexpr bool isPreambleValid(size_type unNr) const { return unNr >= getPreambleMinNrOnes(); }

    /// trigger state machine with an event (received bit)
    void execute(eBit bitRcv);

    /// State machine handle function for STATE_PREAMBLE 
    eState executePreamble(eBit bitRcv);
    /// State machine handle function for STATE_DATA 
    eState executeData(eBit bitRcv);

    /// Number of "1" received
    uint8_least ucNrOnePreamble;
    /// Count received data bits for current packet per byte: first 8 bits are stored in a packet, 9th bit defines "packet finished" (1 bit) or "more bytes" (0 bit)
    /// Counts from 0 (nothing received yet) up to 9 (trailing 0 bit)
    uint8_least ucNrBitsData;

    /// Reference for HandlerIfc interface. This interface is called as soon as a new packet is available.
    HandlerIfc& mHandlerIfc;

    /// Packet that is processed (received) currently
    PacketType CurrentPacket;

  public:
    /// constructor
    PacketExtractor(HandlerIfc& hifc)
      : state(STATE_PREAMBLE)
      , ucNrBitsData(0u)
      , mHandlerIfc(hifc)
    {
      invalid();
    }
    uint32 ulOnes;
    uint32 ulZeros;
    uint32 ulInvalids;
    /// destructor
    ~PacketExtractor() = default;
    /// event trigger: "1" bit received
    void one() { ulOnes++; execute(BIT_ONE); }
    /// event trigger: "0" bit received
    void zero() { ulZeros++; execute(BIT_ZERO); }
    /// event trigger: Invalid received, reset
    void invalid()
    {
      ulInvalids++;
      state = STATE_PREAMBLE;
      ucNrOnePreamble = 0u;
      ucNrBitsData = 0u;
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
      if (ucNrOnePreamble < static_cast<uint16>(255))
      {
        ucNrOnePreamble++;
      }
    }
    else // BIT_ZERO
    {
      if (isPreambleValid(ucNrOnePreamble))
      {
        nextState = STATE_DATA;
      }
      // reset counter because
      // - min number of "1" not reached, invalid or waiting for first "1"
      // - or valid preamble detected, switch to STATE_DATA but prepare next switch to STATE_PREAMBLE
      ucNrOnePreamble = 0;
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

    if (ucNrBitsData < 8u)
    {
      CurrentPacket.addBit(static_cast<uint8>(bitRcv));
      ucNrBitsData++;
    }
    else
    {
      ucNrBitsData = 0u;

      // Bit 0 is expected at the end of a data / address byte
      // If a 1 bit is received instead of a 0 bit, the packet is finished and the next packet is to be received
      if (bitRcv == BIT_ONE)
      {
        nextState = STATE_PREAMBLE;
        // store number of preamble "1" in the packet; can be optimized (use CurrentPacket.ucNrOnePreamble instead of ucNrOnePreamble)
        CurrentPacket.ucNrOnePreamble = ucNrOnePreamble;
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

} // namespace dcc

#endif // DCC_PACKETEXTRACTOR_H