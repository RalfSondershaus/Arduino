/**
  *
  */
#ifndef DCCINTERPRETER_H
#define DCCINTERPRETER_H

namespace Dcc
{

  /// max number of (adress or data) bytes within a single packet
  #define DCCINTERPRETER_MAXBYTES       6u
  /// max number of packets that can be stored
  #define DCCINTERPRETER_MAXPACKETS    50u

    /// buffer for received packets
  class Packet
  {
  public:
    /// Constructor
    Packet() : unNrBits(0), unNrRcv(0) { clear(); }
    /// number of bits received
    unsigned int unNrBits;
    /// how often was this packet received?
    unsigned int unNrRcv;
    /// byte array
    unsigned char aBytes[DCCINTERPRETER_MAXBYTES];
    /// clear all data
    void clear()
    {
      int idx;

      unNrBits = 0;

      for (idx = 0; idx < DCCINTERPRETER_MAXBYTES; idx++)
      {
        aBytes[idx] = (unsigned char)0u;
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
    unsigned int getNrBytes() const { return (unNrBits + 7u) / 8u; }
    /// equality
    bool operator==(const Packet& p) const
    {
      bool bRet;
      unsigned int i;

      bRet = (p.unNrBits == unNrBits);
      for (i = 0; i < byteIdx(); i++)
      {
        bRet = bRet && (p.aBytes[i] == aBytes[i]);
      }

      return bRet;
    }
  };

  // ---------------------------------------------------
  /// A simple container class which supports
  /// - begin
  /// - end
  /// - size
  /// - push_back (max number of elements is limited, no dynamic memory allocation)
  /// and other functions as known from STL containers
  // ---------------------------------------------------
  template<class MemberClass_, int MaxSize_>
  class Container
  {
  public:
    /// MemberClass
    typedef MemberClass_ MemberClass;
    /// This class
    typedef Container<MemberClass, MaxSize_> This;
    /// iterators
    typedef MemberClass *       iterator;
    typedef const MemberClass * const_iterator;
    /// Constructor
    Container() : unSize(0)
    {}
    /// Destructor
    ~Container()
    {}
    /// begin
    iterator begin()             { return (unSize > 0) ? (&(aMembers[0])) : ((iterator) 0); }
    const_iterator begin() const { return (unSize > 0) ? (&(aMembers[0])) : ((iterator) 0); }
    /// end
    iterator end()               { return (unSize > 0) ? (&(aMembers[unSize])) : ((iterator) 0); }
    const_iterator end() const   { return (unSize > 0) ? (&(aMembers[unSize])) : ((iterator) 0); }
    /// push back
    void push_back(const MemberClass& m)
    {
      if (unSize < MaxSize_)
      {
        aMembers[unSize] = m;
        unSize++;
      }
    }
    /// back
    Packet& back()             { return aMembers[unSize - 1u]; }
    const Packet& back() const { return aMembers[unSize - 1u]; }
    /// size
    unsigned int size() const { return unSize; }
    /// clear
    void clear() { unSize = 0u; }
    /// find
    iterator find(const Packet& p)
    {
      iterator it;
      for (it = begin(); it != end(); it++)
      {
        if (*it == p)
        {
          break;
        }
      }
      return it;
    }
  protected:
    /// members
    MemberClass aMembers[MaxSize_];
    /// number of used elements
    unsigned int unSize;
  };

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
    /// Array of packets
    typedef Container<Packet, DCCINTERPRETER_MAXPACKETS> PacketContainer;

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

    /// trigger state machine with an event (received bit)
    void execute(eBit bitRcv);

    /// State machine handle function for STATE_PREAMBLE 
    eState executePreamble(eBit bitRcv);
    /// State machine handle function for STATE_DATA 
    eState executeData(eBit bitRcv);

    /// Store the received packet into the list of packets (if it does not exist already)
    void packetReceived(const Packet& pkt);

    /// Number of "1" received
    unsigned int unNrOnePreamble;
    /// Count received data bits for current packet per byte: first 8 bits are stored in a packet, 9th bit defines "packet finished" (1 bit) or "more bytes" (0 bit)
    /// Counts from 0 (nothing received yet) up to 9 (trailing 0 bit)
    unsigned int unNrBitsData;

    /// received valid packets
    PacketContainer aPackets;
    /// Packet that is processed (received) currently
    Packet CurrentPacket;

  public:
    /// constructor
    DccInterpreter();
    /// destructor
    ~DccInterpreter();
    /// event trigger: "1" bit received
    void one();
    /// event trigger: "0" bit received
    void zero();
    /// event trigger: Invalid received, reset
    void invalid();
    /// access to packets
    PacketContainer& refPacketContainer() { return aPackets; }
    const PacketContainer& refPacketContainer() const { return aPackets; }
  };

} // namespace Dcc

#endif // DCCINTERPRETER_H