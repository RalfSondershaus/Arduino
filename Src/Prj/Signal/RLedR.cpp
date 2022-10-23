/**
 * @file RLedR.cpp
 *
 * @author Ralf Sondershaus
 *
 * Runable class for LED routing.
 * Route input values to output ports
 */

#include <Arduino.h>
#include <Gen/Runable/Runable.h>
#include <Gen/Scheduler/Scheduler.h>
#include <Gen/Timer/Timer.h>
#include <InputClassifier.h>
#include <SignalCmdClassifier.h>
#include <RailwaySignal.h>

/// [ms] default signal dim times
#define SIGNAL_DIM_UP 100
#define SIGNAL_DIM_DN 100

class LedMapper
{
public:
  typedef Array
  typedef Array<tSpeed16, tOnboardTargetArray::Size()> tOnboardSpeedArray;
  typedef Array<tSpeed16, tExternalTargetArray::Size()> tExternalSpeedArray;
  /// Intensities, speeds up, speeds down for onboard pins
  typedef struct
  {
    tOnboardTargetArray IntArray;
    tOnboardSpeedArray SpeedUpArray;
    tOnboardSpeedArray SpeedDnArray;
  } tOnboardTgt;

  /// Intensities, speeds up, speeds down for shift registers
  typedef struct
  {
    tExternalTargetArray IntArray;
    tExternalSpeedArray SpeedUpArray;
    tExternalSpeedArray SpeedDnArray;
  } tExternalTgt;

  /// Targets for onboard pins
  tOnboardTgt& mOnboardTgt;
  /// Targets for shift registers
  tExternalTgt& mExternalTgt;

public:
  /// Construct
  LedMapper(tOnboardTgt& onboardTgt, tExternalTgt& externalTgt) 
    : mOnboardTgt(onboardTgt)
    , mExternalTgt(externalTgt)
  {}
  /// Destruct
  ~LedMapper() {}
  /// Map
  void mapLedValue(const tCalLedR& sigledcal, uint8_t ucTgtVal, tSpeed16 unSpdUp, tSpeed16 unSpdDn)
  {
    Percent16 tgt = Percent16::ConvPercent100(ucTgtVal);

    switch (sigledcal.u8Type)
    {
    case tCalLedR::DO_PWM_PORT:
    {
      if (sigledcal.u8Output < mOnboardTgt.IntArray.size())
      {
        mOnboardTgt.IntArray[sigledcal.u8Output] = tgt;
        mOnboardTgt.SpeedDnArray[sigledcal.u8Output] = unSpdDn;
        mOnboardTgt.SpeedUpArray[sigledcal.u8Output] = unSpdUp;
      }
    }
    break;
    case tCalLedR::SHIFT_REG_PORT:
    {
      if (sigledcal.u8Output < mExternalTgt.IntArray.size())
      {
        mExternalTgt.IntArray[sigledcal.u8Output] = tgt;
        mExternalTgt.SpeedDnArray[sigledcal.u8Output] = unSpdDn;
        mExternalTgt.SpeedUpArray[sigledcal.u8Output] = unSpdUp;
      }
    }
    break;
    default:
    {}
    break;
    }
  }
};

// --------------------------------------------------------------
/// Map signal values (rteSignalTgtArray) to onboard targets and external targets (mOnboardTgt, mExternalTgt)
// --------------------------------------------------------------
class SignalMapper : public LedMapper
{
public:
  /// Interface for signal target values (intensity, speed up, speed down)
  const IfcRteSRSignalTgtArray& rteSignalTgtArray;
  /// The mapping configuration
  const IfcRteSRCalLedRSignalArray& mCal;

public:
  /// Construct
  SignalMapper(const IfcRteSRSignalTgtArray& portSignalTgtArray, const IfcRteSRCalLedRSignalArray& cal, tOnboardTgt& onboardTgt, tExternalTgt& externalTgt)
    : rteSignalTgtArray(portSignalTgtArray)
    , mCal(cal)
    , LedMapper(onboardTgt, externalTgt)
  {}
  /// Destruct
  ~SignalMapper() {}
  /// Map LEDs of a single signal
  void mapSignal(const tCalLedRSignal& sigcal, const tSignalTgt& sigtgt)
  {
    auto sigled = sigtgt.aIntArray.begin();
    auto sigledcal = sigcal.begin();
    while (sigled != sigtgt.aIntArray.end())
    {
      mapLedValue(*sigledcal, *sigled, sigtgt.unSpeedUp, sigtgt.unSpeedDn);
      sigled++;
      sigledcal++;
    }
  }
  /// Map all signals
  void mapSignals()
  {
    if (mCal.isValid())
    {
      auto sigcal = mCal.ref().begin();
      auto sig = rteSignalTgtArray.ref().begin();
      while (sig != rteSignalTgtArray.ref().end())
      {
        mapSignal(*sigcal, *sig);
        sig++;
        sigcal++;
      }
    }
  }
};

template<int NrLeds>
class LedRamper
{
public:
  typedef Array<tSpeed16, NrLeds> tSpeedArray;
  typedef Array<Percent16, NrLeds> tIntArray;
  tSpeedArray& mSpeedUpArray;
  tSpeedArray& mSpeedDnArray;
  tIntArray& mIntArray;
  tInArray& 
};
// --------------------------------------------------------------
/// Runable class for LED routing. Now: just signals are supported. Map signal target values
/// IfcRteSRSignalTgtArray to outputs.
/// Input: IfcRteSRSignalTgtArray: 
/// Output: IfcRteSRConfigLedOutputSignalArray
// --------------------------------------------------------------
class RLedR : public Runable
{
protected:

  /// Interface for onboard target intensities
  IfcRteSROnboardTarget rteOnboardTarget;
  IfcRteSRExternalTarget rteExternalTarget;
  
  /// Targets for onboard pins
  LedMapper::tOnboardTgt mOnboardTgt;
  /// Targets for shift registers
  LedMapper::tExternalTgt mExternalTgt;
  /// Map signal values to onboard targets and external targets
  SignalMapper mSignalMapper;


public:
  /// Constructor
  RLedR(const IfcRteSRSignalTgtArray& portSignalTgtArray, const IfcRteSRCalLedRSignalArray& cal)
    : mSignalMapper(portSignalTgtArray, cal, mOnboardTgt, mExternalTgt)
  {}

  ///
  void calcRamps()
  {
    for (auto it = mOnboardTgt.IntArray.begin(); it != mOnboardTgt.IntArray.end(); it++)
    {

    }
  }
  /// call to notify that calibration values are changed
  void calChanged(void)
  {}
  /// Initialization at system start
  virtual void init(void)
  {}
  /// Main execution function
  virtual void run(void)
  {
    mSignalMapper.mapSignals();
    calcRamps();
  }
};
