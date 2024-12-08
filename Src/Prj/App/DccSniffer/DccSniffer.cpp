/**
  * @file DccSniffer.cpp
  * @author Ralf Sondershaus
  *
  * @brief DCC Sniffer for Arduino
  *
  * @copyright Copyright 2018 - 2023 Ralf Sondershaus
  *
  * This program is free software: you can redistribute it and/or modify it
  * under the terms of the GNU General Public License as published by the
  * Free Software Foundation, either version 3 of the License, or (at your
  * option) any later version.
  *
  * This program is distributed in the hope that it will be useful, but
  * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  * for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <https://www.gnu.org/licenses/>.
  */

#include <Std_Types.h>
#include <Dcc/Decoder.h>
#include <Util/Array.h>
#include <Util/Fix_Queue.h>
#include <Util/Algorithm.h>
#include <Util/Timer.h>
#include <Arduino.h>
#include <Hal/Serial.h>
#include <Hal/Timer.h>

/// The Arduino pin for DCC interrupts
static constexpr uint8 kInterruptPin = 2U;
static constexpr uint8 kBlinkLedPin = 13U;
static constexpr util::MilliTimer::time_type kBlinkLedPeriod_ms = 1000U;
static constexpr uint32 kSerialBaudRate = 115200; // 9600, 19200, 28800, 56000, 115200
static constexpr char kCodeResponse = 0x2E;

using PacketType = dcc::Decoder::PacketType;
using size_type = PacketType::size_type;

// ---------------------------------------------------
/// Local variables
// ---------------------------------------------------
static dcc::Decoder myDecoder;

void toggleLedPin()
{
  static uint8 lastWrite = LOW;

  if (lastWrite == LOW)
  {
    lastWrite = HIGH;
  }
  else
  {
    lastWrite = LOW;
  }

  digitalWrite(kBlinkLedPin, lastWrite);
}

// ---------------------------------------------------
/// Arduino's setup() function.
/// Setup DCC decoder.
/// Setup serial connection.
// ---------------------------------------------------
void setup()
{
  myDecoder.init(kInterruptPin);

  pinMode(kBlinkLedPin, OUTPUT);

  Serial.begin(kSerialBaudRate);
}

static inline uint8 convertNibbleToHex(uint8 ucNibble)
{
  static const uint8 aucChars[16] =
  {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
  };
  // look up and prevent index-out-of-bounds
  return aucChars[ucNibble % 16U];
}

static void convertToHex(uint8 ucByte, uint8 pcBuf[3])
{
  pcBuf[0] = convertNibbleToHex((ucByte >> 4U) & 0x0FU);
  pcBuf[1] = convertNibbleToHex((ucByte >> 0U) & 0x0FU);
  pcBuf[2] = 0;
}

// ----------------------------------------------------
/// Formats bytes to ASCII and prints to Serial
// ----------------------------------------------------
void PrintAscii(const PacketType& pkt)
{
  char pcBuf[3];
  size_type i;

  const size_type n = pkt.getNrBytes();

  for (i = 0; i < n; i++)
  {
    convertToHex(pkt.refByte(i), pcBuf);
    Serial.print(pcBuf);
    Serial.print("-");
  }
  Serial.println();
}

// ----------------------------------------------------
/// Prints bytes to Serial:
/// <0x2E> <len> <bytes> <0>
// ----------------------------------------------------
void PrintBin(const PacketType& pkt)
{
  size_type i;
  // Response byte + length + kMaxNrBytes + 0
  char pcBuf[PacketType::kMaxNrBytes + 3];

  const size_type n = pkt.getNrBytes();

  i = 0;
  pcBuf[i++] = kCodeResponse;
  pcBuf[i++] = n;
  util::copy(pkt.begin(), pkt.end(), &pcBuf[i]);
  i += n;
  pcBuf[i] = 0x00;

  Serial.println(pcBuf);
}

// ---------------------------------------------------
/// Arduino's loop() function.
// ---------------------------------------------------
void loop()
{
  static util::MilliTimer LedTimer;
  static util::MilliTimer DccTimer;

  // alive LED
  if (LedTimer.timeout())
  {
    toggleLedPin();
    LedTimer.start(kBlinkLedPeriod_ms);
    dcc::Decoder::IsrStats stats;
    myDecoder.isrGetStats(stats);
    hal::serial::print(hal::micros());
    hal::serial::print(" ");
    hal::serial::print(myDecoder.getNrInterrupts());
    hal::serial::print(" ");
    hal::serial::print(myDecoder.getNrFetches());
    hal::serial::print(" ");
    hal::serial::print(myDecoder.getNrOnes());
    hal::serial::print(" ");
    hal::serial::print(myDecoder.getNrZeros());
    hal::serial::print(" ");
    hal::serial::print(myDecoder.getNrInvalids());
    hal::serial::print(" ");
    hal::serial::println(myDecoder.getPacketCount());
    hal::serial::print(" ");
    hal::serial::print(stats.curSize);
    hal::serial::print(" ");
    hal::serial::println(stats.maxSize);
  }

  if (DccTimer.timeout()) 
  {
    myDecoder.fetch();
    while (!myDecoder.empty())
    {
      const PacketType& pkt = myDecoder.front();
      //PrintAscii(pkt);
      PrintBin(pkt);
      myDecoder.pop();
    }
    DccTimer.start(10);
  }
}