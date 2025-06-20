 /**
 * @file Test.cpp
   *
   * @author Ralf Sondershaus
   *
   * Unit Test for Gen/Dcc/Packet.h
   *
   * @copyright Copyright 2018 - 2024 Ralf Sondershaus
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

#include <unity_adapt.h>
#include <array>
#include <Dcc/Packet.h>

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

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_001_empty)
{
  dcc::Packet<6> packet;
  int i;

  for (i = 0; i < 6; i++)
  {
    EXPECT_EQ(packet.refByte(i), static_cast<uint8>(0));
  }
  EXPECT_EQ(packet.getNrBytes(), 0u);
}

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_002_add_2_bits)
{
  dcc::Packet<6> packet;

  packet.addBit(1);
  EXPECT_EQ(packet.refByte(0), static_cast<uint8>(1));
  packet.addBit(1);
  EXPECT_EQ(packet.refByte(0), static_cast<uint8>(3));
  EXPECT_EQ(packet.getNrBytes(), 1u);
}

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_003_add_16_bits)
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

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_004_copy_constructor)
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
  EXPECT_EQ(packet_copy.refByte(0), static_cast<uint8>(0b10110110u));
  EXPECT_EQ(packet_copy.refByte(1), static_cast<uint8>(0b01100001u));
  EXPECT_EQ(packet_copy.getNrBytes(), 2u);
}

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_005_copy_assignment)
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
  EXPECT_EQ(packet_copy.refByte(0), static_cast<uint8>(0b10110110u));
  EXPECT_EQ(packet_copy.refByte(1), static_cast<uint8>(0b01100001u));
  EXPECT_EQ(packet_copy.getNrBytes(), 2u);
}

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_006_operator_equal)
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

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_007_checksum_BasicAccessory_Correct)
{
  dcc::Packet<6> packet;
  bool result;

  // BasicAccessory
  // {preamble} 0 10AAAAAA 0 1AAACDDD 0 EEEEEEEE 1
  // {preamble} 0 10000001 0 1111CDDD 0 EEEEEEEE 1
  addByteToPacket(packet, 0b10000001U);
  addByteToPacket(packet, 0b11110000U);
  addByteToPacket(packet, packet.refByte(0) ^ packet.refByte(1));

  result = packet.testChecksum();
  EXPECT_EQ(result, true);
}

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_008_checksum_BasicAccessory_Incorrect)
{
  dcc::Packet<6> packet;
  bool result;

  // BasicAccessory
  // {preamble} 0 10AAAAAA 0 1AAACDDD 0 EEEEEEEE 1
  // {preamble} 0 10000001 0 1111CDDD 0 EEEEEEEE 1
  addByteToPacket(packet, 0b10000001U);
  addByteToPacket(packet, 0b11110000U);
  addByteToPacket(packet, 0U);

  result = packet.testChecksum();
  EXPECT_EQ(result, false);
}

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_009_checksum_ExtendedAccessory_Correct)
{
  dcc::Packet<6> packet;
  bool result;

  // ExtendedAccessory
  // {preamble} 0 10AAAAAA 0 0AAA0AA1 0 000XXXXX 0 EEEEEEEE 1
  // {preamble} 0 10000001 0 01110111 0 00010101 0 EEEEEEEE 1
  addByteToPacket(packet, 0b10000001U);
  addByteToPacket(packet, 0b11110111U);
  addByteToPacket(packet, 0b00010101U);
  addByteToPacket(packet, (packet.refByte(0) ^ packet.refByte(1)) ^ packet.refByte(2));

  result = packet.testChecksum();
  EXPECT_EQ(result, true);
}

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_010_checksum_ExtendedAccessory_Incorrect)
{
  dcc::Packet<6> packet;
  bool result;

  // ExtendedAccessory
  // {preamble} 0 10AAAAAA 0 0AAA0AA1 0 000XXXXX 0 EEEEEEEE 1
  // {preamble} 0 10000001 0 01110111 0 00010101 0 EEEEEEEE 1
  addByteToPacket(packet, 0b10000001U);
  addByteToPacket(packet, 0b11110111U);
  addByteToPacket(packet, 0b00010101U);
  addByteToPacket(packet, 0U);

  result = packet.testChecksum();
  EXPECT_EQ(result, false);
}

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_011_type_BasicAccessory_Correct)
{
  dcc::Packet<6> packet;
  dcc::Packet<6>::packet_type type;
  dcc::Packet<6>::config_type cfg { .Accessory_OutputAddressMethod = 0, .Multifunction_ExtendedAddressing = 0 };
  // BasicAccessory
  // {preamble} 0 10AAAAAA 0 1AAACDDD 0 EEEEEEEE 1
  // {preamble} 0 10000001 0 1111CDDD 0 EEEEEEEE 1
  addByteToPacket(packet, 0b10000001U);
  addByteToPacket(packet, 0b11110111U);
  addByteToPacket(packet, packet.refByte(0) ^ packet.refByte(1));

  type = packet.decode(cfg);
  EXPECT_EQ(type, dcc::Packet<6>::packet_type::BasicAccessory);
  EXPECT_EQ(packet.getType(), dcc::Packet<6>::packet_type::BasicAccessory);
  EXPECT_EQ(packet.baGetC(), static_cast<uint8>(0U));
  EXPECT_EQ(packet.baGetD(), static_cast<uint8>(0b111U));
}

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_012_type_ExtendedAccessory_Correct)
{
  dcc::Packet<6> packet;
  dcc::Packet<6>::packet_type type;
  dcc::Packet<6>::config_type cfg { .Accessory_OutputAddressMethod = 0, .Multifunction_ExtendedAddressing = 0 };

  // ExtendedAccessory
  // {preamble} 0 10AAAAAA 0 0AAA0AA1 0 000XXXXX 0 EEEEEEEE 1
  // {preamble} 0 10000001 0 01110111 0 00010101 0 EEEEEEEE 1
  addByteToPacket(packet, 0b10000001U);
  addByteToPacket(packet, 0b01110111U);
  addByteToPacket(packet, 0b00010101U);
  addByteToPacket(packet, (packet.refByte(0) ^ packet.refByte(1)) ^ packet.refByte(2));

  type = packet.decode(cfg);
  EXPECT_EQ(type, dcc::Packet<6>::packet_type::ExtendedAccessory);
  EXPECT_EQ(packet.getType(), dcc::Packet<6>::packet_type::ExtendedAccessory);
  EXPECT_EQ(packet.eaGetAspect(), static_cast<uint8>(0b00010101U));
}

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_013_checksum_MultiFunction7_03_3F_0D_0A)
{
  dcc::Packet<6> packet;
  bool result;

  // BasicAccessory
  // {preamble} 0 10AAAAAA 0 1AAACDDD 0 EEEEEEEE 1
  // {preamble} 0 10000001 0 1111CDDD 0 EEEEEEEE 1
  addByteToPacket(packet, 0x03);
  addByteToPacket(packet, 0x3F);
  addByteToPacket(packet, 0x0D);
  addByteToPacket(packet, 0x0A);

  result = packet.testChecksum();
  EXPECT_EQ(result, false);
}


void setUp(void)
{
}

void tearDown(void)
{
}

void test_setup(void)
{
}

bool test_loop(void)
{
  UNITY_BEGIN();

  RUN_TEST(packet_001_empty);
  RUN_TEST(packet_002_add_2_bits);
  RUN_TEST(packet_003_add_16_bits);
  RUN_TEST(packet_004_copy_constructor);
  RUN_TEST(packet_005_copy_assignment);
  RUN_TEST(packet_006_operator_equal);
  RUN_TEST(packet_007_checksum_BasicAccessory_Correct);
  RUN_TEST(packet_008_checksum_BasicAccessory_Incorrect);
  RUN_TEST(packet_009_checksum_ExtendedAccessory_Correct);
  RUN_TEST(packet_010_checksum_ExtendedAccessory_Incorrect);
  RUN_TEST(packet_011_type_BasicAccessory_Correct);
  RUN_TEST(packet_012_type_ExtendedAccessory_Correct);
  RUN_TEST(packet_013_checksum_MultiFunction7_03_3F_0D_0A);

  (void) UNITY_END();

  // Return false to stop program execution (relevant on Windows)
  return false;
}

