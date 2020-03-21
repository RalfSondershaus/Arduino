/**
  *
  */
#ifndef DCCINTERPRETER_H
#define DCCINTERPRETER_H

namespace Dcc
{

  /// max number of (adress or data) bytes within a single packet
  #define DCCINTERPRETER_MAXBYTES       5u
  /// max number of packets
  #define DCCINTERPRETER_MAXPACKETS   100u

  // ---------------------------------------------------
  /// Interpret a stream of bits<BR>
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
  class DccInterpreter
  {
  public:
    /// buffer for received packets
    class Packet
    {
    public:
      /// Constructor
      Packet() : unNrBits(0) {}
      /// number of bits received
      unsigned int unNrBits;
      /// byte array
      unsigned char aBytes[DCCINTERPRETER_MAXBYTES];
      /// clear all data
      void clear() 
      { 
        int idx;

        unNrBits = 0;

        for (idx = 0; idx < DCCINTERPRETER_MAXBYTES; idx++)
        {
          aBytes[idx] = (unsigned char) 0u;
        }
      }
      /// Return current byte index (index into array of bytes)
      unsigned int byteIdx() const { return unNrBits / 8u; }
      /// Return current bit index (in current byte byteIdx())
      unsigned int bitIdx() const { return unNrBits % 8u; }
      /// add a bit (0 or 1)
      void addBit(unsigned int unBit)
      {
        aBytes[byteIdx()] = (aBytes[byteIdx()] << 1u) | unBit;
        unNrBits++;
      }
      /// get a single byte
      const unsigned char refByte(unsigned int idx) { return aBytes[idx]; }
      /// get the number of used bytes
      unsigned int getNrBytes() const { return (unNrBits + 7u) / 8u;  }
    };
  protected:
    /// debugging: total number of "1" received
    unsigned int unNrOne;
    /// debugging: total number of "0" received
    unsigned int unNrZero;

    /// State of reception
    typedef enum
    {
      STATE_PREAMBLE  = 0,   /// Receiving preamble
      STATE_DATA      = 1,   /// Receiving adress or data bytes
      STATE_MAX_COUNT = 2
    } eState;

    /// Bit "0", Bit "1" or invalid bit received
    typedef enum
    {
      BIT_ZERO = 0,
      BIT_ONE = 1
    } eBit;

    /// Current state
    eState state;

    /// trigger state machine
    void execute(eBit bitRcv);

    /// State machine handle function for STATE_PREAMBLE 
    eState executePreamble(eBit bitRcv);
    /// State machine handle function for STATE_DATA 
    eState executeData(eBit bitRcv);

    /// Number of "1" received
    unsigned int unNrOnePreamble;
    /// Count received data bits: first 8 bits are stored in a packet, 9th bit defines "packet finished" (1 bit) or "more bytes" (0 bit)
    unsigned int unNrBitsData;

    /// switch to the next packet (ring buffer)
    void nextPacket();
    /// received packets
    Packet aPackets[DCCINTERPRETER_MAXPACKETS];
    /// current packet
    unsigned int unPacket;

  public:
    /// constructor
    DccInterpreter();
    /// destructor
    ~DccInterpreter();
    /// "1" bit received
    void one();
    /// "0" bit received
    void zero();
    /// Invalid received, reset
    void invalid();
    /// get a packet
    const Packet& refPacket(unsigned int idx) const { return aPackets[idx]; }
    Packet& refPacket(unsigned int idx) { return aPackets[idx]; }
    const Packet& refCurrentPacket() const { return refPacket(unPacket); }
    Packet& refCurrentPacket() { return refPacket(unPacket); }
    /// debugging: return number of "1" received so far
    unsigned int getNrOne()  const { return unNrOne; }
    /// debugging: return number of "0" received so far
    unsigned int getNrZero() const { return unNrZero; }
  };

} // namespace Dcc

#endif // DCCINTERPRETER_H