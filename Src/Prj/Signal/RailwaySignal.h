/**
 * Class RailwaySignal.
 */

#ifndef RAILWAYSIGNAL_H__
#define RAILWAYSIGNAL_H__

#include <IfcRtePrj.h>

/// [ms] default dim up time
#define SIGNAL_DIM_TIME_INC  100u
/// [ms] default dim down time
#define SIGNAL_DIM_TIME_DEC  100u

// -----------------------------------------------------------------------------------
/// Set target intensities based on input commands for signals
// -----------------------------------------------------------------------------------
class RailwaySignal
{
public:
  constexpr static int nCycleTime = 10;
  typedef Speed16<nCycleTime> MySpeed16;

protected:
  uint8_t ucOutRed1;    ///< [%] Intensity
  uint8_t ucOutRed2;    ///< [%] Intensity
  uint8_t ucOutGreen;   ///< [%] Intensity
  uint8_t ucOutYellow;  ///< [%] Intensity
  uint8_t ucOutWhite;   ///< [%] Intensity
  MySpeed16 unInc;       ///< [1/32768 % / 10 ms] dim up speed: 1 = 1/32768 % / 10 ms (0% - 100%: 327.68 sec = 5.46 min), 32768 = 0% - 100%: 0 ms
  MySpeed16 unDec;       ///< [1/32768 % / 10 ms] dim down speed

public:

  RailwaySignal()
    : ucOutRed1(0)
    , ucOutRed2(0)
    , ucOutGreen(0)
    , ucOutYellow(0)
    , ucOutWhite(0)
    , unInc(MySpeed16::ConvTime(SIGNAL_DIM_INC))
    , unDec(MySpeed16::ConvTime(SIGNAL_DIM_DEC))
  {}
  ~RailwaySignal()
  {}
  /// Set output values for hp0
  void hp0()
  {
    ucOutRed1   = 100u;
    ucOutRed2   = 100u;
    ucOutGreen  = 0u;
    ucOutYellow = 0u;
    ucOutWhite  = 0u;
  }
  void hp1()
  {
    ucOutRed1 = 0u;
    ucOutRed2 = 0u;
    ucOutGreen = 100u;
    ucOutYellow = 0u;
    ucOutWhite = 0u;
  }
  void hp2()
  {
    ucOutRed1 = 0u;
    ucOutRed2 = 0u;
    ucOutGreen = 100u;
    ucOutYellow = 100u;
    ucOutWhite = 0u;
  }
  void hp3()
  {
    ucOutRed1 = 100u;
    ucOutRed2 = 100u;
    ucOutGreen = 0u;
    ucOutYellow = 0u;
    ucOutWhite = 100u;
  }
  void activateAll()
  {
    ucOutRed1 = 100u;
    ucOutRed2 = 100u;
    ucOutGreen = 100u;
    ucOutYellow = 100u;
    ucOutWhite = 100u;
  }
};

#endif // RAILWAYSIGNAL_H__
