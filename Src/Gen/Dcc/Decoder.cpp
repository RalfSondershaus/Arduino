/**
  * @file Decoder.cpp
  *
  * @author Ralf Sondershaus
  *
  * @brief DCC Decoder for Arduino
  *
  * Declares class Dcc::Decoder with
  * - setup
  * - loop
  */

#include <Std_Types.h>
#include <Dcc/Decoder.h>
#include <Util/RingBuffer.h>
#include <OS_Type.h>  // SuspendAllInterrupts, ResumeAllInterrupts, ISR macros

namespace Dcc
{
  /// buffer size. Consider 60 interrupts / 1.5 ms = 180 / 4.5 ms ~ 200 / 5 ms
  static constexpr uint16 unTimeBufferSize = 200U;

  /// A ring buffer is used to exchange data between ISR and Dcc::Decoder::loop.
  /// Arduino's implementation of micros() returns an unsigned long, so we use unsigned long here instead of uint32 or a similar type.
  typedef RingBuffer<unsigned long, unTimeBufferSize> TimeDiffBuffer;

  /// Share data between ISR and DccDecoder::loop
  /// The time stamp ring buffer
  static TimeDiffBuffer DccTimeDiffBuffer;

  /// for debugging: 
  /// [0]: Number of interrupt (ISR) calls
  /// [1]: Number of bitExtr.execute
  uint32 ulDebugVal[10] = { 0 };

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
    ulDebugVal[0]++;
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
      bitExtr.execute(ulTimeDiff); // informs the handler (if any)
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