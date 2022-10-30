/// DccDecoder.h
///
/// DCC Decoder for Arduino
///
/// Declares class DccDecoder with
/// - setup
/// - loop

#ifndef DCCDECODER_H
#define DCCDECODER_H

#include <PeriodSM.h>
#include <DccInterpreter.h>

namespace Dcc
{
  /// Main class
  class DccDecoder
  {
  public:
    /// Constructor
    DccDecoder() : periodSM(DccInterp) {}
    /// Desctructor
    ~DccDecoder() {}
    /// Initialize with interrupt pin
    void setup(unsigned int unIntPin);
    /// Loop
    void loop();
    /// The state machine for short and long half bits (-> detecting "1" and "0" and INVALID)
    HalfBitStateMachine periodSM;
    /// The DCC interpreter
    DccInterpreter DccInterp;
    /// for debugging: number of interrupt (ISR) calls
    unsigned int getDebugVal(int i);
  };
} // namespace Dcc

#endif // DCCDECODER_H