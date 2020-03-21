/// main.cpp
///
/// DCC Decoder for Arduino
///
/// Implements
/// - setup
/// - loop

#include <DccDecoder.h>
#include <Arduino.h>

using namespace Dcc;

DccDecoder MyDecoder;

void setup()
{
  unsigned int unInterruptPin = 2u;

  MyDecoder.setup(unInterruptPin);

  Serial.begin(9600);
}

extern unsigned long aulDbgTimeDiff[100];
extern unsigned char aucDbgState[100];
extern unsigned int unDbgIdx;
bool bPrintTime = false;
bool bPrintPackets = false;

void loop()
{
  MyDecoder.loop();

  static unsigned long ulTimePrint = 0;
  unsigned long ulTimePrintCur = millis();
  int i, j;
  int n;
  if (Serial.available())
  {
    int nRcv;
    nRcv = Serial.read();
    Serial.print("RECEIVED: ");
    Serial.print(nRcv, DEC);
    Serial.println();
    if (nRcv == 't')
    {
      unDbgIdx = 0;
      bPrintTime = false;
    }
    else if (nRcv == 'b')
    {
      bPrintPackets = true;
    }
    else
    {
    }
  }
  if ((unDbgIdx >= 100u) && (!bPrintTime))
  {
    for (i = 0; i < 100; i++)
    {
      Serial.print(aulDbgTimeDiff[i]);
      Serial.print(" ");
      Serial.print(aucDbgState[i]);
      Serial.println();
    }
    bPrintTime = true;
  }
  if (bPrintPackets)
  {
    for (i = 0; i < DCCINTERPRETER_MAXPACKETS; i++)
    {
      for (j = 0; j < MyDecoder.DccInterp.refPacket(i).getNrBytes(); j++)
      {
        Serial.print(MyDecoder.DccInterp.refPacket(i).aBytes[j]);
        Serial.print(" ");
      }
      Serial.println();
    }
    bPrintPackets = false;
  }
  //if (ulTimePrintCur - ulTimePrint > 1000u)
  //{
  //  Serial.print(MyDecoder.DccInterp.getNrOne());
  //  Serial.print(" ");
  //  Serial.print(MyDecoder.DccInterp.getNrZero());
  //  n = MyDecoder.DccInterp.refCurrentPacket().getNrBytes();
  //  for (i = 0; i < n; i++)
  //  {
  //    Serial.print(" ");
  //    Serial.print(MyDecoder.DccInterp.refCurrentPacket().refByte(i));
  //  }
  //  Serial.println();
  //  ulTimePrint = ulTimePrintCur;
  //}
}