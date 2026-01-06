/**
 * @file test.cpp
 *
 * @author Ralf Sondershaus
 *
 * @brief Unit test for Gen/Dcc/Filter.h
 *
 * @copyright Copyright 2023 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Dcc/Filter.h>
#include <Dcc/DecoderCfg.h>
#include <unity_adapt.h>

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
    using packet_type = dcc::packet<6>;
    using filter_type = dcc::pass_primary_address_filter<packet_type>;

    filter_type filter;

    // BasicAccessory
    // {preamble} 0 10AAAAAA 0 1AAACDDD 0 EEEEEEEE 1
    // {preamble} 0 10000001 0 1111CDDD 0 EEEEEEEE 1

    uint8 byte0 = packet_type::kPrimaryAddressAccessory_Lo; // 128
    uint8 byte1 = 0b11110000U;
    uint8 checksum = byte0 ^ byte1;

    const uint8 bytes[] = {
        byte0, // primary address = 128
        byte1, // data byte
        checksum  // checksum byte (dummy value, will be replaced)
    };
    packet_type packet(bytes, sizeof(bytes) / sizeof(bytes[0]));

    // By default, the filter does not let any packet pass.
    EXPECT_EQ(filter.do_filter(packet), false);

    filter.setLo(packet_type::kPrimaryAddressAccessory_Lo);
    filter.setHi(packet_type::kPrimaryAddressAccessory_Lo);

    // With kPrimaryAddressAccessory_Lo, the packet should pass the filter.
    EXPECT_EQ(filter.do_filter(packet), true);
}

// -----------------------------------------------------------------------
/// @brief Test if a pass filter for primary addresses is working:
/// uninitialized and initialized.
// -----------------------------------------------------------------------
TEST(Ut_Filter, filter_PassAddressFilter_1)
{
    using packet_type = dcc::packet<6>;
    using filter_type = dcc::pass_address_filter<packet_type>;

    constexpr uint8 cv29 = dcc::cfg::kBitMask_Cv29_OutputAddressMethod; // Use output address method for Basic Accessory
    filter_type filter;
    uint16 address = 1; // Use address 1 for 11 bit output address (1-2047)

    // BasicAccessory
    // {preamble} 0 10AAAAAA 0 1AAACDDD 0 EEEEEEEE 1
    // The most significant bits of the 9 - bit address are bits 4 - 6 of the
    // second data byte. By convention these bits(bits 4 - 6 of the second data byte)
    // are in ones complement.
    const uint8 byte0 = 0b10000000U | static_cast<uint8>(((address & 0x003FU) >> 0U));
    const uint8 byte1 = 0b10000000U | static_cast<uint8>(((~address & 0x01C0U) >> 2U));
    const uint8 checksum = byte0 ^ byte1;
    const uint8 bytes[] = {
        byte0, // first data byte
        byte1, // second data byte
        checksum  // checksum byte (dummy value, will be replaced)
    };
    packet_type packet(bytes, sizeof(bytes) / sizeof(bytes[0]));

    filter.set_cv29(cv29);

    // By default, the filter does not let any packet pass.
    EXPECT_EQ(filter.do_filter(packet), false);

    filter.set_lo(address);
    filter.set_hi(address);

    // With address, the packet should pass the filter.
    EXPECT_EQ(filter.do_filter(packet), true);
}

/**
 * @brief Intended to be called before each test.
 */
void setUp(void)
{
}

/**
 * @brief Intended to be called after each test.
 */
void tearDown(void)
{
}

/**
 * @brief Test setup function, called once at the beginning as part of Arduino's setup().
 */
void test_setup(void)
{
}

/**
 * @brief Test setup function, called once at the beginning as part of Arduino's setup().
 *
 * @return bool false to stop execution (relevant on Windows)
 */
bool test_loop(void)
{
    UNITY_BEGIN();

    RUN_TEST(filter_PassPrimaryAddressFilter_1);
    RUN_TEST(filter_PassAddressFilter_1);

    (void)UNITY_END();

    // Return false to stop program execution (relevant on Windows)
    return false;
}
