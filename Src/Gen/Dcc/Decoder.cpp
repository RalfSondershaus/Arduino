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
#include <Util/Algorithm.h> // max
#include <Util/Fix_Queue.h>
#include <Hal/Timer.h>
#include <Hal/Interrupt.h>
#include <OS_Type.h>  // SuspendAllInterrupts, ResumeAllInterrupts, ISR macros

namespace dcc
{
  /// buffer size. Consider 60 interrupts / 1.5 ms = 180 / 4.5 ms ~ 200 / 5 ms ~ 400 / 10 ms
  static constexpr uint16 kTimeBufferSize = 400U;

  /// A FIFO (queue) is used to exchange data between ISR and dcc::Decoder.
  /// Arduino's implementation of micros() returns an unsigned long, so we use unsigned long here instead of uint32 or a similar type.
  using time_diff_deque = util::fix_deque<uint8, kTimeBufferSize>;

  static constexpr time_diff_deque::value_type kTimeDeltaInvalid = platform::numeric_limits<time_diff_deque::value_type>::max_();
  static constexpr time_diff_deque::value_type kTimeDeltaLastValid = platform::numeric_limits<time_diff_deque::value_type>::max_() - 1;

  /// Share data between ISR and DccDecoder::loop
  /// The buffers for time deltas
  static time_diff_deque TimeDiffDeque[2];
  
  /// Selects TimeDiffDeque 0 or 1.
  static uint8 DoubleBufferIdx;
  
  static uint16 TimeDiffDequeMaxSize = 0;

  /// For debugging: the number of interrupt ISR_Dcc calls (can overflow)
  uint32 ul_ISR_Dcc_Count;
  uint32 ul_Fetch_Count;

  /// Encode dt into time_diff_deque::value_type:
  /// If dt is greater than maximum value for long intervals (e.g. 10000),
  /// dt is set to the maximal value that can be represented by time_diff_deque::value_type.
  /// Otherwise, dt is limited to the maximal value - 1.
  /// For time_diff_deque::value_type = uint8:
  /// dt  > 10000 -> 255
  /// dt <= 10000 -> dt but with dt > 254 -> 254
  static inline time_diff_deque::value_type limitTimeDelta(unsigned long dt)
  {
    if (dt > Decoder::BitExtractorConstantsType::getPartTimeLongMax())
    {
      dt = kTimeDeltaInvalid;
    }
    else
    {
      dt = util::min_<unsigned long>(kTimeDeltaLastValid, dt);
    }
    return static_cast<time_diff_deque::value_type>(dt);
  }

  /// For time_diff_deque::value_type = uint8:
  /// 255 -> 10001
  /// 0 ... 254 -> 0 ... 254
  static inline unsigned long delimitTimeDelta(time_diff_deque::value_type dt)
  {
    unsigned long ret = dt;

    if (dt == kTimeDeltaInvalid)
    {
      ret = Decoder::BitExtractorConstantsType::getPartTimeLongMax() + 1;
    }

    return ret;
  }

  // ---------------------------------------------------
  /// Interrupt service routine: a falling or rising edge has triggered this interrupt.
  /// Write the time difference into the FIFO buffer.
  /// @note Average run time 27 usec @ATmega2560 @16 MHz with gcc -Os
  /// @note Average run time 14 usec @ATmega2560 @16 MHz with gcc -O3
  // ---------------------------------------------------
  ISR(ISR_Dcc)
  {
    static unsigned long ulTimeStampPrev = 0;
    unsigned long ulTimeStamp = hal::micros();
    unsigned long ulTimeDiff;

    if (ulTimeStampPrev > 0u)
    {
      // second call or beyond
      time_diff_deque& currentDeque = TimeDiffDeque[DoubleBufferIdx];
      // for debugging
      TimeDiffDequeMaxSize = util::max_<uint16>(TimeDiffDequeMaxSize, currentDeque.size());
      if (currentDeque.size() < currentDeque.max_size())
      {
        ulTimeDiff = ulTimeStamp - ulTimeStampPrev;
        currentDeque.push_back(limitTimeDelta(ulTimeDiff));
      }
      else
      {
        // TBD
      }
    }
    ulTimeStampPrev = ulTimeStamp;

    // for debugging
    ul_ISR_Dcc_Count++;
  }

  void Decoder::isrGetStats(IsrStats& stat) const noexcept
  {
    SuspendAllInterrupts();
    stat.curSize = static_cast<uint16>(TimeDiffDeque[DoubleBufferIdx].size());
    stat.maxSize = TimeDiffDequeMaxSize;
    ResumeAllInterrupts();
  }

  bool Decoder::isrOverflow() const noexcept
  {
    return TimeDiffDequeMaxSize >= TimeDiffDeque[0].max_size();
  }

  // ---------------------------------------------------
  /// Initialize
  // ---------------------------------------------------
  void Decoder::init(uint8 ucIntPin)
  {
    hal::attachInterrupt(digitalPinToInterrupt(ucIntPin), ISR_Dcc, CHANGE);
  }

  // ---------------------------------------------------
  /// Loop
  // ---------------------------------------------------
  void Decoder::fetch()
  {
    time_diff_deque& oldDeque = TimeDiffDeque[DoubleBufferIdx];

    SuspendAllInterrupts();
    DoubleBufferIdx = (DoubleBufferIdx == 0) ? 1 : 0;
    ResumeAllInterrupts();

    while (!oldDeque.empty())
    {
      ul_Fetch_Count++;
      bitExtr.execute(delimitTimeDelta(oldDeque.front())); // informs the handler (if any)
      oldDeque.pop_front();
    }
  }

  /// for debugging: number of interrupt (ISR) calls
  uint32 Decoder::getNrInterrupts() const
  {
    return ul_ISR_Dcc_Count;
  }
  uint32 Decoder::getNrFetches() const
  {
    return ul_Fetch_Count;
  }

} // namespace dcc

// EOF