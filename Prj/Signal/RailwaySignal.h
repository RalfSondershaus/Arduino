/**
 * Class RailwaySignal.
 */

#ifndef RAILWAYSIGNAL_H__
#define RAILWAYSIGNAL_H__

#include <IfcSignalCommand.h>
#include <IfcDiagnose.h>
#include <IfcSignalTarget.h>

/// [% / 10 ms] default dim up speed
#define SIGNAL_DIM_INC  20u
/// [% / 10 ms] default dim down speed
#define SIGNAL_DIM_DEC  20u

// -----------------------------------------------------------------------------------
/// Set target intensities based on input commands for signals
// -----------------------------------------------------------------------------------
class RailwaySignal
{
public:
  /// type for intensities [%]
  typedef IfcSignalTarget::tIntensity1 tIntensity1;
  /// type for intensity speeds [% / 10 ms]
  typedef IfcSignalTarget::tSpeed1_10 tSpeed1_10;
  /// type for diagnostic commands
  typedef IfcDiagnose::eIfcDiagnosticCommand eIfcDiagnosticCommand;

  /// type of my (provided) IfcSignalTarget port
  class IfcPortSignalTarget : public IfcSignalTarget
  {
  public:
    /// Parent
    const RailwaySignal& mParent;
    /// Constructor
    IfcPortSignalTarget(const RailwaySignal& s) : mParent(s)
    {}
    /// Return target intensity for red 1.
    virtual tIntensity1 getRed1() const { return mParent.ucOutRed1; }
    /// Return target intensity for red 2.
    virtual tIntensity1 getRed2() const { return mParent.ucOutRed2; }
    /// Return target intensity for green.
    virtual tIntensity1 getGreen1() const { return mParent.ucOutGreen; }
    /// Return target intensity for white.
    virtual tIntensity1 getWhite1() const { return mParent.ucOutWhite; }
    /// Return target intensity for yellow.
    virtual tIntensity1 getYellow1() const { return mParent.ucOutYellow; }
    /// Return target increment value (speed)
    virtual tSpeed1_10 getTargetInc() const { return mParent.ucInc; }
    /// Return target decrement value (speed)
    virtual tSpeed1_10 getTargetDec() const { return mParent.ucDec; }
  };
  /// The (provided) port for signal targets
  IfcPortSignalTarget mIfcPortSignalTarget;

protected:
  tIntensity1 ucOutRed1;    ///< [%] Intensity
  tIntensity1 ucOutRed2;    ///< [%] Intensity
  tIntensity1 ucOutGreen;   ///< [%] Intensity
  tIntensity1 ucOutYellow;  ///< [%] Intensity
  tIntensity1 ucOutWhite;   ///< [%] Intensity
  tSpeed1_10  ucInc;        ///< [% / 10 ms] dim up speed
  tSpeed1_10  ucDec;        ///< [% / 10 ms] dim down speed

  /// Reference to the (required) port for signal commands
  const IfcSignalCommand& mIfcSignalCommand;
  /// Reference to the (required, optional) port for diagnostic commands
  const IfcDiagnose * mpIfcDiagnose;
  /// Return true if a diagnostic interface is attached, return false otherwise
  bool isDiagnoseAttached() const { return mpIfcDiagnose != (const IfcDiagnose *) nullptr;  }
  /// Return current diagnostic command (if any, return NONE otherwise)
  eIfcDiagnosticCommand getDiagnoseCommand() const
  {
    eIfcDiagnosticCommand cmd = eIfcDiagnosticCommand::NONE;
    if (isDiagnoseAttached())
    {
      cmd = mpIfcDiagnose->getCommand();
    }
    return cmd;
  }
public:

  RailwaySignal(const IfcSignalCommand& cmd, const IfcDiagnose * pDiag = (const IfcDiagnose *) nullptr) 
    :  (cmd)
    , mpIfcDiagnose(pDiag)
    , mIfcPortSignalTarget(*this)
    , ucOutRed1(0)
    , ucOutRed2(0)
    , ucOutGreen(0)
    , ucOutYellow(0)
    , ucOutWhite(0)
    , ucInc(SIGNAL_DIM_INC)
    , ucDec(SIGNAL_DIM_DEC)
  {}
  ~RailwaySignal()
  {}
  /// Set a diagnostic interface
  void setIfcDiagnose(const IfcDiagnose * pDiag) { mpIfcDiagnose = pDiag ; }
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
  void update( )
  {
    if (getDiagnoseCommand() != eIfcDiagnosticCommand::NONE)
    {
    }
    else
    {
      switch (mIfcSignalCommand.getCommand())
      {
      case IfcSignalCommand::RED:      hp0(); break;
      case IfcSignalCommand::GREEN:    hp1(); break;
      case IfcSignalCommand::YELLOW:   hp2(); break;
      case IfcSignalCommand::WHITE:    hp3(); break;
      case IfcSignalCommand::ALL:      activateAll(); break;
      default:                         hp0(); break;
      }
    }
  }
};

#endif // RAILWAYSIGNAL_H__
