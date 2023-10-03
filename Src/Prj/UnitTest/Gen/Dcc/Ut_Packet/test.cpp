 /**
 * @file test.cpp
   *
   * @author Ralf Sondershaus
   *
 * Google Test for Gen/Dcc/Packet.h
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

#include <Dcc/Packet.h>
#include <gtest/gtest.h>
#include <array>

TEST(Ut_Packet, packet_empty)
{
  dcc::Packet<6> packet;
  int i;

  for (i = 0; i < 6; i++)
  {
    EXPECT_EQ(packet.refByte(i), 0);
  }
  EXPECT_EQ(packet.getNrBytes(), 0u);
}

TEST(Ut_Packet, packet_add_2_bits)
{
  dcc::Packet<6> packet;

  packet.addBit(1);
  EXPECT_EQ(packet.refByte(0), 1);
  packet.addBit(1);
  EXPECT_EQ(packet.refByte(0), 3);
  EXPECT_EQ(packet.getNrBytes(), 1u);
}

TEST(Ut_Packet, packet_add_16_bits)
{
  dcc::Packet<6> packet;
  std::array<uint8_t, 8> byte0 = { 1, 0, 1, 1,  0, 1, 1, 0 };
  std::array<uint8_t, 8> byte1 = { 0, 1, 1, 0,  0, 0, 0, 1 };
  uint8_t byte;

  byte = 0;
  for (auto it = byte0.begin(); it != byte0.end(); it++)
  {
    packet.addBit(*it);
    byte = static_cast<uint8_t>((byte << 1u) | *it);
    EXPECT_EQ(packet.refByte(0), byte);
    EXPECT_EQ(packet.getNrBytes(), 1u);
  }

  byte = 0;
  for (auto it = byte1.begin(); it != byte1.end(); it++)
  {
    packet.addBit(*it);
    byte = static_cast<uint8_t>((byte << 1u) | *it);
    EXPECT_EQ(packet.refByte(1), byte);
    EXPECT_EQ(packet.getNrBytes(), 2u);
  }
}

TEST(Ut_Packet, packet_copy_constructor)
{
  dcc::Packet<6> packet;
  std::array<uint8_t, 8> byte0 = { 1, 0, 1, 1,  0, 1, 1, 0 };
  std::array<uint8_t, 8> byte1 = { 0, 1, 1, 0,  0, 0, 0, 1 };

  for (auto it = byte0.begin(); it != byte0.end(); it++)
  {
    packet.addBit(*it);
  }

  for (auto it = byte1.begin(); it != byte1.end(); it++)
  {
    packet.addBit(*it);
  }

  auto packet_copy = packet; // copy initialization
  EXPECT_EQ(packet_copy.refByte(0), 0b10110110u);
  EXPECT_EQ(packet_copy.refByte(1), 0b01100001u);
  EXPECT_EQ(packet_copy.getNrBytes(), 2u);
}

TEST(Ut_Packet, packet_copy_assignment)
{
  dcc::Packet<6> packet;
  dcc::Packet<6> packet_copy;
  std::array<uint8_t, 8> byte0 = { 1, 0, 1, 1,  0, 1, 1, 0 };
  std::array<uint8_t, 8> byte1 = { 0, 1, 1, 0,  0, 0, 0, 1 };

  for (auto it = byte0.begin(); it != byte0.end(); it++)
  {
    packet.addBit(*it);
  }

  for (auto it = byte1.begin(); it != byte1.end(); it++)
  {
    packet.addBit(*it);
  }

  packet_copy = packet; // copy assignment
  EXPECT_EQ(packet_copy.refByte(0), 0b10110110u);
  EXPECT_EQ(packet_copy.refByte(1), 0b01100001u);
  EXPECT_EQ(packet_copy.getNrBytes(), 2u);
}

TEST(Ut_Packet, packet_operator_equal)
{
  dcc::Packet<6> packet;
  dcc::Packet<6> packet_copy;
  std::array<uint8_t, 8> byte0 = { 1, 0, 1, 1,  0, 1, 1, 0 };
  std::array<uint8_t, 8> byte1 = { 0, 1, 1, 0,  0, 0, 0, 1 };

  for (auto it = byte0.begin(); it != byte0.end(); it++)
  {
    packet.addBit(*it);
  }

  for (auto it = byte1.begin(); it != byte1.end(); it++)
  {
    packet.addBit(*it);
  }

  packet_copy = packet; // copy assignment

  EXPECT_EQ(packet_copy == packet, true);

  packet.addBit(1);

  EXPECT_EQ(packet_copy == packet, false);
}