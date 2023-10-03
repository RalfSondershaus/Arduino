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
#include <Util/Fix_Queue.h>
#include <Arduino.h>
#include <OS_Type.h>  // SuspendAllInterrupts, ResumeAllInterrupts, ISR macros

namespace dcc
{
  /// buffer size. Consider 60 interrupts / 1.5 ms = 180 / 4.5 ms ~ 200 / 5 ms
  static constexpr uint16 kTimeFifoSize = 200U;

  /// A FIFO (queue) is used to exchange data between ISR and dcc::Decoder.
  /// Arduino's implementation of micros() returns an unsigned long, so we use unsigned long here instead of uint32 or a similar type.
  typedef util::fix_queue<unsigned long, kTimeFifoSize> TimeDiffFifo;

  /// Share data between ISR and DccDecoder::loop
  /// The time stamp ring buffer
  static TimeDiffFifo DccTimeDiffFifo;

  /// For debugging: the number of interrupt ISR_Dcc calls (can overflow)
  uint16 un_ISR_Dcc_Count;

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
      DccTimeDiffFifo.push(ulTimeDiff);
    }
    ulTimeStampPrev = ulTimeStamp;

    // for debugging
    un_ISR_Dcc_Count++;
  }

  // ---------------------------------------------------
  /// Initialize
  // ---------------------------------------------------
  void Decoder::init(uint8 ucIntPin)
  {
    attachInterrupt(digitalPinToInterrupt(ucIntPin), ISR_Dcc, CHANGE);
  }

  // ---------------------------------------------------
  /// Loop
  // ---------------------------------------------------
  void Decoder::fetch()
  {
    unsigned long ulTimeDiff;
    bool bReceived;

    SuspendAllInterrupts();
    bReceived = !DccTimeDiffFifo.empty();
    ResumeAllInterrupts();

    while (bReceived)
    {
      SuspendAllInterrupts();
      ulTimeDiff = DccTimeDiffFifo.front();
      ResumeAllInterrupts();

      bitExtr.execute(ulTimeDiff); // informs the handler (if any)

      SuspendAllInterrupts();
      DccTimeDiffFifo.pop();
      bReceived = !DccTimeDiffFifo.empty();
      ResumeAllInterrupts();
    }
  }

  /// for debugging: number of interrupt (ISR) calls
  uint16 Decoder::getNrInterrupts() const
  {
    return un_ISR_Dcc_Count;
  }

} // namespace dcc

// EOF