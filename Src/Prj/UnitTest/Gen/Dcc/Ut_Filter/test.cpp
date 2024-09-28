/**
  * @file test.cpp
  *
  * @author Ralf Sondershaus
  *
  * Google Test for Gen/Dcc/Filter.h
  *
  * @copyright Copyright 2023 Ralf Sondershaus
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
  * along with this program. If not, see <https://www.gnu.org/licenses/>.
  */

#include <Dcc/Filter.h>
#include <gtest/gtest.h>
#include <array>

/// Helper function: add all bits of a byte to a packet. Start with MSB, end with LSB.
template<int N>
void addByteToPacket(dcc::Packet<N>& pkt, uint8 byte)
{
  for (size_t i = 0; i < 8U; i++)
  {
    pkt.addBit((byte & static_cast<uint8>(0x80)) ? 1U : 0U);
    byte <<= 1U;
  }
}

/// 00000000              0         Broadcast address
/// 00000001-01111111     1 - 127   Multi-Function decoders with 7 bit addresses (locomotives)
/// 10000000-10111111   128 - 191   Basic Accessory Decoders with 9 bit addresses and Extended Accessory Decoders with 11-bit addresses
/// 11000000-11100111   192 - 231   Multi Function Decoders with 14 bit addresses (locomotives)
/// 11101000-11111110   232 - 254   Reserved for Future Use
/// 11111111            255         Idle Packet

// -----------------------------------------------------------------------
/// @brief Test if a pass filter for primary addresses is working: 
/// uninitialized and initialized.
// -----------------------------------------------------------------------
TEST(Ut_Filter, filter_PassPrimaryAddressFilter_1)
{
  typedef dcc::Packet<6> PacketType;
  typedef dcc::PassPrimaryAddressFilter<PacketType> FilterType;

  FilterType filter;
  PacketType packet;
  uint8 address = 128;

  // BasicAccessory
  // {preamble} 0 10AAAAAA 0 1AAACDDD 0 EEEEEEEE 1
  // {preamble} 0 10000001 0 1111CDDD 0 EEEEEEEE 1
  addByteToPacket(packet, address);
  addByteToPacket(packet, 0b11110000U);
  addByteToPacket(packet, packet.refByte(0) ^ packet.refByte(1));

  packet.decode();

  EXPECT_EQ(filter.filter(packet), false);

  filter.setLo(PacketType::kPrimaryAddressAccessory_Lo);
  filter.setHi(PacketType::kPrimaryAddressAccessory_Lo);

  EXPECT_EQ(filter.filter(packet), true);
}

// -----------------------------------------------------------------------
/// @brief Test if a pass filter for primary addresses is working: 
/// uninitialized and initialized.
// -----------------------------------------------------------------------
TEST(Ut_Filter, filter_PassAddressFilter_1)
{
  typedef dcc::Packet<6> PacketType;
  typedef dcc::PassAddressFilter<PacketType> FilterType;

  FilterType filter;
  PacketType packet;
  uint16 address = 128; // 9 bits of address

  // BasicAccessory
  // {preamble} 0 10AAAAAA 0 1AAACDDD 0 EEEEEEEE 1
  // The most significant bits of the 9 - bit address are bits 4 - 6 of the
  // second data byte. By convention these bits(bits 4 - 6 of the second data byte)
  // are in ones complement.
  addByteToPacket(packet, 0b10000000U | static_cast<uint8>((( address & 0x003FU) >> 0U)));
  addByteToPacket(packet, 0b10000000U | static_cast<uint8>(((~address & 0x01C0U) >> 2U)));
  addByteToPacket(packet, packet.refByte(0) ^ packet.refByte(1));

  packet.decode();

  EXPECT_EQ(filter.filter(packet), false);

  filter.setLo(address);
  filter.setHi(address);

  EXPECT_EQ(filter.filter(packet), true);
}

