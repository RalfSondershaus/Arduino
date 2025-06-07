/**
  * @file BitExtractor.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Defines class BitExtractor in namespace dcc
  */

#ifndef DCC_BITEXTRACTOR_H
#define DCC_BITEXTRACTOR_H

#include <Std_Types.h> // for uint32_t
#include <Dcc/PacketExtractor.h>
#include <Util/Array.h> // for debugging
#include <Util/Fix_Queue.h>   // For the bit stream queue
#include <Util/Ptr.h>   // for pointer to bit stream

namespace dcc
{
  // ---------------------------------------------------
  /// a helper class to define all constant values in a single class
  // ---------------------------------------------------
  template<
      int unPartTimeShortMin = 48   ///< [us] Minimum time between two edges for a "short" part (first or second part of a "1"). Spec: 52 us, with resolution of 4 us: 48 us
    , int unPartTimeShortMax = 68   ///< [us] Maximum time between two edges for a "short" part (first or second part of a "1"). Spec: 64 us, with resolution of 4 us: 68 us
    , int unPartTimeLongMin = 86    ///< [us] Minimum time between two edges for a "long" part (first or second part of a "0"). Spec: 90 us, with resolution of 4 us: 86 us
    , int unPartTimeLongMax = 10000 ///< [us] Maximum time between two edges for a "long" part (first or second part of a "0"). Typical value: 10000
  >
  class BitExtractorConstants
  {
  public:
    /// [us] Minimum time between two edges for a "short" part (first or second part of a "1"). Spec: 52 us, with resolution of 4 us: 48 us
    static constexpr uint32_t getPartTimeShortMin() { return unPartTimeShortMin; }
    /// [us] Maximum time between two edges for a "short" part (first or second part of a "1"). Spec: 64 us, with resolution of 4 us: 68 us
    static constexpr uint32_t getPartTimeShortMax() { return unPartTimeShortMax; }
    /// [us] Minimum time between two edges for a "long" part (first or second part of a "0"). Spec: 90 us, with resolution of 4 us: 86 us
    static constexpr uint32_t getPartTimeLongMin() { return unPartTimeLongMin; }
    /// [us] Maximum time between two edges for a "long" part (first or second part of a "0"). Typical value: 10000
    static constexpr uint32_t getPartTimeLongMax() { return unPartTimeLongMax; }
  };

  /**
   * @class BitStream
   * @brief A fixed-size bit stream with parallel invalid bit tracking for digital protocols.
   *
   * The BitStream class manages a queue of bits and a parallel queue for marking invalid bits,
   * supporting bit-level operations and error tracking. It is intended for use in digital
   * communication protocols (such as DCC decoders) where both the value and validity of each bit
   * must be tracked.
   *
   * Features:
   * - Fixed maximum size, determined by the template parameter N (default: kBitStreamSize).
   * - Efficient push and pop operations for both bit and invalid bit queues.
   * - Query current size, maximum capacity, and front elements of both queues.
   * - Methods to handle state changes when valid or invalid bit sequences are detected.
   *
   * @tparam N The maximum number of bits the stream can hold (default: kBitStreamSize).
   *
   * @note The underlying queue implementation is util::fix_queue_bool<N>.
   * @note Invalid bits are tracked in parallel with the main bit queue.
   *
   * Example usage:
   * @code
   * BitStream<128> stream;
   * stream.push(true, false); // Push a valid '1' bit
   * stream.push(false, true); // Push an invalid '0' bit
   * if (!stream.empty()) {
   *   bool bit = stream.front();
   *   bool invalid = stream.invFront();
   *   stream.pop();
   * }
   * @endcode
   */
  template<uint16 N>
  class BitStream
  {
  public:
    /// @brief Alias for a fixed-size boolean queue used in the bit stream.
    using queue_type = util::fix_queue_bool<N>;

    /// @brief The maximum size of the bit stream.
    /// This is a compile-time constant that defines the maximum number of bits that can be stored in the bit stream.
    /// @note The maximum size is set to N, which is the template parameter for the BitStream class.
    static constexpr uint16 kMaxSize = N;

    BitStream() = default;
    /// @brief Returns the number of bits currently stored in the bit stream.
    /// @return The number of bits in the bit stream.
    uint16 size() const noexcept
    {
      return static_cast<uint16>(bitQueue.size());
    }
    /// @brief Returns the maximum size of the bit stream.
    /// @return The maximum number of bits that can be stored in the bit stream.
    constexpr uint16 max_size() noexcept
    {
      return static_cast<uint16>(bitQueue.max_size());
    }
    /// @brief Adds a bit to the end of the queue
    /// @param bit The bit to add (true for "1", false for "0").
    void push(bool bit, bool invBit) noexcept
    {
      if (bitQueue.size() < bitQueue.max_size())
      {
        bitQueue.push(bit);
        invQueue.push(invBit);
      }
    }
    /// @brief Removes the first bit from the bit stream.
    ///
    /// This function removes the front element from both the bit queue and the invalid bits queue.
    /// It should be called when the first bit in the stream has been processed and is no longer needed.
    ///
    /// @note If the queues are already empty, calling this function may result in undefined behavior.
    void pop() noexcept
    {
      bitQueue.pop();
      invQueue.pop();
    }
    /// @brief Checks if the bit stream is empty.
    /// @return true if the bit stream is empty, false otherwise.
    bool empty() const noexcept
    {
      return bitQueue.empty();
    }
    /// @brief Returns the first bit in the bit stream.
    /// @return The first bit in the bit stream.
    bool front() const noexcept
    {
      return bitQueue.front();
    }
    /// @brief Returns the first invalid bit in the invalid bits queue.
    /// @return The first invalid bit in the invalid bits queue.
    bool invFront() const noexcept
    {
      return invQueue.front();
    }

    /// Called when an invalid bit sequence is detected (state machine reset)
    void invalid() { push(false, true); }
    /// Called when a valid "1" bit is detected
    void one() { push(true, false); }
    /// Called when a valid "0" bit is detected
    void zero() { push(false, false); }

  private:
    /// The queue that stores the bits
    queue_type bitQueue;
    /// Queue for invalid bits
    queue_type invQueue; 
  };

  /**
   * @brief Generates bits from short and long ticks ("half bits") according to DCC timing.
   *
   * This class implements a state machine that interprets timing intervals (ticks) as DCC bits ("1" or "0")
   * based on configurable timing constants. The resulting bits are forwarded as events to an instance of the
   * BitStream class (template parameter).
   *
   * @details
   * The BitExtractor receives timing intervals (in microseconds) and classifies them as "short", "long", or "invalid"
   * according to the DCC specification. It then transitions its internal state machine and triggers the appropriate
   * event on the BitStream instance.
   *
   * The following interface (member functions) must be provided by the BitStream template parameter:
   * - void invalid();    ///< Called when an invalid bit sequence is detected (state machine reset)
   * - void one();        ///< Called when a valid "1" bit is detected
   * - void zero();       ///< Called when a valid "0" bit is detected
   */
  template<class TBitExtractorConstants, class TBitStream>
  class BitExtractor
  {
  protected:
    /// This class
    typedef BitExtractor<TBitExtractorConstants, TBitStream> This;
    using bitstream_type = TBitStream;
    using pointer_type = typename util::ptr<bitstream_type>;
    using reference_type = bitstream_type&;
    using const_reference_type = const bitstream_type&;

    /// [us] Minimum time between two edges for a "short" part (first or second part of a "1"). Spec: 52 us, with resolution of 4 us: 48 us
    static constexpr uint32_t getPartTimeShortMin() { return TBitExtractorConstants::getPartTimeShortMin(); }
    /// [us] Maximum time between two edges for a "short" part (first or second part of a "1"). Spec: 64 us, with resolution of 4 us: 68 us
    static constexpr uint32_t getPartTimeShortMax() { return TBitExtractorConstants::getPartTimeShortMax(); }
    /// [us] Minimum time between two edges for a "long" part (first or second part of a "0"). Spec: 90 us, with resolution of 4 us: 86 us
    static constexpr uint32_t getPartTimeLongMin() { return TBitExtractorConstants::getPartTimeLongMin(); }
    /// [us] Maximum time between two edges for a "long" part (first or second part of a "0"). Typical value: 10000
    static constexpr uint32_t getPartTimeLongMax() { return TBitExtractorConstants::getPartTimeLongMax(); }
    
    /// States of this state machine
    typedef enum
    {
      STATE_INVALID       = 0,      ///< No bit detected, phase not detected; this is the default state after startup
      STATE_SHORT_INIT_1  = 1,      ///< One short tick detected, no long tick detected since last INVALID
      STATE_SHORT_INIT_2  = 2,      ///< Two subsequent short ticks detected (bit value 1), no long tick detected since last INVALID
      STATE_LONG_INIT_1   = 3,      ///< One long tick detected, no short tick detected since last INVALID
      STATE_LONG_INIT_2   = 4,      ///< Two subsequent long ticks detected (bit value 0), no short tick detected since last INVALID
      STATE_SHORT_1       = 5,      ///< One short tick detected
      STATE_SHORT_2       = 6,      ///< Two subsequent short ticks detected (bit value 1)
      STATE_LONG_1        = 7,      ///< One long tick detected
      STATE_LONG_2        = 8,      ///< Two subsequent long ticks detected (bit value 0)
      STATE_MAX_COUNT     = 9       ///< Maximum number of states
    } eState;

    /// Type for received tick
    typedef enum
    {
      INVALID_HALFBIT = 0,
      SHORT_HALFBIT = 1,
      LONG_HALFBIT = 2
    } eHalfBit;

    /// For debugging: number of received interrupts per state (half bits)
    typedef util::array<uint32_t, STATE_MAX_COUNT> TickArray;

    /// Current state
    eState state;

    /// Previous state
    eState prevState;

    /// The bit stream that receives events
    pointer_type pBitStream;

    /// For debugging: number of received interrupts per state (half bits)
    TickArray tickcounts;

    /// Return half bit type (short, long, invalid)
    static eHalfBit checkTick(uint32_t ulTime) noexcept
    {
      eHalfBit halfBitRcv;

      if (isShortHalfBit(ulTime))
      {
        halfBitRcv = SHORT_HALFBIT;
      }
      else if (isLongHalfBit(ulTime))
      {
        halfBitRcv = LONG_HALFBIT;
      }
      else
      {
        halfBitRcv = INVALID_HALFBIT;
      }

      return halfBitRcv;
    }

    // ---------------------------------------------------
    /// S-9.1: In a "1" bit, the first and last part of a bit shall have the same duration, and that duration shall nominally be 58 microseconds2, giving
    /// 15 the bit a total duration of 116 microseconds. Digital Command Station components shall transmit "1" bits with the first and last parts
    /// each having a duration of between 55 and 61 microseconds. A Digital Decoder must accept bits whose first and last parts have a
    /// duration of between 52 and 64 microseconds, as a valid bit with the value of "1".
    /// @param ulTime [us] Time between two edges
    // ---------------------------------------------------
    static constexpr bool isShortHalfBit(uint32_t ulTime) noexcept
    {
      return (ulTime >= getPartTimeShortMin()) && (ulTime <= getPartTimeShortMax());
    }

    // ---------------------------------------------------
    /// In a "0" bit, the duration of the first and last parts of each transition shall nominally be greater than or equal to 100 microseconds. To
    /// 20 keep the DC component of the total signal at zero as with the "1" bits, the first and last part of the "0" bit are normally equal to one
    /// another. Digital Command Station components shall transmit "0" bits with each part of the bit having a duration of between 95 and
    /// 9900 microseconds with the total bit duration of the "0" bit not exceeding 12000 microseconds. A Digital Decoder must accept bits,
    /// whose first or last parts have a duration between 90 and 10,000 microseconds, as a valid bit with the value of "0". Figure 1 provides
    /// an example of bits encoded using this technique.
    // ---------------------------------------------------
    static constexpr bool isLongHalfBit(unsigned long ulTime) noexcept
    {
      return (ulTime >= getPartTimeLongMin()) && (ulTime <= getPartTimeLongMax());
    }

  public:
    /// constructor
    BitExtractor(pointer_type pbs) : state(STATE_INVALID), pBitStream(pbs)
    {
      tickcounts.fill(0);
    }

    /// destructor
    ~BitExtractor() = default;

    void setBitStream(pointer_type pbs) noexcept { pBitStream = pbs;}
    reference_type refBitStream() noexcept  { return *pBitStream; }
    const_reference_type refBitStream() const noexcept  { return *pBitStream; }

    /// Execute state machine with a time difference (to last tick / interrupt, that is, to last half bit)
    void execute(uint32_t ulTimeDiff);
    /// For debugging: return number of calls for a state (< STATE_MAX_COUNT)
    uint32_t getNrCalls(unsigned int unState) const;
  };

  // ---------------------------------------------------
  /// Execute state machine with an event
  // ---------------------------------------------------
  template<class TBitExtractorConstants, class PacketGen>
  void BitExtractor<TBitExtractorConstants, PacketGen>::execute(uint32_t ulTimeDiff)
  {
    static const eState aTransitionMap[STATE_MAX_COUNT][3] =
    {     // received:  INVALID_HALFBIT, SHORT_HALFBIT     , LONG_HALFBIT
                      { STATE_INVALID  , STATE_SHORT_INIT_1, STATE_LONG_INIT_1 } // STATE_INVALID
                    , { STATE_INVALID  , STATE_SHORT_INIT_2, STATE_LONG_1      } // STATE_SHORT_INIT_1
                    , { STATE_INVALID  , STATE_SHORT_INIT_1, STATE_LONG_1      } // STATE_SHORT_INIT_2
                    , { STATE_INVALID  , STATE_SHORT_1     , STATE_LONG_INIT_2 } // STATE_LONG_INIT_1 ,     
                    , { STATE_INVALID  , STATE_SHORT_1     , STATE_LONG_INIT_1 } // STATE_LONG_INIT_2 ,
                    , { STATE_INVALID  , STATE_SHORT_2     , STATE_INVALID     } // STATE_SHORT_1       
                    , { STATE_INVALID  , STATE_SHORT_1     , STATE_LONG_1      } // STATE_SHORT_2     
                    , { STATE_INVALID  , STATE_INVALID     , STATE_LONG_2      } // STATE_LONG_1      
                    , { STATE_INVALID  , STATE_SHORT_1     , STATE_LONG_1      } // STATE_LONG_2      
    };

    eHalfBit halfBitRcv;

    prevState = state;

    halfBitRcv = checkTick(ulTimeDiff);
    state = aTransitionMap[state][static_cast<uint32_t>(halfBitRcv)];

    // for debugging
    tickcounts.at(static_cast<typename TickArray::size_type>(state))++;

    switch (state)
    {
    case STATE_INVALID:      { pBitStream->invalid(); } break;
    case STATE_SHORT_INIT_1: {                        } break;
    case STATE_SHORT_INIT_2: { pBitStream->one();     } break;
    case STATE_LONG_INIT_1:  {                        } break;
    case STATE_LONG_INIT_2:  { pBitStream->zero();    } break;
    case STATE_SHORT_1:      {                        } break;
    case STATE_SHORT_2:      { pBitStream->one();     } break;
    case STATE_LONG_1:       {                        } break;
    case STATE_LONG_2:       { pBitStream->zero();    } break;
    default:                 { } break;
    }
  }

  // ---------------------------------------------------
  /// For debugging: return number of calls for a state (< STATE_MAX_COUNT)
  // ---------------------------------------------------
  template<class TBitExtractorConstants, class PacketGen>
  uint32_t BitExtractor<TBitExtractorConstants, PacketGen>::getNrCalls(unsigned int unState) const
  {
    uint32_t unRet;

    if (unState < static_cast<uint32_t>(STATE_MAX_COUNT))
    {
      unRet = tickcounts.ref(unState);
    }
    else
    {
      unRet = static_cast<uint32_t>(-1);
    }

    return unRet;
  }

} // namespace dcc

#endif // DCC_BITEXTRACTOR_H