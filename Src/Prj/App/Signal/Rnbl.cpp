/**
 * Control signals on a model railroad based on input AD values
 */

#include <Arduino.h>
#include <Gen/Runable/Runable.h>
#include <Gen/Scheduler/Scheduler.h>
#include <Gen/Timer/Timer.h>
#include <InputClassifier.h>
#include <SignalCmdClassifier.h>
#include <RailwaySignal.h>

#define GREEN_LOW    (605-10)
#define GREEN_HIGH   (605+10)
#define YELLOW_LOW   (399-10)
#define YELLOW_HIGH  (399+10)
#define RED_LOW      (192-10)
#define RED_HIGH     (192+10)
#define WHITE_LOW    (147-10)
#define WHITE_HIGH   (147+10)

/// [ms] default signal dim times
#define SIGNAL_DIM_UP 100
#define SIGNAL_DIM_DN 100

// --------------------------------------------------------------
// RTE
//$ TODO move to RTE
// --------------------------------------------------------------
/// RTE interface for classified integral valus
IfcRteSRClassifiedValueArray rteClassifierArray;
/// RTE interface for signal commands
IfcRteSignalCmd rteSignalCmd[NR_SIGNALS];

// --------------------------------------------------------------
// 
// --------------------------------------------------------------

// --------------------------------------------------------------
/// Runable class for input AD value classification: a classifier per input AD value
// --------------------------------------------------------------
template<int NrClassifiers, int NrClasses>
class RAnalogueInputClassifier : public Runable
{
public:
  typedef InputClassifier<NrClasses> MyInputClassifier;
  typedef MyInputClassifier MyInputClassifierArray[NrClassifiers];
  typedef MyInputClassifier::tConfig tConfigArray[NrClassifiers];

protected:
  MyInputClassifierArray        aClassifiers; ///< Array of input classifiers
  tClassifiedValueArray         aClassifiedValues; ///< Array of classified values; is written to RTE output port
  IfcRteSRClassifiedValueArray& rteOutput; ///< Reference to RTE output port for classification result

public:
  /// Constructor
  RAnalogueInputClassifier(IfcRteSRClassifiedValueArray& port)
    : rteOutput(port)
  {}
  /// Constructor
  RAnalogueInputClassifier(IfcRteSRClassifiedValueArray& port, const tConfigArray& config)
    : rteOutput(port)
  {
    setConfig(config);
  }
  /// Set / update configurations
  void setConfig(const tConfigArray& config)
  {
    int i;
    for (i = 0; i < NrClassifiers; i++)
    {
      aClassifiers->setConfig(config[i]);
    }
  }
  /// Initialization at system start
  virtual void init(void)
  {}
  /// Main execution function
  virtual void run(void)
  {
    int i;
    for (i = 0; i < NrClassifiers; i++)
    {
      (void) aClassifier[i].measure();
      aClassifiedValues[i] = (uint8_t) aClassifier[i].getClassIndex();
    }
    rteOutput.write(aClassifiedValues);
  }
};

// --------------------------------------------------------------
/// Runable class for signals. Read IfcRteSRClassifiedValueArray and calculate target values for IfcRteSRSignalTgtArray.
// --------------------------------------------------------------
class RRailwaySignals : public Runable
{
public:

protected:
  const IfcRteSRClassifiedValueArray& rteClassifiedValuesArray;
  IfcRteSRSignalTgtArray& rteSignalTgtArray;
  const tRailwaySignalConfig& mConfig;

public:
  /// Constructor
  RRailwaySignals(const IfcRteSRClassifiedValueArray& portClassifiedValues, IfcRteSRSignalTgtArray portSignalTgtArray, const tRailwaySignalConfig& config)
    : rteClassifiedValuesArray(portClassifiedValues)
    , rteSignalTgtArray(portSignalTgtArray)
    , mConfig(config)
  {}
  /// Map configuration values (for signal pictures) to output ports depending on classified values.
  /// The i-th classified value (n = c[i]) maps signal picture n to output port i (which is specified by the classifier map).
  void mapClassifiedValues()
  {
    const tClassifiedValueArray& cva = rteClassifiedValuesArray.ref();
    const tClassifierMapArray& cma = mConfig.aClassifierMap;
    tSignalTgtArray& sta = rteSignalTgtArray.ref();
    auto cmait = cma.begin();
    auto cvait = cva.begin();
    while ((cvait != cva.end()) && (cmait != cma.end()))
    {
      if ((*cmait < sta.size()) && (*cvait < mConfig.aSignalPictures.size()))
      {
        sta[*cmait].aIntArray = mConfig.aSignalPictures[*cvait].aIntArray;
        sta[*cmait].unSpeedDn = mConfig.unSpeedDn;
        sta[*cmait].unSpeedUp = mConfig.unSpeedUp;
      }
      cvait++;
      cmait++;
    }
  }
  /// Initialization at system start
  virtual void init(void) 
  {}
  /// Main execution function
  virtual void run(void)
  {
    mapClassifiedValues();
  }
};

// --------------------------------------------------------------
/// Runable class for LED routing. Now: just signals are supported. Map signal target values
/// IfcRteSRSignalTgtArray to outputs.
// --------------------------------------------------------------
template<int NrSignals, int NrSignalLeds, int >
class RLedRouter : public Runable
{
protected:
  const IfcRteSRSignalTgtArray& rteSignalTgtArray;
  IfcRteSROnboardTarget rteOnboardTarget;
  IfcRteSRExternalTarget rteExternalTarget;
  tOnboardTargetArray mOnboardTargetArray;
  tExternalTargetArray mExternalTargetArray;

  /// The mapping configuration
  const IfcRteSRConfigLedOutputSignalArray& mConfig;

public:
  /// Constructor
  RLedRouter(const IfcRteSRSignalTgtArray& portSignalTgtArray, const IfcRteSRConfigLedOutputSignalArray& config)
    : rteSignalTgtArray(portSignalTgtArray)
    , mConfig(config)
  {}
  //RnblRailwaySignalsDO()
  //  : S2(rRailwaySignals.S2.mIfcPortSignalTarget  , 13, 12, 11, 10,  9)
  //  , S4(rRailwaySignals.S4.mIfcPortSignalTarget  ,  8,  7,  6,  5,  4)
  //  , S6(rRailwaySignals.S6.mIfcPortSignalTarget  , 40, 42, 44, 46, 48)
  //  , S8(rRailwaySignals.S8.mIfcPortSignalTarget  ,  3, 24,  2, 22, -1)
  //  , S10(rRailwaySignals.S10.mIfcPortSignalTarget, 30, 32, 28, 26, -1)
  //  , S12(rRailwaySignals.S12.mIfcPortSignalTarget, 52, -1, 50, -1, -1)
  //{}
  void mapSignalValue(tLedConfig& sigledconfig, uint8_t ucTgtVal)
  {
    Percent16 tgt = Percent16::ConvPercent100(ucTgtVal);

    switch (sigledconfig.u8Type)
    {
      case tLedConfig::DO_PWM_PORT: 
      { 
        if (sigledconfig.u8Output < mOnboardTargetArray.size())
        {
          mOnboardTargetArray[sigledconfig.u8Output] = tgt;
        }
      } 
      break;
      case tLedConfig::SHIFT_REG_PORT: 
      { 
        if (sigledconfig.u8Output < mExternalTargetArray.size())
        {
          mExternalTargetArray[sigledconfig.u8Output] = tgt;
        }
      } 
      break;
      default: 
      {} 
      break;
    }
  }
  void mapSignal(const tSignalConfig& sigconfig, const tSignalTgt& sigtgt)
  {
    auto sigled = sigtgt.aIntArray.begin();
    auto sigledconfig = sigconfig.begin();
    while (sigled != sigtgt.aIntArray.end())
    {
      mapSignalValue(*sigledconfig, *sigled);
      sigled++;
      sigledconfig++;
    }
  }

  void mapSignals()
  {
    if (isConfigValid())
    {
      auto sigconfig = mConfig.ref().begin();
      auto sig = rteSignalTgtArray.ref().begin();
      while (sig != rteSignalTgtArray.ref().end())
      {
        mapSignal(*sigconfig, *sig);
        sig++;
        sigconfig++;
      }
    }
  }
  /// Initialization at system start
  virtual void init(void)
  {}
  /// Main execution function
  virtual void run(void)
  {
    mapSignals(); 
  }
};

//static const int nPorts[] = { 13,12,11,10,9,8,7,6,5,4,3,2, 22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52 };
//static const int nPortsNr = sizeof(nPorts) / sizeof(int);

static const int aLower[] = { GREEN_LOW, WHITE_LOW, RED_LOW, YELLOW_LOW };
static const int aUpper[] = { GREEN_HIGH, WHITE_HIGH, RED_HIGH, YELLOW_HIGH };

/// Array of classifiers for input analogue values (for each signal an input value)
RAnalogueInputClassifier<NR_SIGNALS, NR_CLASSES>::tConfigArray aClassifierConfig =
{
  { aLower, aUpper, A0 },
  { aLower, aUpper, A1 },
  { aLower, aUpper, A2 },
  { aLower, aUpper, A3 },
  { aLower, aUpper, A4 },
};

/// The runable instance for input AD value measurements
RAnalogueInputClassifier<NR_SIGNALS, NR_CLASSES> rInputButtonClassifier(rteClassifierArray, aClassifierConfig);
/// The runable instance for signals
RRailwaySignals<NR_SIGNALS> rRailwaySignals;

/// The runable instance
RnblRailwaySignalsDO rRailwaySignalsDO;

/// The (one and only) scheduler
Scheduler SchM;

// the setup function runs once when you press reset or power the board
void setup()
{
  SchM.add(0u,  5u, &rInputButtonClassifier);
  SchM.add(0u, 10u, &rRailwaySignals);
  SchM.add(0u, 10u, &rRailwaySignalsDO);
  SchM.add(3u, 50u, &rDiagnoseCommander);
  SchM.init();
  rRailwaySignals.S2.setIfcDiagnose(&rDiagnoseCommander.diagCmd.mIfcPortDiagnose);
 }
// the loop function runs over and over again forever
void loop()
{
  SchM.schedule();

  //printAnalog();
  //activateOutputsOneAfterOther();
  //activateOutputs0_4();
}