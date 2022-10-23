/// @file DccDecoder.cpp
///
/// DCC Decoder for Arduino
///
/// Implements
/// - setup
/// - loop

#include <Dcc/Decoder.h>
#include <Util/RingBuffer.h>
#include <OS_Type.h>  // SuspendAllInterrupts, ResumeAllInterrupts, ISR macros

namespace Dcc
{
  /// buffer size. Consider 60 interrupts / 1.5 ms = 180 / 4.5 ms ~ 200 / 5 ms
  #define TIME_BUFFER_SIZE  200U

  /// A ring buffer is used to exchange data between ISR and DccDecoder::loop.
  /// Arduino's implementation of micros() returns an unsigned long, so we use unsigned long here instead of uint32 or a similar type.
  typedef RingBuffer<unsigned long, TIME_BUFFER_SIZE> TimeDiffBuffer;

#if 0
  // ---------------------------------------------------
  /// Ring buffer for time stamps
  // ---------------------------------------------------
  class TimeStampBuffer
  {
  public:
    /// Constructor
    TimeStampBuffer() : unIdxRead(0u), unIdxWrite(0u), bBufferFull(false) {}
    /// Destructor
    ~TimeStampBuffer() {}
    /// called within loop() context, might be interrupted by a call to add().
    /// Returns true if a value is available (written into parameter val), return false otherwise (and keep parameter val unchanged)
    bool get(unsigned long& val)
    {
      bool bRet;

      noInterrupts();
      if (isValueAvailable())
      {
        val = aBuffer[unIdxRead];
        unIdxRead++;
        if (unIdxRead >= TIME_BUFFER_SIZE)
        {
          unIdxRead = 0;
        }
        bRet = true;
      }
      else
      {
        bRet = false;
      }
      interrupts();

      return bRet;
    }
    /// add an element; called within interrupt context
    void add(unsigned long unTimeDiff)
    {
      aBuffer[unIdxWrite] = unTimeDiff;
      unIdxWrite++;
      if (unIdxWrite >= TIME_BUFFER_SIZE)
      {
        unIdxWrite = 0;
      }
      if (unIdxWrite == unIdxRead)
      {
        clear();
        bBufferFull = true;
      }
    }
    /// Return true is buffer had an overflow, clear internal state flag for buffer overflow.
    bool isBufferFull() 
    { 
      bool bRet;

      noInterrupts();
      bRet = bBufferFull;
      bBufferFull = false;
      interrupts();

      return bRet;
    }
  protected:
    /// Return true if the next value can be read (has been written before)
    bool isValueAvailable()
    {
      return (unIdxRead != unIdxWrite) && (!bBufferFull);
    }
    /// Restart
    void clear()
    {
      unIdxRead = 0u;
      unIdxWrite = 0u;
    }
    /// The buffer contains time differences
    unsigned char aBuffer[TIME_BUFFER_SIZE];
    /// Current read index
    unsigned int unIdxRead;
    /// Current write index
    unsigned int unIdxWrite;
    /// Flag to signal that an overflow happened
    bool bBufferFull;
  };
#endif

  /// Share data between ISR and DccDecoder::loop
  /// The time stamp ring buffer
  static TimeDiffBuffer DccTimeDiffBuffer;

  /// for debugging: 
  /// [0]: Number of interrupt (ISR) calls
  /// [1]: Number of bitExtr.execute
  unsigned int unDebugVal[10] = { 0 };

  // ---------------------------------------------------
  /// Interrupt service routine: a falling or rising edge has triggered this interrupt.
  /// Write the time difference into the ring buffer.
  // ---------------------------------------------------
  ISR(ISR_Dcc)
  {
    static unsigned long ulTimeStampPrev = 0;
    unsigned long ulTimeStamp = micros();
    unsigned long ulTimeDiff;

    if (ulTimeStampPrev > 0u)
    {
      // second call or beyond
      ulTimeDiff = ulTimeStamp - ulTimeStampPrev;
      DccTimeDiffBuffer.add(ulTimeDiff);
    }
    ulTimeStampPrev = ulTimeStamp;

    // for debugging
    unDebugVal[0]++;
  }

  // ---------------------------------------------------
  /// Initialize
  // ---------------------------------------------------
  void Decoder::setup(uint8 ucIntPin)
  {
    attachInterrupt(digitalPinToInterrupt(ucIntPin), ISR_Dcc, CHANGE);
  }

  // ---------------------------------------------------
  /// Initialize
  // ---------------------------------------------------
  void Decoder::loop()
  {
    unsigned long ulTimeDiff;
    bool bReceived;
    
    SuspendAllInterrupts();
    bReceived = DccTimeDiffBuffer.get(ulTimeDiff);
    ResumeAllInterrupts();

    while (bReceived)
    {
      unDebugVal[1]++;
      bitExtr.execute(ulTimeDiff);
      SuspendAllInterrupts();
      bReceived = DccTimeDiffBuffer.get(ulTimeDiff);
      ResumeAllInterrupts();
    }

    SuspendAllInterrupts();
    const bool bIsFull = DccTimeDiffBuffer.isBufferFull();
    ResumeAllInterrupts();
    if (bIsFull)
    {
      // TODO reset periodSM
    }
  }

  /// for debugging: number of interrupt (ISR) calls
  unsigned int Decoder::getDebugVal(int i)
  {
    return unDebugVal[i];
  }

} // namespace Dcc

// EOF