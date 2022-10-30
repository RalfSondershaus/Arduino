/**
  * @file DccSniffer.cpp
  *
  * @author Ralf Sondershaus
  *
  * @brief DCC Sniffer for Arduino
  */

#include <Dcc/Decoder.h>
#include <Util/Array.h>
#include <Arduino.h>

using namespace Dcc;
using namespace Util;

/// The Arduino pin for DCC interrupts
static constexpr unsigned int unInterruptPin = 2U;

// ---------------------------------------------------
/// Interface for a handler that is called if a packet is received.
/// Stores received packets and counts how often a packet is received.
// ---------------------------------------------------
template<int MaxNrPackets = 60>
class ArrayHandlerIfc : public Decoder::HandlerIfc
{
public:
  /// Array of packets
  typedef Array<PacketType, MaxNrPackets> PacketArray;
  /// received valid packets
  PacketArray packets;

  /// Default constructor
  ArrayHandlerIfc() = default;
  /// Destructor
  virtual ~ArrayHandlerIfc() = default;

  /// Store the received packet into the list of packets (if it does not exist already)
  virtual void packetReceived(const PacketType& pkt) override
  {
    auto it = packets.find(pkt);
    if (it == packets.end())
    {
      if (packets.size() < MaxNrPackets)
      {
        packets.push_back(pkt);
        packets.back().unNrRcv++;
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

// ---------------------------------------------------
/// Local variables
// ---------------------------------------------------
typedef ArrayHandlerIfc<> MyArrayHandlerIfc;

static MyArrayHandlerIfc MyHandlerIfc;
static Decoder MyDecoder(MyHandlerIfc);
static bool bPrintTime = false;
static bool bPrintPackets = false;

// ---------------------------------------------------
// ---------------------------------------------------
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

// ---------------------------------------------------
/// Arduino's setup() function.
/// Setup DCC decoder.
/// Setup serial connection.
// ---------------------------------------------------
void setup()
{
  MyDecoder.setup(unInterruptPin);

  Serial.begin(9600);
}

// ---------------------------------------------------
/// Arduino's loop() function.
// ---------------------------------------------------
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