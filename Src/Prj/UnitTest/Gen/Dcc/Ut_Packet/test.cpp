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

using packet_type = dcc::Packet<6>::packet_type;
using address_type = dcc::Packet<6>::address_type;

/// @brief Specialization for T = dcc::Packet<>::packet_type
template <>
void EXPECT_EQ<dcc::Packet<>::packet_type, dcc::Packet<>::packet_type>(dcc::Packet<>::packet_type expected, dcc::Packet<>::packet_type actual)
{
    EXPECT_EQ(static_cast<int>(expected), static_cast<int>(actual));
}

/// Helper function: add all bits of a byte to a packet. Start with MSB, end with LSB.
template <int N>
void addByteToPacket(dcc::Packet<N> &pkt, uint8 byte)
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
    std::array<uint8_t, 8> byte0 = {1, 0, 1, 1, 0, 1, 1, 0};
    std::array<uint8_t, 8> byte1 = {0, 1, 1, 0, 0, 0, 0, 1};
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
    std::array<uint8_t, 8> byte0 = {1, 0, 1, 1, 0, 1, 1, 0};
    std::array<uint8_t, 8> byte1 = {0, 1, 1, 0, 0, 0, 0, 1};

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
    std::array<uint8_t, 8> byte0 = {1, 0, 1, 1, 0, 1, 1, 0};
    std::array<uint8_t, 8> byte1 = {0, 1, 1, 0, 0, 0, 0, 1};

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
    std::array<uint8_t, 8> byte0 = {1, 0, 1, 1, 0, 1, 1, 0};
    std::array<uint8_t, 8> byte1 = {0, 1, 1, 0, 0, 0, 0, 1};

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
    // BasicAccessory
    // {preamble} 0 10AAAAAA 0 1AAACDDD 0 EEEEEEEE 1
    // {preamble} 0 10000001 0 1111CDDD 0 EEEEEEEE 1
    addByteToPacket(packet, 0b10000001U);
    addByteToPacket(packet, 0b11110111U);
    addByteToPacket(packet, packet.refByte(0) ^ packet.refByte(1));

    EXPECT_EQ(packet.get_type(), dcc::Packet<6>::packet_type::BasicAccessory);
    EXPECT_EQ(packet.ba_get_output_power(), static_cast<uint8>(0U));
    EXPECT_EQ(packet.ba_get_DDD(), static_cast<uint8>(0b111U));
}

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_012_type_ExtendedAccessory_Correct)
{
    dcc::Packet<6> packet;

    // ExtendedAccessory
    // {preamble} 0 10AAAAAA 0 0AAA0AA1 0 000XXXXX 0 EEEEEEEE 1
    // {preamble} 0 10000001 0 01110111 0 00010101 0 EEEEEEEE 1
    addByteToPacket(packet, 0b10000001U);
    addByteToPacket(packet, 0b01110111U);
    addByteToPacket(packet, 0b00010101U);
    addByteToPacket(packet, (packet.refByte(0) ^ packet.refByte(1)) ^ packet.refByte(2));

    EXPECT_EQ(packet.get_type(), dcc::Packet<6>::packet_type::ExtendedAccessory);
    EXPECT_EQ(packet.ea_get_aspect(), static_cast<uint8>(0b00010101U));
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

/*
User Address | Linear                        | Non-Linear
             | Byte 1   | Byte 2   | A10..A0 | Byte 1   | Byte 2   | A10..A0
1            | 10000001 | 1111D00R | 4       | 10000001 | 1111D00R | 4
252          | 10111111 | 1111D11R | 255     | 10111111 | 1111D11R | 255
253          | 10000000 | 1110D00R | 256     | 10000000 | 1111D00R | 0
254          | 10000000 | 1110D01R | 257     | 10000000 | 1111D01R | 1
255          | 10000000 | 1110D10R | 258     | 10000000 | 1111D10R | 2
 */
struct Test
{
    uint8 byte1;
    uint8 byte2;
    address_type address;
};

static util::array<Test, 11> test_array = {{
    {0b10000001, 0b11110000, 1},    // 0
    {0b10111111, 0b11110110, 252},  // 1
    {0b10000000, 0b11100000, 253},  // 2
    {0b10000000, 0b11100010, 254},  // 3
    {0b10000000, 0b11100100, 255},  // 4
    {0b10111111, 0b10000100, 2043}, // 5
    {0b10111111, 0b10000110, 2044}, // 6
    {0b10000000, 0b11110000, 2045}, // 7
    {0b10000000, 0b11110010, 2046}, // 8
    {0b10000000, 0b11110100, 2047}, // 9
    {0b10000000, 0b11110110, 2048}  // 10
}};

//-------------------------------------------------------------------------
void test_address_BasicAccessory_OutputAddress(size_t index)
{
    dcc::Packet<6> packet;
    constexpr uint8 cv29 = 0b01000000; // CV29, bit 6 = 1: Output address method
                                       // could be cfg::kBitMask_Cv29_OutputAddressMethod

    // BasicAccessory with output address method (CV29, bit 6 = 1)
    // {preamble} 0 10AAAAAA 0 1ĀĀĀDAAR 0 EEEEEEEE 1 [S-9.2.1 2025]
    addByteToPacket(packet, test_array[index].byte1);
    addByteToPacket(packet, test_array[index].byte2);
    addByteToPacket(packet, packet.refByte(0) ^ packet.refByte(1));

    EXPECT_EQ(packet.get_type(), dcc::Packet<6>::packet_type::BasicAccessory);
    EXPECT_EQ(packet.get_address(cv29), static_cast<address_type>(test_array[index].address));
}

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_014_address_BasicAccessory_OutputAddress_1)
{
    test_address_BasicAccessory_OutputAddress(0);
}

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_014_address_BasicAccessory_OutputAddress_252)
{
    test_address_BasicAccessory_OutputAddress(1);
}

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_014_address_BasicAccessory_OutputAddress_253)
{
    test_address_BasicAccessory_OutputAddress(2);
}

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_014_address_BasicAccessory_OutputAddress_254)
{
    test_address_BasicAccessory_OutputAddress(3);
}

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_014_address_BasicAccessory_OutputAddress_255)
{
    test_address_BasicAccessory_OutputAddress(4);
}

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_014_address_BasicAccessory_OutputAddress_2043)
{
    test_address_BasicAccessory_OutputAddress(5);
}

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_014_address_BasicAccessory_OutputAddress_2044)
{
    test_address_BasicAccessory_OutputAddress(6);
}

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_014_address_BasicAccessory_OutputAddress_2045)
{
    test_address_BasicAccessory_OutputAddress(7);
}

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_014_address_BasicAccessory_OutputAddress_2046)
{
    test_address_BasicAccessory_OutputAddress(8);
}

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_014_address_BasicAccessory_OutputAddress_2047)
{
    test_address_BasicAccessory_OutputAddress(9);
}

//-------------------------------------------------------------------------
TEST(Ut_Packet, packet_014_address_BasicAccessory_OutputAddress_2048)
{
    test_address_BasicAccessory_OutputAddress(10);
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
    RUN_TEST(packet_014_address_BasicAccessory_OutputAddress_1);
    RUN_TEST(packet_014_address_BasicAccessory_OutputAddress_252);
    RUN_TEST(packet_014_address_BasicAccessory_OutputAddress_253);
    RUN_TEST(packet_014_address_BasicAccessory_OutputAddress_254);
    RUN_TEST(packet_014_address_BasicAccessory_OutputAddress_255);
    RUN_TEST(packet_014_address_BasicAccessory_OutputAddress_2043);
    RUN_TEST(packet_014_address_BasicAccessory_OutputAddress_2044);
    RUN_TEST(packet_014_address_BasicAccessory_OutputAddress_2045);
    RUN_TEST(packet_014_address_BasicAccessory_OutputAddress_2046);
    RUN_TEST(packet_014_address_BasicAccessory_OutputAddress_2047);
    RUN_TEST(packet_014_address_BasicAccessory_OutputAddress_2048);

    (void)UNITY_END();

    // Return false to stop program execution (relevant on Windows)
    return false;
}
