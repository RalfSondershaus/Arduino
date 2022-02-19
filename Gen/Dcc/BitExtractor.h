///
/// @file BitExtractor.h
///

#ifndef DCC_BITEXTRACTOR_H
#define DCC_BITEXTRACTOR_H

#include <Dcc/PacketExtractor.h>
#include <stdint.h> // for uint32_t. AVR gcc does not support <cstdint>

namespace Dcc
{
  /// a helper class to define all constant values in a single class
  template<
    /// [us] Minimum time between two edges for a "short" part (first or second part of a "1"). Spec: 52 us, with resolution of 4 us: 48 us
    int unPartTimeShortMin = 48
    /// [us] Maximum time between two edges for a "short" part (first or second part of a "1"). Spec: 64 us, with resolution of 4 us: 68 us
    , int unPartTimeShortMax = 68
    /// [us] Minimum time between two edges for a "long" part (first or second part of a "0"). Spec: 90 us, with resolution of 4 us: 86 us
    , int unPartTimeLongMin = 86
    /// [us] Maximum time between two edges for a "long" part (first or second part of a "0"). Typical value: 10000
    , int unPartTimeLongMax = 10000
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

  // ---------------------------------------------------
  // Generate a bit out of short ticks and long ticks (so called half bits).
  // The bit is forwarded to an instance of class PacketGenerator.
  // ---------------------------------------------------
  template<class TBitExtractorConstants, class PacketGen>
  class BitExtractor
  {
  public:
    /// States of this state machine
    typedef enum
    {
      STATE_INVALID      = 0,      ///< No bit detected, phase not detected; this is the default state after startup
      STATE_SHORT_INIT_1 = 1,      ///< One short tick detected, no long tick detected since last INVALID
      STATE_SHORT_INIT_2 = 2,      ///< Two subsequent short ticks detected (bit value 1), no long tick detected since last INVALID
      STATE_LONG_INIT_1  = 3,      ///< One long tick detected, no short tick detected since last INVALID
      STATE_LONG_INIT_2  = 4,      ///< Two subsequent long ticks detected (bit value 0), no short tick detected since last INVALID
      STATE_SHORT_1      = 5,      ///< One short tick detected
      STATE_SHORT_2      = 6,      ///< Two subsequent short ticks detected (bit value 1)
      STATE_LONG_1       = 7,      ///< One long tick detected
      STATE_LONG_2       = 8,      ///< Two subsequent long ticks detected (bit value 0)
      STATE_MAX_COUNT    = 9       ///< Maximum number of states
    } eState;
  protected:
    /// This class
    typedef BitExtractor<TBitExtractorConstants, PacketGen> This;

    /// [us] Minimum time between two edges for a "short" part (first or second part of a "1"). Spec: 52 us, with resolution of 4 us: 48 us
    static constexpr uint32_t getPartTimeShortMin() { return TBitExtractorConstants::getPartTimeShortMin(); }
    /// [us] Maximum time between two edges for a "short" part (first or second part of a "1"). Spec: 64 us, with resolution of 4 us: 68 us
    static constexpr uint32_t getPartTimeShortMax() { return TBitExtractorConstants::getPartTimeShortMax(); }
    /// [us] Minimum time between two edges for a "long" part (first or second part of a "0"). Spec: 90 us, with resolution of 4 us: 86 us
    static constexpr uint32_t getPartTimeLongMin() { return TBitExtractorConstants::getPartTimeLongMin(); }
    /// [us] Maximum time between two edges for a "long" part (first or second part of a "0"). Typical value: 10000
    static constexpr uint32_t getPartTimeLongMax() { return TBitExtractorConstants::getPartTimeLongMax(); }
    
    /// Type for received tick
    typedef enum
    {
      INVALID_HALFBIT = 0,
      SHORT_HALFBIT = 1,
      LONG_HALFBIT = 2
    } eHalfBit;

    /// Current state
    eState state;

    /// Previous state
    eState prevState;

    /// The packet generator
    PacketGen& pktGen;

    /// For debugging: number of received interrupts per state (half bits)
    unsigned long aunTickCnt[STATE_MAX_COUNT];

    /// Return half bit type (short, long, invalid)
    static eHalfBit checkTick(uint32_t ulTime);

    // ---------------------------------------------------
    /// S-9.1: In a "1" bit, the first and last part of a bit shall have the same duration, and that duration shall nominally be 58 microseconds2, giving
    /// 15 the bit a total duration of 116 microseconds. Digital Command Station components shall transmit "1" bits with the first and last parts
    /// each having a duration of between 55 and 61 microseconds. A Digital Decoder must accept bits whose first and last parts have a
    /// duration of between 52 and 64 microseconds, as a valid bit with the value of "1".
    /// @param ulTime [us] Time between two edges
    // ---------------------------------------------------
    static constexpr bool isShortHalfBit(uint32_t ulTime)
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
    static constexpr bool isLongHalfBit(unsigned long ulTime)
    {
      return (ulTime >= getPartTimeLongMin()) && (ulTime <= getPartTimeLongMax());
    }

  public:
    /// constructor
    BitExtractor(PacketGen& pktgen) : state(STATE_INVALID), pktGen(pktgen)
    {
      int i;

      for (i = 0; i < STATE_MAX_COUNT; i++)
      {
        aunTickCnt[i] = 0u;
      }
    }

    /// destructor
    virtual ~BitExtractor() {}
    /// Execute state machine with an event
    void execute(uint32_t ulTimeDiff);
    /// For debugging: return number of calls for a state (< STATE_MAX_COUNT)
    unsigned int getNrCalls(unsigned int unState) const;
  };

  // ---------------------------------------------------
  /// Return tick type (short, long, invalid)
  // ---------------------------------------------------
  template<class TBitExtractorConstants, class PacketGen>
  typename BitExtractor<TBitExtractorConstants, PacketGen>::eHalfBit BitExtractor<TBitExtractorConstants, PacketGen>::checkTick(uint32_t ulTime)
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
    state = aTransitionMap[state][halfBitRcv];

    aunTickCnt[state]++;

    switch (state)
    {
    case STATE_INVALID: { pktGen.invalid(); } break;
    case STATE_SHORT_INIT_1: {                    } break;
    case STATE_SHORT_INIT_2: { pktGen.one();     } break;
    case STATE_LONG_INIT_1: {                    } break;
    case STATE_LONG_INIT_2: { pktGen.zero();    } break;
    case STATE_SHORT_1: {                    } break;
    case STATE_SHORT_2: { pktGen.one();     } break;
    case STATE_LONG_1: {                    } break;
    case STATE_LONG_2: { pktGen.zero();    } break;
    default: { } break;
    }
  }

  // ---------------------------------------------------
  /// For debugging: return number of calls for a state (< STATE_MAX_COUNT)
  // ---------------------------------------------------
  template<class TBitExtractorConstants, class PacketGen>
  unsigned int BitExtractor<TBitExtractorConstants, PacketGen>::getNrCalls(unsigned int unState) const
  {
    unsigned int unRet;

    if (unState < (unsigned int)STATE_MAX_COUNT)
    {
      unRet = aunTickCnt[unState];
    }
    else
    {
      unRet = (unsigned int)-1;
    }

    return unRet;
  }

} // namespace Dcc

#endif // DCC_BITEXTRACTOR_H