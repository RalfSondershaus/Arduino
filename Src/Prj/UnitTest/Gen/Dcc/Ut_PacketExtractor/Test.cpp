/**
  * @file Test.cpp
  *
  * @author Ralf Sondershaus
  *
  * Unit Test for Gen/Dcc/BitExtractor.h and Gen/Dcc/packet_extractor.h
  *
  * @copyright Copyright 2018 - 2024 Ralf Sondershaus
  *
 * SPDX-License-Identifier: Apache-2.0
  */

#include <unity_adapt.h>
#include <array>
#include <Dcc/PacketExtractor.h>

typedef dcc::packet_extractor<> packet_extractor_type;

// -----------------------------------------------------------------------
/// A handler class for new packets.
// -----------------------------------------------------------------------
class PacketExtractorHandlerClass : public packet_extractor_type::handler_ifc
{
public:
  int nReceived;
  packet_type lastpacket;
  PacketExtractorHandlerClass() : nReceived(0)
  {}
  virtual void packet_received(packet_type& pkt) override
  {
    nReceived++;
    lastpacket = pkt;
  }
};

// -----------------------------------------------------------------------
/// @brief Test an invalid preamble (just a single 1 is received)
// -----------------------------------------------------------------------
TEST(Ut_PacketExtractor, packetextractor_preamble_invalid_1_bit)
{
  PacketExtractorHandlerClass packethandler;
  packet_extractor_type packetextractor(packethandler);
  packetextractor.one();
  EXPECT_EQ(packethandler.nReceived, 0);
}

// -----------------------------------------------------------------------
/// @brief Test an invalid preamble (9x "1" are received instead of 10x "1")
// -----------------------------------------------------------------------
TEST(Ut_PacketExtractor, packetextractor_preamble_invalid_9_bit_without_packets)
{
  std::array<uint8_t, 9 + 1 + 8 + 1 + 8 + 1> input = 
  {
    1, 1, 1, 1,  1, 1, 1, 1,  1,
    0,
    1, 0, 1, 0,  1, 0, 1, 0,
    0,
    1, 0, 1, 0,  1, 0, 1, 0,
    0,
  };
  PacketExtractorHandlerClass packethandler;
  packet_extractor_type packetextractor(packethandler);
  for (auto it = input.begin(); it != input.end(); it++)
  {
    if (*it == 0)
    {
      packetextractor.zero();
    }
    else
    {
      packetextractor.one();
    }
  }
  EXPECT_EQ(packethandler.nReceived, 0);
}

// -----------------------------------------------------------------------
/// @brief Test a valid preamble (9x "1" are received instead of 10x "1")
// -----------------------------------------------------------------------
TEST(Ut_PacketExtractor, packetextractor_preamble_invalid_9_bit_with_packets)
{
  std::array<uint8_t, 9 + 1 + 8 + 1 + 8 + 1> input =
  {
    1, 1, 1, 1,  1, 1, 1, 1,  1,
    0,
    1, 0, 1, 0,  1, 0, 1, 0,
    0,
    1, 1, 1, 0,  1, 0, 1, 0,
    1,
  };
  PacketExtractorHandlerClass packethandler;
  packet_extractor_type packetextractor(packethandler);
  dcc::packet<> packet;

  packet.refByte(0) = 0b10101010;
  packet.refByte(1) = 0b11101010;
  for (auto it = input.begin(); it != input.end(); it++)
  {
    if (*it == 0)
    {
      packetextractor.zero();
    }
    else
    {
      packetextractor.one();
    }
  }
  EXPECT_EQ(packethandler.nReceived, 0);
}

// -----------------------------------------------------------------------
/// @brief Test a valid preamble (10x "1" are received)
// -----------------------------------------------------------------------
TEST(Ut_PacketExtractor, packetextractor_preamble_valid_10_bit)
{
  std::array<uint8_t, 10 + 1 + 8 + 1 + 8 + 1> input =
  {
    1, 1, 1, 1,  1, 1, 1, 1,  1, 1,
    0,
    1, 0, 1, 0,  1, 0, 1, 0,
    0,
    1, 1, 1, 0,  1, 0, 1, 0,
    1,
  };
  PacketExtractorHandlerClass packethandler;
  packet_extractor_type packetextractor(packethandler);
  dcc::packet<> packet;

  packet.refByte(0) = 0b10101010;
  packet.refByte(1) = 0b11101010;
  for (auto it = input.begin(); it != input.end(); it++)
  {
    if (*it == 0)
    {
      packetextractor.zero();
    }
    else
    {
      packetextractor.one();
    }
  }
  EXPECT_EQ(packethandler.nReceived, 1);
  EXPECT_EQ(packet.refByte(0) == packethandler.lastpacket.refByte(0), true);
  EXPECT_EQ(packet.refByte(1) == packethandler.lastpacket.refByte(1), true);
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

  RUN_TEST(packetextractor_preamble_invalid_1_bit);
  RUN_TEST(packetextractor_preamble_invalid_9_bit_without_packets);
  RUN_TEST(packetextractor_preamble_invalid_9_bit_with_packets);
  RUN_TEST(packetextractor_preamble_valid_10_bit);

  (void) UNITY_END();

  // Return false to stop program execution (relevant on Windows)
  return false;
}
