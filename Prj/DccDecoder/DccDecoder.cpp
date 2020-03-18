/// DccDecoder.cpp
///
/// DCC Decoder for Arduino
///
/// Implements
/// - setup
/// - loop

#include <Arduino.h>
#include <ISR_Dcc.h>

/// Pin that triggers ISR_Dcc
const byte DccInterruptPin = 2;

void setup()
{
  attachInterrupt(digitalPinToInterrupt(DccInterruptPin), ISR_Dcc, CHANGE);
  Serial.begin(9600);
}

void loop()
{
  static unsigned long ulTimePrint = 0;
  unsigned long ulTimePrintCur = millis();
  if (ulTimePrintCur - ulTimePrint > 1000u)
  {
    Serial.print(Dcc_GetNrOne());
    Serial.print(" ");
    Serial.print(Dcc_GetNrZero());
    Serial.print(" ");
    Serial.print(Dcc_GetByte(0));
    Serial.println();
    ulTimePrint = ulTimePrintCur;
  }
}
