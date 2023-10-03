/**
  * @file test.cpp
  *
  * @author Ralf Sondershaus
  *
  * Google Test for Gen/Dcc/BitExtractor.h and Gen/Dcc/PacketExtractor.h
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
  * along with this program. If not, see <https://www.gnu.org/licenses/>.
  */

#include <Dcc/PacketExtractor.h>
#include <gtest/gtest.h>
#include <array>

typedef dcc::PacketExtractor<> PacketExtractorType;

// -----------------------------------------------------------------------
/// A handler class for new packets.
// -----------------------------------------------------------------------
class PacketExtractorHandlerClass : public PacketExtractorType::HandlerIfc
{
public:
  int nReceived;
  PacketType lastpacket;
  PacketExtractorHandlerClass() : nReceived(0)
  {}
  virtual void packetReceived(const PacketType& pkt)
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
  PacketExtractorType packetextractor(packethandler);
  packetextractor.one();
  EXPECT_EQ(packethandler.nReceived, 0);
}

// -----------------------------------------------------------------------
/// @brief Test an invalid preamble (9x "1" are received instead of 10x "1")
// -----------------------------------------------------------------------
TEST(Ut_PacketExtractor, packetextractor_preamble_invalid_9_bit)
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
  PacketExtractorType packetextractor(packethandler);
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
  PacketExtractorType packetextractor(packethandler);
  dcc::Packet<> packet;

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