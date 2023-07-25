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
#include <Util/Algorithm.h>
#include <Arduino.h>

/// The Arduino pin for DCC interrupts
static constexpr uint16 kInterruptPin = 2U;
static constexpr uint16 kSerialBaudRate = 9600U;

// ---------------------------------------------------
/// Interface for a handler that is called if a packet is received.
/// Stores received packets and counts how often a packet is received.
// ---------------------------------------------------
template<int MaxNrPackets = 60>
class ArrayHandlerIfc : public dcc::Decoder::HandlerIfc
{
public:
  /// Array of packets
  typedef util::array<PacketType, MaxNrPackets> PacketArray;
  /// Array of numbers: the number of received packets
  typedef util::array<uint16, MaxNrPackets> NrPacketsArray;
  /// received valid packets
  PacketArray packets;
  /// Number of received packets
  NrPacketsArray nr_packets;
  /// number of packets in array packets
  uint8 uc_nr_packets;
  /// Default constructor
  ArrayHandlerIfc() = default;
  /// Destructor
  virtual ~ArrayHandlerIfc() = default;

  /// Store the received packet into the list of packets (if it does not exist already)
  virtual void packetReceived(const PacketType& pkt) override
  {
    auto it = util::find(packets.begin(), packets.end(), pkt);
    if (it == packets.end())
    {
      if (uc_nr_packets < static_cast<uint8>(MaxNrPackets))
      {
        packets[uc_nr_packets] = pkt;
        nr_packets[uc_nr_packets]++;
        uc_nr_packets = util::add_no_overflow(uc_nr_packets, static_cast<uint8>(1u));
      }
    }
    else
    {
      auto pos = it - packets.begin();
      nr_packets[pos] = util::add_no_overflow(nr_packets[pos], static_cast<uint16>(1u));
    }
  }
};

// ---------------------------------------------------
/// Local variables
// ---------------------------------------------------
typedef ArrayHandlerIfc<> MyArrayHandlerIfc;

static MyArrayHandlerIfc my_handler_ifc;
static dcc::Decoder my_decoder(my_handler_ifc);
static bool b_print_time = false;
static bool b_print_packets = false;

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
  my_decoder.setup(kInterruptPin);

  Serial.begin(kSerialBaudRate);
}

// ---------------------------------------------------
/// Arduino's loop() function.
// ---------------------------------------------------
void loop()
{
  my_decoder.loop();

  static unsigned long ulTimePrint = 0;
  unsigned long ulTimePrintCur = millis();
  if (Serial.available())
  {
    int n_rcv = Serial.read();
    Serial.print("RECEIVED: ");
    Serial.print(n_rcv, DEC);
    Serial.println();
    if (n_rcv == 't')
    {
      b_print_time = false;
    }
    else if (n_rcv == 'b')
    {
      b_print_packets = true;
    }
    else
    {
    }
  }
  if (b_print_packets)
  {
    uint8 pkt;
    MyArrayHandlerIfc::PacketType::size_type i;

    for (pkt = 0U; pkt < my_handler_ifc.uc_nr_packets; pkt++)
    {
      const MyArrayHandlerIfc::PacketType::size_type n = my_handler_ifc.packets[pkt].getNrBytes();
      Serial.print(my_handler_ifc.nr_packets[pkt], DEC);
      Serial.print(": ");
      for (i = 0; i < n; i++)
      {
        printBin(my_handler_ifc.packets[pkt].refByte(i));
        Serial.print(" ");
      }
      Serial.println();
    }
    b_print_packets = false;
  }
}