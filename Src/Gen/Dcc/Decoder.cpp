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
  static constexpr uint16 kBitStreamSize = 400U;
  using bit_extractor_constants = BitExtractorConstants<>;
  /// A FIFO (queue) is used to exchange data between ISR and dcc::Decoder.
  using bit_stream_type = BitStream<kBitStreamSize>;
  using bit_extractor_type = BitExtractor<bit_extractor_constants, bit_stream_type>;

  /// Share data between ISR and DccDecoder::loop
  /// The buffers for bits are double-buffered to allow
  /// the ISR to write into one buffer while the main loop reads from the other.
  /// The double buffering is implemented with a static index.
  /// The index is toggled in the ISR and in the main loop.
  static bit_stream_type bit_streams[2];
  
  static bit_extractor_type bitExtr(&bit_streams[0]); ///< The bit extractor that processes the timing intervals and generates bits

  /// Selects bit_streams by index.
  static uint8 ucDoubleBufferIdx;

  
  /// For debugging: the number of interrupt ISR_Dcc calls (can overflow)
  static uint16 unBitStreamMaxSize = 0;
  uint32 ul_ISR_Dcc_Count;
  uint32 ul_Fetch_Count;

  // ---------------------------------------------------
  /// Interrupt service routine: a falling or rising edge has triggered this interrupt.
  /// Write the time difference into the FIFO buffer.
  /// @note Average run time 27 usec @ATmega2560 @16 MHz with gcc -Os
  /// @note Average run time 14 usec @ATmega2560 @16 MHz with gcc -O3
  // ---------------------------------------------------
  ISR(ISR_Dcc)
  {
    static bool bFirstCall = true;
    static unsigned long prev = 0;
    const unsigned long now = hal::micros();
    unsigned long dt;

    if (bFirstCall)
    {
      // first call
      bFirstCall = false;
    }
    else
    {
      // second call or beyond

      // This is the time delta in microseconds
      // Note: ULONG_MAX is the maximum value for an unsigned long, which is 4294967295 on most platforms.
      // This calculation handles the wrap-around case correctly.
      const unsigned long dt = (now >= prev) ? 
                               (now - prev) : 
                               (now + (platform::numeric_limits<unsigned long>::max_() - prev) + 1UL);
      // execute the state machine with the time delta
      // push 0, 1, or invalid into the underlying bit stream
      bitExtr.execute(dt); 

      // for debugging
      unBitStreamMaxSize = util::max_<uint16>(unBitStreamMaxSize, bitExtr.refBitStream().size());
    }
    prev = now;

    // for debugging
    ul_ISR_Dcc_Count++;
  }

  void Decoder::isrGetStats(IsrStats& stat) const noexcept
  {
    SuspendAllInterrupts();
    stat.curSize = static_cast<uint16>(bitExtr.refBitStream().size());
    stat.maxSize = unBitStreamMaxSize;
    ResumeAllInterrupts();
  }

  bool Decoder::isrOverflow() const noexcept
  {
    return unBitStreamMaxSize >= bitExtr.refBitStream().max_size();
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
    bit_stream_type& bs = bitExtr.refBitStream();
    SuspendAllInterrupts();
    ucDoubleBufferIdx = (ucDoubleBufferIdx == 0) ? 1 : 0;
    bitExtr.setBitStream(&bit_streams[ucDoubleBufferIdx]); // switch to the other buffer
    ResumeAllInterrupts();

    while (!bs.empty())
    {
      if (bs.invFront())
      {
        pktExtr.invalid();
      }
      else
      {
        ul_Fetch_Count++;
        bs.front() ? pktExtr.one() : pktExtr.zero(); // informs the handler (if any)
      }
      bs.pop();
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