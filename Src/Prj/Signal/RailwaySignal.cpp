/**
 * Control signals on a model railroad based on input AD values
 */

#include <Arduino.h>
#include <Runable/Runable.h>
#include <Scheduler/Scheduler.h>
#include <Timer/Timer.h>
#include <InputButtonClassifier.h>
#include <RailwaySignal.h>
#include <RailwaySignalDO.h>
#include <DiagnoseCommander.h>
#include <PortMxp.h>

#define GREEN_LOW    (605-10)
#define GREEN_HIGH   (605+10)
#define YELLOW_LOW   (399-10)
#define YELLOW_HIGH  (399+10)
#define RED_LOW      (192-10)
#define RED_HIGH     (192+10)
#define WHITE_LOW    (147-10)
#define WHITE_HIGH   (147+10)


/// Print analogue input values for ADC A0, A1, ..., A5
void printAnalog(void)
{
  int nSensorValue;

  nSensorValue = analogRead(A0);  // G=605, Y=396, R=191, W=147
  Serial.print("A0=");
  Serial.print(nSensorValue);
  nSensorValue = analogRead(A1);  // G=605, Y=NA, R=191
  Serial.print(" A1=");
  Serial.print(nSensorValue);
  nSensorValue = analogRead(A2);  // G=603, Y=396, R=191
  Serial.print(" A2=");
  Serial.print(nSensorValue);
  nSensorValue = analogRead(A3);  // G=604, Y=397, R=193
  Serial.print(" A3=");
  Serial.print(nSensorValue);
  nSensorValue = analogRead(A4);  // G=605, Y=397, R=192
  Serial.print(" A4=");
  Serial.print(nSensorValue);
  nSensorValue = analogRead(A5);  // G=605, Y=---, R=192
  Serial.print(" A5=");
  Serial.print(nSensorValue);
  Serial.println();
}

static const int nPorts[] = { 13,12,11,10,9,8,7,6,5,4,3,2, 22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52 };
static const int nPortsNr = sizeof(nPorts) / sizeof(int);

void activateOutputsOneAfterOther_Init(void)
{
  int nIdx;

  for (nIdx = 0; nIdx < nPortsNr; nIdx++)
  {
    pinMode(nPorts[nIdx], OUTPUT);
    digitalWrite(nPorts[nIdx], LOW);
  }
}

/// Outputs are specified by nPorts.
/// Activate first output port. By pressing button connected to A0, activate next output port.
/// Walk through output ports is possible.
void activateOutputsOneAfterOther(void)
{
  int nSensorValue;
  static int nIdx = 0;
  static boolean bPressed = false;

  nSensorValue = analogRead(A0);  // G=605, Y=399, R=192
  digitalWrite(nPorts[nIdx], HIGH);
  Serial.print(nSensorValue);
  Serial.print(" ");
  Serial.println(nPorts[nIdx]);

  if (nSensorValue < 700)
  {
    if (bPressed == false)
    {
      bPressed = true;
      digitalWrite(nPorts[nIdx], LOW);
      if (nIdx < nPortsNr)
      {
        nIdx++;
      }
      if (nIdx >= nPortsNr)
      {
        nIdx = 0;
      }
    }
  }
  else
  {
    bPressed = false;
  }
}

/// Activate output ports 0 ... 4
void activateOutputs0_4(void)
{
  int nIdx;

  for (nIdx = 0; nIdx < 5; nIdx++)
  {
    digitalWrite(nPorts[nIdx], HIGH);
  }
}

/// Number of classes for input AD values (from buttons)
#define INPUTBUTTON_NR_CLASSES    4

/// configuration
static const unsigned char aucCmds[INPUTBUTTON_NR_CLASSES] = { IfcSignalCommand::GREEN, IfcSignalCommand::WHITE, IfcSignalCommand::RED, IfcSignalCommand::YELLOW };
static const int           aunLow [INPUTBUTTON_NR_CLASSES] = { GREEN_LOW              , WHITE_LOW              , RED_LOW              , YELLOW_LOW  };
static const int           aunHigh[INPUTBUTTON_NR_CLASSES] = { GREEN_HIGH             , WHITE_HIGH             , RED_HIGH             , YELLOW_HIGH };

/// Runable class for input AD value classification: has a classifier per input AD value (that is, per signal)
class RnblInputButtonClassifier : public Runable
{
public:
  typedef InputButtonClassifier<INPUTBUTTON_NR_CLASSES> MyInputButtonClassifier;

  MyInputButtonClassifier btnClassifierS2; // Ausfahrsignal
  MyInputButtonClassifier btnClassifierS4; // Ausfahrsignal
  MyInputButtonClassifier btnClassifierS6; // Ausfahrsignal
  MyInputButtonClassifier btnClassifierS8; // Ausfahrsignal
  MyInputButtonClassifier btnClassifierS10; // Ausfahrsignal
  MyInputButtonClassifier btnClassifierS12; // Blocksignal

  /// Constructor
  RnblInputButtonClassifier()
    : btnClassifierS2 (A4, aucCmds, aunLow, aunHigh)
    , btnClassifierS4 (A3, aucCmds, aunLow, aunHigh)
    , btnClassifierS6 (A2, aucCmds, aunLow, aunHigh)
    , btnClassifierS8 (A1, aucCmds, aunLow, aunHigh)
    , btnClassifierS10(A0, aucCmds, aunLow, aunHigh)
    , btnClassifierS12(A5, aucCmds, aunLow, aunHigh)
  {}
    /// Initialization at system start
  virtual void init(void)
  {}
  /// Main execution function
  virtual void run(void)
  {
    btnClassifierS2.measure();
    btnClassifierS4.measure();
    btnClassifierS6.measure();
    btnClassifierS8.measure();
    btnClassifierS10.measure();
    btnClassifierS12.measure();
  }
};

/// The port multiplexer for SignalCommand
class IfcPortMxpSignalCommand : public IfcSignalCommand
{
public:
  PortMxp2<IfcSignalCommand> mPortMxp2;

  IfcPortMxpSignalCommand(const IFC& p1, const IFC& p2) : mPortMxp2(p1, p2)
  {}

  virtual eIfcSignalCommand getCommand() const { return mPortMxp2.ref().getCommand(); }
};

/// The runable instance for input AD value measurements
RnblInputButtonClassifier rInputButtonClassifier;
/// The port multiplexers for SignalCommand
IfcPortMxpSignalCommand ifcSignalCommandS2(rInputButtonClassifier.btnClassifierS2.mIfcPortSignalCommand, RnblDiagnoseCommander.mDiagnoseCommander.mIfcPortSignalCommand);
IfcPortMxpSignalCommand ifcSignalCommandS4(rInputButtonClassifier.btnClassifierS4.mIfcPortSignalCommand, RnblDiagnoseCommander.mDiagnoseCommander.mIfcPortSignalCommand);
IfcPortMxpSignalCommand ifcSignalCommandS6(rInputButtonClassifier.btnClassifierS6.mIfcPortSignalCommand, RnblDiagnoseCommander.mDiagnoseCommander.mIfcPortSignalCommand);
IfcPortMxpSignalCommand ifcSignalCommandS8(rInputButtonClassifier.btnClassifierS8.mIfcPortSignalCommand, RnblDiagnoseCommander.mDiagnoseCommander.mIfcPortSignalCommand);
IfcPortMxpSignalCommand ifcSignalCommandS10(rInputButtonClassifier.btnClassifierS10.mIfcPortSignalCommand, RnblDiagnoseCommander.mDiagnoseCommander.mIfcPortSignalCommand);
IfcPortMxpSignalCommand ifcSignalCommandS12(rInputButtonClassifier.btnClassifierS12.mIfcPortSignalCommand, RnblDiagnoseCommander.mDiagnoseCommander.mIfcPortSignalCommand);

/// Runable class for signals: has a RailwaySignal per signal and sets target intensities based on input commands (from IfcSignalCommand)
class RnblRailwaySignals : public Runable
{
public:
  RailwaySignal S2;
  RailwaySignal S4;
  RailwaySignal S6;
  RailwaySignal S8;
  RailwaySignal S10;
  RailwaySignal S12;
  /// Constructor
  RnblRailwaySignals()
    : S2(ifcSignalCommandS2)
    , S4(ifcSignalCommandS4)
    , S6(ifcSignalCommandS6)
    , S8(ifcSignalCommandS8)
    , S10(ifcSignalCommandS10)
    , S12(ifcSignalCommandS12)
  {}
  /// Initialization at system start
  virtual void init(void) 
  {}
  /// Main execution function
  virtual void run(void)
  {
    S2.update();
    S4.update();
    S6.update();
    S8.update();
    S10.update();
    S12.update();
  }
};

/// The runable instance for signals
RnblRailwaySignals rRailwaySignals;

/// Runable class for digital output of signal target values (no fading / dimming) 
class RnblRailwaySignalsDO : public Runable
{
public:
  RailwaySignalDO S2;
  RailwaySignalDO S4;
  RailwaySignalDO S6;
  RailwaySignalDO S8;
  RailwaySignalDO S10;
  RailwaySignalDO S12;
  /// Constructor
  RnblRailwaySignalsDO()
    : S2(rRailwaySignals.S2.mIfcPortSignalTarget  , 13, 12, 11, 10,  9)
    , S4(rRailwaySignals.S4.mIfcPortSignalTarget  ,  8,  7,  6,  5,  4)
    , S6(rRailwaySignals.S6.mIfcPortSignalTarget  , 40, 42, 44, 46, 48)
    , S8(rRailwaySignals.S8.mIfcPortSignalTarget  ,  3, 24,  2, 22, -1)
    , S10(rRailwaySignals.S10.mIfcPortSignalTarget, 30, 32, 28, 26, -1)
    , S12(rRailwaySignals.S12.mIfcPortSignalTarget, 52, -1, 50, -1, -1)
  {}
  /// Initialization at system start
  virtual void init(void)
  {}
  /// Main execution function
  virtual void run(void)
  {
    S2.update();
    S4.update();
    S6.update();
    S8.update();
    S10.update();
    S12.update();
  }
};

/// The runable instance
RnblRailwaySignalsDO rRailwaySignalsDO;

/// Runable class for diagnostic commands / control
class RnblDiagnoseCommander : public Runable
{
public:
  /// The diagnose commander
  DiagnoseCommander diagCmd;

  /// Constructor
  RnblDiagnoseCommander()
  {}
  /// Initialization at system start
  virtual void init(void)
  {
    diagCmd.init();
  }
  /// Main execution function
  virtual void run(void)
  {
    diagCmd.update();
  }
};

/// The diagnostic runable
RnblDiagnoseCommander rDiagnoseCommander;

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