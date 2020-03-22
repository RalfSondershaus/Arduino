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

void printBin(unsigned char b)
{
  int i;
  for (i = 0; i < 8; i++)
  {
    if ((b & 0x80u) == 0)
    {
      Serial.print('0');
    }
    else
    {
      Serial.print('1');
    }
    b <<= 1u;
  }
}
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
    const DccInterpreter::PacketContainer& Container = MyDecoder.DccInterp.refPacketContainer();
    DccInterpreter::PacketContainer::const_iterator cit;
    for (cit = Container.begin(); cit != Container.end(); cit++)
    {
      Serial.print(cit->unNrRcv, DEC);
      Serial.print(", ");
      Serial.print(cit->unNrBits, DEC);
      Serial.print(" -> ");
      n = cit->byteIdx();
      for (i = 0; i < n; i++)
      {
        printBin(cit->refByte(i));
        Serial.print(" ");
      }
      Serial.println();
    }
    bPrintPackets = false;
  }
}