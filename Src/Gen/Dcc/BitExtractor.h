/**
 * @file bit_extractor.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Provides classes for extracting DCC bits from timing intervals in namespace dcc.
 *
 * This file defines:
 * - BitExtractorConstants: Compile-time configuration for DCC timing thresholds.
 * - BitStream: A fixed-size bit stream with parallel invalid bit tracking for digital protocols.
 * - bit_extractor: A state machine that interprets timing intervals as DCC bits and forwards events to a user-supplied bit stream or packet generator.
 *
 * @details
 * The bit_extractor class implements a state machine for decoding DCC signals from timing intervals.
 * It uses configurable timing constants (via BitExtractorConstants) and forwards detected bit events
 * to a user-supplied class (such as BitStream or a packet generator) that must provide the following interface:
 *   - void invalid(); ///< Called when an invalid bit sequence is detected (state machine reset)
 *   - void one();     ///< Called when a valid "1" bit is detected
 *   - void zero();    ///< Called when a valid "0" bit is detected
 */

#ifndef DCC_BITEXTRACTOR_H
#define DCC_BITEXTRACTOR_H

#include <Std_Types.h> // for uint32_t
#include <Dcc/DecoderCfg.h>
#include <Dcc/PacketExtractor.h>
#include <Util/Array.h> // for debugging
#include <Util/Fix_Queue.h>   // For the bit stream queue
#include <Util/Ptr.h>   // for pointer to bit stream

namespace dcc
{
  /**
   * @class BitExtractorConstants
   * @brief Helper class to define all DCC timing constants in a single place.
   *
   * This class provides compile-time constants for the minimum and maximum durations (in microseconds)
   * of "short" and "long" parts of DCC bits, as required by the DCC protocol specification.
   *
   * @tparam PartTimeShortMin Minimum time between two edges for a "short" part (first or second part of a "1"). Spec: 52 us, with resolution of 4 us: 48 us
   * @tparam PartTimeShortMax Maximum time between two edges for a "short" part (first or second part of a "1"). Spec: 64 us, with resolution of 4 us: 68 us
   * @tparam PartTimeLongMin  Minimum time between two edges for a "long" part (first or second part of a "0"). Spec: 90 us, with resolution of 4 us: 86 us
   * @tparam PartTimeLongMax  Maximum time between two edges for a "long" part (first or second part of a "0"). Typical value: 10000
   */
  template<int PartTimeShortMin = 48, int PartTimeShortMax = 68, int PartTimeLongMin = 86, int PartTimeLongMax = 10000>
  class bit_extractor_constants
  {
  public:
    /// [us] Minimum time between two edges for a "short" part (first or second part of a "1"). Spec: 52 us, with resolution of 4 us: 48 us
    static constexpr uint32_t kPartTimeShortMin = static_cast<uint32_t>(PartTimeShortMin);
    /// [us] Maximum time between two edges for a "short" part (first or second part of a "1"). Spec: 64 us, with resolution of 4 us: 68 us
    static constexpr uint32_t kPartTimeShortMax = static_cast<uint32_t>(PartTimeShortMax);
    /// [us] Minimum time between two edges for a "long" part (first or second part of a "0"). Spec: 90 us, with resolution of 4 us: 86 us
    static constexpr uint32_t kPartTimeLongMin = static_cast<uint32_t>(PartTimeLongMin);
    /// [us] Maximum time between two edges for a "long" part (first or second part of a "0"). Typical value: 10000
    static constexpr uint32_t kPartTimeLongMax = static_cast<uint32_t>(PartTimeLongMax);
  };

  /**
   * @class bit_extractor
   * @brief State machine for generating DCC bits from timing intervals ("half bits").
   *
   * This class implements a state machine that interprets timing intervals (ticks) as DCC bits ("1" or "0")
   * based on configurable timing constants. The resulting bits are forwarded as events to an instance of the
   * BitStream or a user-supplied packet generator class (template parameter).
   *
   * @tparam TBitExtractorConstants Class providing static timing constants (see BitExtractorConstants).
   * @tparam TBitStream             Class that receives bit events (must provide: invalid(), one(), zero()).
   *
   * @details
   * The bit_extractor receives timing intervals (in microseconds) and classifies them as "short", "long", or "invalid"
   * according to the DCC specification. It then transitions its internal state machine and triggers the appropriate
   * event on the TBitStream instance.
   *
   * @section BitExtractor_Interface Interface requirements for TBitStream
   * The TBitStream (or PacketGen) template parameter must provide the following public member functions:
   * - void invalid();    ///< Called when an invalid bit sequence is detected (state machine reset)
   * - void one();        ///< Called when a valid "1" bit is detected
   * - void zero();       ///< Called when a valid "0" bit is detected
   */
  template<class TBitExtractorConstants = bit_extractor_constants<>, class TPacketExtractor = packet_extractor<> >
  class bit_extractor
  {
  protected:
    using This = bit_extractor<TBitExtractorConstants, TPacketExtractor>;
    using packet_extractor_type = TPacketExtractor;
    using pointer_type = typename util::ptr<packet_extractor_type>;
    using reference_type = packet_extractor_type&;
    using const_reference_type = const packet_extractor_type&;

    /**
     * @brief Minimum time between two edges for a "short" part (first or second part of a "1"). Spec: 52 us, with resolution of 4 us: 48 us 
     */
    static constexpr uint32_t kPartTimeShortMin = TBitExtractorConstants::kPartTimeShortMin;
    /**
     * @brief Maximum time between two edges for a "short" part (first or second part of a "1"). Spec: 64 us, with resolution of 4 us: 68 us
     */
    static constexpr uint32_t kPartTimeShortMax = TBitExtractorConstants::kPartTimeShortMax;
    /**
     * @brief Minimum time between two edges for a "long" part (first or second part of a "0"). Spec: 90 us, with resolution of 4 us: 86 us
     */
    static constexpr uint32_t kPartTimeLongMin = TBitExtractorConstants::kPartTimeLongMin;
    /**
     * @brief Maximum time between two edges for a "long" part (first or second part of a "0"). Typical value: 10000 us.
     */
    static constexpr uint32_t kPartTimeLongMax = TBitExtractorConstants::kPartTimeLongMax;
    
    /**
     * @brief States of the bit extractor state machine.
     */
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

    /**
     * @brief Types of half bits, e.g., type for received ticks.
     */
    typedef enum
    {
      INVALID_HALFBIT = 0,
      SHORT_HALFBIT = 1,
      LONG_HALFBIT = 2
    } eHalfBit;

    /**
     * @brief For debugging: number of received interrupts per state (half bits) 
     */ 
    using tick_array_type = util::array<uint32_t, STATE_MAX_COUNT>;

    /**
     * @brief Current state.
     * 
     */
    eState state;

    /**
     * @brief Previous state.
     */
    eState prevState;

    /** 
     * @brief Reference to the instance that receives bit events (invalid, one, zero) and generates 
     * packets.
     */
    reference_type packet_extractor;

    #if CFG_DCC_DECODER_DEBUG == OPT_DCC_DECODER_DEBUG_ON
    /// For debugging: number of received interrupts per state (half bits)
    tick_array_type call_counts;
    #endif

    /**
     * @brief Classify the received time interval as SHORT_HALFBIT, LONG_HALFBIT, or INVALID_HALFBIT.
     * 
     * @param ulTime Time interval in microseconds since the last tick/interrupt.
     * @return eHalfBit Classified half bit type.
     */
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
      return (ulTime >= kPartTimeShortMin) && (ulTime <= kPartTimeShortMax);
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
      return (ulTime >= kPartTimeLongMin) && (ulTime <= kPartTimeLongMax);
    }

  public:
    /**
     * @brief Constructs a bit_extractor with a reference to the packet generator.
     * @param pex Reference to the packet generator that receives bit events.
     */
    bit_extractor(reference_type pex) : state(STATE_INVALID), packet_extractor(pex)
    {
    #if CFG_DCC_DECODER_DEBUG == OPT_DCC_DECODER_DEBUG_ON
      call_counts.fill(0);
    #endif
    }

    /** @brief Destructor (defaulted). */
    ~bit_extractor() = default;

    /**
     * @brief Execute the state machine with a time difference (to last tick/interrupt, i.e., to last half bit).
     * @param ulTimeDiff Time difference in microseconds since the last tick/interrupt.
     *
     * This function processes the timing interval, updates the state machine, and forwards the detected bit event
     * to the bit stream or packet generator.
     */
    void execute(uint32_t ulTimeDiff);

    /**
     * @brief For debugging: return the number of calls for a given state (< STATE_MAX_COUNT).
     * @param unState State index (must be less than STATE_MAX_COUNT).
     * @return Number of times the state has been entered, or -1 if out of range.
     */
    uint32_t get_call_count(unsigned int unState) const;
  };

  /**
   * @brief Executes the bit_extractor state machine with a timing event.
   * 
   * @param ulTimeDiff Time difference in microseconds since the last tick/interrupt (i.e., since the last half bit).
   *
   * This function processes the given timing interval, updates the internal state machine according to the DCC protocol,
   * and triggers the appropriate event (invalid, one, or zero) on the connected bit stream or packet generator.
   * The state transition logic ensures correct decoding of DCC bits from the incoming timing intervals.
   */
  template<class TBitExtractorConstants, class PacketGen>
  void bit_extractor<TBitExtractorConstants, PacketGen>::execute(uint32_t ulTimeDiff)
  {
    static const uint8 aTransitionMap[STATE_MAX_COUNT][3] =
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
    state = static_cast<eState>(aTransitionMap[state][static_cast<uint32_t>(halfBitRcv)]);

    #if CFG_DCC_DECODER_DEBUG == OPT_DCC_DECODER_DEBUG_ON
    // for debugging
    call_counts.at(static_cast<typename tick_array_type::size_type>(state))++;
    #endif

    switch (state)
    {
    case STATE_INVALID:      { packet_extractor.invalid(); } break;
    case STATE_SHORT_INIT_1: {                             } break;
    case STATE_SHORT_INIT_2: { packet_extractor.one();     } break;
    case STATE_LONG_INIT_1:  {                             } break;
    case STATE_LONG_INIT_2:  { packet_extractor.zero();    } break;
    case STATE_SHORT_1:      {                             } break;
    case STATE_SHORT_2:      { packet_extractor.one();     } break;
    case STATE_LONG_1:       {                             } break;
    case STATE_LONG_2:       { packet_extractor.zero();    } break;
    default:                 { } break;
    }
  }

  // ---------------------------------------------------
  /// For debugging: return number of calls for a state (< STATE_MAX_COUNT)
  // ---------------------------------------------------
  template<class TBitExtractorConstants, class PacketGen>
  uint32_t bit_extractor<TBitExtractorConstants, PacketGen>::get_call_count(unsigned int unState) const
  {
    uint32_t unRet;

    #if CFG_DCC_DECODER_DEBUG == OPT_DCC_DECODER_DEBUG_ON
    if (unState < static_cast<uint32_t>(STATE_MAX_COUNT))
    {
      unRet = call_counts.at(unState);
    }
    else
    {
      unRet = static_cast<uint32_t>(-1);
    }
    #else
    unRet = static_cast<uint32_t>(0);
    #endif

    return unRet;
  }

} // namespace dcc

#endif // DCC_BITEXTRACTOR_H