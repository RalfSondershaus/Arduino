// ---------------------------------------------------
/// State machine for period (which are transformed into bits)
// ---------------------------------------------------

#include "PeriodSM.h"

unsigned long aulDbgTimeDiff[100];
unsigned char aucDbgState[100];
unsigned int unDbgIdx = 0;

namespace Dcc
{
  /// [us] Minimum time between two edges for a "short" part (first or second part of a "1"). Spec: 52 us, with resolution of 4 us: 48 us
  #define PART_TIME_SHORT_MIN      48u
  /// [us] Maximum time between two edges for a "short" part (first or second part of a "1"). Spec: 64 us, with resolution of 4 us: 68 us
  #define PART_TIME_SHORT_MAX      68u
  /// [us] Minimum time between two edges for a "long" part (first or second part of a "0"). Spec: 90 us, with resolution of 4 us: 86 us
  #define PART_TIME_LONG_MIN       86u
  /// [us] Maximum time between two edges for a "long" part (first or second part of a "0")
  #define PART_TIME_LONG_MAX    10000u

  // ---------------------------------------------------
  /// S-9.1: In a "1" bit, the first and last part of a bit shall have the same duration, and that duration shall nominally be 58 microseconds2, giving
  /// 15 the bit a total duration of 116 microseconds. Digital Command Station components shall transmit "1" bits with the first and last parts
  /// each having a duration of between 55 and 61 microseconds. A Digital Decoder must accept bits whose first and last parts have a
  /// duration of between 52 and 64 microseconds, as a valid bit with the value of "1".
  /// @param ulTime [us] Time between two edges
  // ---------------------------------------------------
  static inline bool isShortHalfBit(unsigned long ulTime)
  {
    return (ulTime >= PART_TIME_SHORT_MIN) && (ulTime <= PART_TIME_SHORT_MAX);
  }

  // ---------------------------------------------------
  /// In a "0" bit, the duration of the first and last parts of each transition shall nominally be greater than or equal to 100 microseconds. To
  /// 20 keep the DC component of the total signal at zero as with the "1" bits, the first and last part of the "0" bit are normally equal to one
  /// another. Digital Command Station components shall transmit "0" bits with each part of the bit having a duration of between 95 and
  /// 9900 microseconds with the total bit duration of the "0" bit not exceeding 12000 microseconds. A Digital Decoder must accept bits,
  /// whose first or last parts have a duration between 90 and 10,000 microseconds, as a valid bit with the value of "0". Figure 1 provides
  /// an example of bits encoded using this technique.
  // ---------------------------------------------------
  static inline bool isLongHalfBit(unsigned long ulTime)
  {
    return (ulTime >= PART_TIME_LONG_MIN) && (ulTime <= PART_TIME_LONG_MAX);
  }

  // ---------------------------------------------------
  /// constructor
  // ---------------------------------------------------
  HalfBitStateMachine::HalfBitStateMachine(DccInterpreter& DccI)
    : state(STATE_INVALID)
    , DccIntp(DccI)
  {
    int i;

    for (i = 0; i < STATE_MAX_COUNT; i++)
    {
      aunTickCnt[i] = 0u;
    }
  }

  // ---------------------------------------------------
  /// destructor
  // ---------------------------------------------------
  HalfBitStateMachine::~HalfBitStateMachine()
  {}

  // ---------------------------------------------------
  /// Return tick type (short, long, invalid)
  // ---------------------------------------------------
  HalfBitStateMachine::eHalfBit HalfBitStateMachine::checkTick(unsigned long ulTime)
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
  void HalfBitStateMachine::execute(unsigned long ulTimeDiff)
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

    if (unDbgIdx < 100u)
    {
      aulDbgTimeDiff[unDbgIdx] = ulTimeDiff;
      aucDbgState[unDbgIdx] = state;
      unDbgIdx++;
    }

    aunTickCnt[state]++;

    switch (state)
    {
    case STATE_INVALID     : { DccIntp.invalid(); } break;
    case STATE_SHORT_INIT_1: {                    } break;
    case STATE_SHORT_INIT_2: { DccIntp.one();     } break;
    case STATE_LONG_INIT_1 : {                    } break;
    case STATE_LONG_INIT_2 : { DccIntp.zero();    } break;
    case STATE_SHORT_1     : {                    } break;
    case STATE_SHORT_2     : { DccIntp.one();     } break;
    case STATE_LONG_1      : {                    } break;
    case STATE_LONG_2      : { DccIntp.zero();    } break;
    default: { } break;
    }
  }

  // ---------------------------------------------------
  /// For debugging: return number of calls for a state (< STATE_MAX_COUNT)
  // ---------------------------------------------------
  unsigned int HalfBitStateMachine::getNrCalls(unsigned int unState) const
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

// EOF