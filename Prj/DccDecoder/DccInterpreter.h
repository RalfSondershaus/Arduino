/**
  *
  */
#ifndef DCCINTERPRETER_H
#define DCCINTERPRETER_H

/// max number of (adress or data) bytes within a single packet
#define DCCINTERPRETER_MAXBYTES       5u
/// max number of packets
#define DCCINTERPRETER_MAXPACKETS  1000u

// ---------------------------------------------------
/// Interpret a stream of bits<BR>
/// Minimum time for a packet (preamble + 2 bytes of data):<BR>
/// <CODE>
/// Preamble: 10x "1" + 1x "0"<BR>
/// Bytes: 8x "1" + 1x "0" (resp. + 1x "1" for last byte)<BR>
///     10x  52 us <BR>
/// +    1x  90 us <BR>
/// + 2x 8x  52 us <BR>
/// + 1x 1x  90 us <BR>
/// + 1x 1x  52 us <BR>
/// =     1.584 us <BR>
/// =       1.5 ms <BR>
/// </CODE>
// ---------------------------------------------------
class DccInterpreter
{
protected:
  /// debugging: total number of "1" received
  unsigned int unNrOne;
  /// debugging: total number of "0" received
  unsigned int unNrZero;

  /// State of reception
  typedef enum
  {
    STATE_PREAMBLE    = 0,   /// Receiving preamble
    STATE_BYTE        = 1,   /// Receiving adress or data bytes
    STATE_MAX_COUNT   = 2
  } tState;

  /// Bit "0" or Bit "1" received
  typedef enum
  {
    BIT_ONE   = 0,
    BIT_ZERO  = 1
  } tBit;

  /// Representation of a single state
  class StateBase
  {
  protected:
    DccInterpreter& DccInterp;
  public:
    /// Constructor
    StateBase(DccInterpreter& DccI) : DccInterp(DccI) {}
    /// entry function
    virtual void entry() = 0;
    /// execute state function
    virtual tState execFunc(tState curState, tBit bitReceived) = 0;
  };

  /// State STATE_PREAMBLE
  class StatePreamble : public StateBase                                       
  {                                                                    
  public:                                                     
    /// Number of "1" received
    unsigned int unNrOne;
    /// Constructor
    StatePreamble(DccInterpreter& DccI) : StateBase(DccI), unNrOne(0)
    {}
    /// entry function
    virtual void entry();
    /// Execute state function
    virtual tState execFunc(tState curState, tBit bitReceived);
  };

  /// State STATE_BYTE
  class StateByte : public StateBase
  {
  public:
    /// Constructor
    StateByte(DccInterpreter& DccI) : StateBase(DccI), unNrBytes(0), unNrBits(0)
    {}
    /// entry function
    virtual void entry();
    /// Execute state function
    virtual tState execFunc(tState curState, tBit bitReceived);
  };

  /// The states
  StatePreamble statePreamble;
  StateByte     stateByte;

  /// Current state
  tState state;

  /// Array of pointers to the states
  const StateBase * aStates[STATE_MAX_COUNT];

  /// trigger state machine
  void execute(tBit bitReceived);

  /// add a "1" to the current byte of current packet
  void addOne(unsigned int unByte)    { aPackets[unPacket].addOne(unByte); }
  /// add a "0" to the current byte of current packet
  void addZero(unsigned int unByte)   { aPackets[unPacket].addZero(unByte); }
  /// go to next byte of current packet
  void nextByte() { aPackets[unPacket].next();  }

  /// buffer for received packets
  class Packet
  {
    /// Constructor
    Packet() : unNrBits(0), unNrBytes(0) {}
    /// number of bits received
    unsigned int unNrBits;
    /// number of bytes received (index into aBytes)
    unsigned int unNrBytes;
    /// byte array
    unsigned char aBytes[DCCINTERPRETER_MAXBYTES];
    /// add a "1"
    void addOne(unsigned int unByte)  { aBytes[unByte] = (aBytes[unByte] << 1u) | 1u; }
    /// add a "0"
    void addZero(unsigned int unByte) { aBytes[unByte] = (aBytes[unByte] << 1u);  }
    /// switch to next byte
    void next() 
    { 
      if (unNrBytes < DCCINTERPRETER_MAXBYTES - 1u)
      {
        unNrBytes++;
        unNrBits = 0;
      }
    }
  };

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
  /// debugging: return number of "1" received so far
  unsigned int getNrOne()  const { return unNrOne;  }
  /// debugging: return number of "0" received so far
  unsigned int getNrZero() const { return unNrZero; }
  /// debugging: get bytes
  unsigned char getByte(unsigned int idx) const { return stateByte.aBytes[idx]; }
};

#endif // DCCINTERPRETER_H