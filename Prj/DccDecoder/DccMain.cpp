/// main.cpp
///
/// DCC Decoder for Arduino
///
/// Implements
/// - setup
/// - loop

#include <Dcc/Decoder.h>
#include <Arduino.h>
#include <limits.h>

using namespace Dcc;

// ---------------------------------------------------
/// Interface for a handler that is called if a packet is received
// ---------------------------------------------------
template<int MaxNrPackets = 60>
class ArrayHandlerIfc : public Decoder::HandlerIfc
{
public:
  /// Array of packets
  typedef Array<MyPacket, MaxNrPackets> PacketArray;
  /// received valid packets
  PacketArray aPackets;

  /// Default constructor
  ArrayHandlerIfc() {}
  /// Destructor
  virtual ~ArrayHandlerIfc() {}

  /// Store the received packet into the list of packets (if it does not exist already)
  virtual void packetReceived(const MyPacket& pkt)
  {
    auto it = aPackets.find(pkt);
    if (it == aPackets.end())
    {
      if (aPackets.size() < MaxNrPackets)
      {
        aPackets.push_back(pkt);
        aPackets.back().unNrRcv++;
      }
    }
    else
    {
      if (it->unNrRcv < UINT_MAX - 1u)
      {
        it->unNrRcv++;
      }
    }
  }
};

typedef ArrayHandlerIfc<> MyArrayHandlerIfc;

MyArrayHandlerIfc MyHandlerIfc;
Decoder MyDecoder(MyHandlerIfc);

void setup()
{
  unsigned int unInterruptPin = 2u;

  MyDecoder.setup(unInterruptPin);

  Serial.begin(9600);
}

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
  if (bPrintPackets)
  {
    for (auto cit = MyHandlerIfc.aPackets.begin(); cit != MyHandlerIfc.aPackets.end(); cit++)
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