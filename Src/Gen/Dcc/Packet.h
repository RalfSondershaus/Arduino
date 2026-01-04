/**
 * @file Packet.h
 * @brief Defines the dcc::Packet class template for handling DCC (Digital Command Control) packets.
 *
 * @details
 * The Packet class template represents a DCC packet, which consists of up to kMaxBytes bytes.
 * It provides methods for constructing, manipulating, and decoding DCC packets according to the NMRA DCC standards.
 * The class supports various packet types, including Multi-Function, Basic Accessory, Extended Accessory, and Idle packets.
 * It also provides utilities for checksum validation, address extraction, and output address calculation.
 *
 * @tparam kMaxBytes The maximum number of bytes in a packet (default: 6).
 *
 * @author Ralf Sondershaus
 * @date 2018-2023
 *
 * @copyright Copyright 2018 - 2022 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DCC_PACKET_H
#define DCC_PACKET_H

#include <Std_Types.h>
#include <Util/Math.h>
#include <Util/Array.h>
#include <Util/bitset.h>

namespace dcc
{
    namespace cfg
    {
        /**
         * @brief Bit mask for CV29 bit 6: Output Address Method
         */
        constexpr uint8 kBitMask_Cv29_OutputAddressMethod = 0b01000000;
    }

    /**
     * @brief Represents a DCC (Digital Command Control) packet with up to kMaxBytes bytes.
     *
     * This class provides storage and manipulation for DCC packets, including bit/byte access,
     * address decoding, checksum validation, and support for various DCC packet types such as
     * Multi-Function, Basic Accessory, and Extended Accessory packets.
     *
     * @tparam kMaxBytes The maximal number of bytes in a packet (default: 6).
     *
     * The Packet class supports:
     * - Bitwise construction of packets.
     * - Decoding of DCC packet types and addresses according to NMRA DCC standards.
     * - Checksum calculation and validation.
     * - Access to primary and decoder addresses, as well as accessory output addresses.
     * - Iteration over the underlying byte array.
     *
     * For convenience, the class defines access functions for Basic Accessory decoders according to 
     * [S-9.2.1 2012]: 
     * 
     * {preamble}  0  10AAAAAA  0  1ĀĀĀCDDD  0  EEEEEEEE  1 
     * 
     * With [S-9.2.1 2025], bit names of Basic Accessory decoders have changed.
     * 
     * {preamble}  0  10AAAAAA  0  1ĀĀĀDAAR12  0  EEEEEEEE  1
     * 
     * Usage:
     * - Add bits to the packet using addBit().
     * - Decode the packet type and address using decode().
     * - Access packet decoded_data and type using provided getters.
     *
     * @note This class assumes the existence of utility types and functions in the util namespace,
     *       such as util::array, util::math, and util::bits.
     * 
     * @see [S-9.2.1 2012] NMRA Standard DCC Extended Packet Formats, Jul 12, 2012, www.nmra.org
     * @see [S-9.2.1 2025] NMRA Standard DCC Extended Packet Formats, Jan 24, 2025, www.nmra.org
     */
    template <int kMaxBytes = 6>
    class packet
    {
    public:
        /// We are using 16 bit to store addresses.
        using address_type = uint16;
        using size_type = size_t;

        /**
         * @brief Packet types for DCC packets.
         */
        enum class packet_type : uint8
        {
            Init,                   /**< Initial state, no valid packet */
            Idle,                   /**< Idle packet */
            MultiFunctionBroadcast, /**< MultiFunction Broadcast packet */
            MultiFunction7,         /**< MultiFunction packet with 7 bit address */
            MultiFunction14,        /**< MultiFunction packet with 14 bit address */
            BasicAccessory,         /**< Basic Accessory packet */
            ExtendedAccessory,      /**< Extended Accessory packet */
            Unknown,                /**< Unknown packet type */
            Invalid                 /**< Invalid packet type */
        };

        /// Maximal number of bytes (template parameter)
        static constexpr int kMaxNrBytes = kMaxBytes;

        /// Ranges of primary addresses
        /// 00000000              0         Broadcast address
        /// 00000001-01111111     1 - 127   Multi-Function decoders with 7 bit addresses (locomotives)
        /// 10000000-10111111   128 - 191   Basic Accessory Decoders with 9 bit addresses and Extended Accessory Decoders with 11-bit addresses
        /// 11000000-11100111   192 - 231   Multi Function Decoders with 14 bit addresses (locomotives)
        /// 11101000-11111110   232 - 254   Reserved for Future Use
        /// 11111111            255         Idle Packet
        static constexpr uint8 kPrimaryAddressBroadCast = 0;
        static constexpr uint8 kPrimaryAddressMultiFunction7_Lo = 1;
        static constexpr uint8 kPrimaryAddressMultiFunction7_Hi = 127;
        static constexpr uint8 kPrimaryAddressAccessory_Lo = 128;
        static constexpr uint8 kPrimaryAddressAccessory_Hi = 191;
        static constexpr uint8 kPrimaryAddressMultiFunction14_Lo = 192;
        static constexpr uint8 kPrimaryAddressMultiFunction14_Hi = 231;
        static constexpr uint8 kPrimaryAddressReserved_Lo = 232;
        static constexpr uint8 kPrimaryAddressReserved_Hi = 254;
        static constexpr uint8 kPrimaryAddressIdle = 255;
        static constexpr uint16 kInvalidAddress = 0xFFFF; ///< Invalid address

    protected:
        /// array of bytes
        using byte_array_type = util::array<uint8, kMaxBytes>;
        using iterator = typename byte_array_type::iterator;
        using const_iterator = typename byte_array_type::const_iterator;

        /// number of bits received
        uint8_least ucNrNbits;
        /// byte array
        byte_array_type bytes;

        /// Return current byte index (index into array of bytes)
        uint16 byteIdx() const noexcept { return static_cast<uint16>(ucNrNbits / 8u); }
        /// Return current bit index (in current byte byteIdx())
        uint16 bitIdx() const noexcept { return static_cast<uint16>(ucNrNbits % 8u); }

        /**
         * @brief Decoded packet data.
         */
        struct
        {
            /**
             * This field contains the address of the packet. The address can be:
             * - Multi-Function 7 bit address: 1-127
             * - Multi-Function 14 bit address:  (192-231) + 8 bit secondary address
             * - Basic Accessory decoder address (9 bit): 1-511
             * - Basic Accessory output address (11 bit): 1 - 2048
             * - Extended Accessory decoder address (11 bit): 1 - 2048
             * - Broadcast address: 0
             * - Idle packet: 255
             *
             * Accessory decoders:
             * CV29, bit 6 (output address mode) = 0: 9 bit decoder address (1-511)
             * CV29, bit 6 (output address mode) = 1: 11 bit output address (1-2047)
             */
            address_type address;
            /**
             * @brief Any of packet_type such as BasicAccessory. Init after construction.
             * 
             */
            packet_type type;
        } decoded_data;

        /**
         * @brief Decode the packet type.
         * 
         * The first byte of a packet contains the primary address.
         * 
         * | Bit Range             | Address Range | Description |
         * |-----------------------|---------------|-------------|
         * | 00000000              | 0             | Broadcast address |
         * | 00000001-01111111     | 1 - 127       | Multi-Function decoders with 7 bit addresses (locomotives) |
         * | 10000000-10111111     | 128 - 191     | Basic and Extended Accessory Decoders with 9 bit and 11 bit addresses, resp. |
         * | 11000000-11100111     | 192 - 231     | Multi Function Decoders with 14 bit addresses (locomotives) |
         * | 11101000-11111110     | 232 - 254     | Reserved for Future Use |
         * | 11111111              | 255           | Idle Packet |
         *
         * @return packet_type The decoded packet type.
         */
        packet_type decode_type()
        {
            packet_type type = packet_type::Unknown;

            if (getNrBytes() > 0)
            {
                // first, check for idle packet which can have less than three bytes
                if (refByte(0) == kPrimaryAddressIdle)
                {
                    type = packet_type::Idle;
                }
                // continue with non-idle packets. These packets must have three bytes at least.
                else if (getNrBytes() > 2)
                {
                    if (testChecksum())
                    {
                        type = decodeMultiFunctionOrAccessory();
                    }
                    else
                    {
                        // invalid checksum
                        type = packet_type::Invalid;
                    }
                }
                else
                {
                    // invalid number of bytes
                    type = packet_type::Invalid;
                }
            }
            else
            {
                // invalid number of bytes
                type = packet_type::Invalid;
            }

            return type;
        }

        /**
         * @brief Decode the packet type.
         * 
         * @param cv29_address_method The value of CV29 from the decoder's configuration.
         * 
         * @note Precondition: getNrBytes() must be greater than 2
         */
        packet_type decodeMultiFunctionOrAccessory()
        {
            if (refByte(0) == kPrimaryAddressBroadCast)
            {
                decoded_data.type = packet_type::MultiFunctionBroadcast;
            }
            else if (refByte(0) <= kPrimaryAddressMultiFunction7_Hi)
            {
                // {preamble} 0 [ 0AAAAAAA ] 0 {instruction-bytes} 0 EEEEEEEE 1
                decoded_data.type = packet_type::MultiFunction7;
            }
            else if (refByte(0) <= kPrimaryAddressAccessory_Hi)
            {
                if (util::bits::test(refByte(1), 7))
                {
                    // {preamble} 0 10AAAAAA 0 1ĀĀĀCDDD 0 EEEEEEEE 1 [S-9.2.1 2012]
                    // {preamble} 0 10AAAAAA 0 1ĀĀĀDAAR 0 EEEEEEEE 1 [S-9.2.1 2025]
                    decoded_data.type = packet_type::BasicAccessory;
                }
                else
                {
                    // {preamble} 0 10AAAAAA 0 0AAA0AA1 0 000XXXXX 0 EEEEEEEE 1
                    decoded_data.type = packet_type::ExtendedAccessory;
                }
            }
            else if (refByte(0) <= kPrimaryAddressMultiFunction14_Hi)
            {
                // {preamble} 0 [ 11AAAAAA ] 0 {instruction-bytes} 0 EEEEEEEE 1
                decoded_data.type = packet_type::MultiFunction14;
            }
            else if (refByte(0) <= kPrimaryAddressReserved_Hi)
            {
                decoded_data.type = packet_type::Unknown;
            }
            else
            {
                decoded_data.type = packet_type::Idle;
            }

            return decoded_data.type;
        }

        /**
         * @brief Returns the decoded address.
         * 
         * The address can be:
         * - Multi-Function 7 bit address (1-127)  
         * - Multi-Function 14 bit address
         * - Basic Accessory decoder address (9 bit)
         * - Basic Accessory output address (11 bit): 1 - 2048
         * - Extended Accessory decoder address (11 bit)
         * - Broadcast address (0)
         */
        uint16 decode_address(uint8 cv29) noexcept 
        { 
            address_type address;
            // Address is not valid yet. Try to decode it.
            switch (get_type())
            {
                case packet_type::MultiFunction14:
                    // {preamble} 0 [ 11AAAAAA ] 0 {instruction-bytes} 0 EEEEEEEE 1
                    // The first address byte contains 8 bits of address information.
                    // If the most significant bits of the address are "11"and the remaining bits 
                    // are not 111111, then a second address byte must immediately follow.
                    // This second address byte will then contain an additional 8 bits of address 
                    // decoded_data. The most significant bit of two byte addresses is bit 5 of the 
                    // first address byte. Bits #6 and #7 having the value of "1" in this case.
                    address = static_cast<address_type>(
                        (util::bits::apply_mask_as<uint8, address_type>(refByte(0), 0b00111111U) << 8U) |
                        (refByte(1)));
                    break;
                case packet_type::MultiFunction7:
                    // {preamble} 0 [ 0AAAAAAA ] 0 {instruction-bytes} 0 EEEEEEEE 1
                    address = static_cast<address_type>(refByte(0));
                    break;
                case packet_type::MultiFunctionBroadcast:
                    address = 0;
                    break;
                case packet_type::Idle:
                    address = 255;
                    break;
                case packet_type::BasicAccessory:
                    // {preamble} 0 10AAAAAA 0 1ĀĀĀCDDD 0 EEEEEEEE 1 [S-9.2.1 2012]
                    // {preamble} 0 10AAAAAA 0 1ĀĀĀDAAR 0 EEEEEEEE 1 [S-9.2.1 2025]
                    // Decoder address method
                    address = static_cast<address_type>(
                        (util::bits::apply_mask_as<uint8, address_type>(refByte(0), 0b00111111U)) |
                        (util::bits::apply_mask_as<uint8, address_type>(~refByte(1), 0b01110000U) << 2U));
                    if (cv29 & cfg::kBitMask_Cv29_OutputAddressMethod)
                    {
                        // Output address method
                        // Two possible ways to calculate:
                        // addr--; addr = addr << 2 | output_pair; addr++;
                        //         addr = addr << 2 | output_pair; addr -= 3;
                        address = static_cast<address_type>(
                            (address << 2U) | 
                            (util::bits::apply_mask_as<uint8, address_type>(refByte(1), 0b00000110U) >> 1U));
                        if (address > 3)
                        {
                            address -= 3; // 1 .. 2044
                        }
                        else
                        {
                            address += 2045; // 2045 .. 2048
                        }
                    }
                    break;
                case packet_type::ExtendedAccessory:
                    // {preamble} 0 10AAAAAA 0 0AAA0AA1 0 000XXXXX 0 EEEEEEEE 1
                    address = static_cast<uint16>(
                        (util::bits::apply_mask_as<uint8, uint16>(refByte(0), 0b00111111U)) |
                        (util::bits::apply_mask_as<uint8, uint16>(refByte(1), 0b01110000U) << 4U) |
                        (util::bits::apply_mask_as<uint8, uint16>(refByte(1), 0b00000110U) << 5U));
                    break;
                case packet_type::Init:
                case packet_type::Unknown:
                case packet_type::Invalid:
                default:
                    address = kInvalidAddress;    
                    break;
            }

            return address; 
        }


    public:
        /// number of "1" in the preamble
        uint8 preamble_one_count;

        /// Constructor
        packet() { clear(); }
        /**
         * @brief Construct a new packet object from raw data.
         * 
         * Convenience constructor to create a packet from raw byte data.
         * The bits are added in MSB-first order.
         * 
         * @param data Pointer to the raw byte data.
         * @param num_bytes Number of bytes in the data.
         */
        packet(const uint8 *data, uint8_least num_bytes)
            : ucNrNbits{0}, bytes{}, decoded_data{ kInvalidAddress, packet_type::Init }, preamble_one_count{0}
        {
            for (uint8_least i = 0; i < num_bytes; i++)
            {
                for (sint8_fast bit = 7; bit >= 0; bit--)
                {
                    addBit((data[i] >> bit) & 0x01U);
                }
            }
        }
        /// clear all decoded_data
        void clear()
        {
            ucNrNbits = 0;
            bytes.fill(0);
            decoded_data.type = packet_type::Init;
            decoded_data.address = kInvalidAddress;
            preamble_one_count = 0;
        }
        /// add a bit (0 or 1)
        void addBit(uint8 uc_bit)
        {
            refByte(byteIdx()) = static_cast<uint8>((refByte(byteIdx()) << 1u) | uc_bit);
            ucNrNbits++;
        }

        /// Returns a reference to the byte at specified location idx. No bounds checking is performed.
        uint8 &refByte(size_type idx) { return bytes[idx]; }
        const uint8 &refByte(size_type idx) const { return bytes[idx]; }

        /// Returns a reference to the element at specified location idx. No bounds checking is performed.
        uint8 &operator[](size_type idx) { return refByte(idx); }
        const uint8 &operator[](size_type idx) const { return refByte(idx); }

        /// get the number of used bytes
        size_type getNrBytes() const noexcept { return util::math::ceilt(ucNrNbits, static_cast<uint8_least>(8u)); }

        /// equality
        bool operator==(const packet &p) const
        {
            uint16_t i;

            bool b_ret = (p.ucNrNbits == ucNrNbits);

            for (i = 0; i < byteIdx(); i++)
            {
                b_ret = b_ret && (p.refByte(i) == refByte(i));
            }

            return b_ret;
        }

        /// returns an iterator to the first byte
        iterator begin() { return bytes.begin(); }
        const_iterator begin() const { return bytes.begin(); }

        /// return an iterator to one beyond the last used byte
        iterator end_used() { return bytes.begin() + getNrBytes(); }
        const_iterator end_used() const { return bytes.begin() + getNrBytes(); }

        /// returns an iterator to the end
        iterator end() { return bytes.end(); }
        const_iterator end() const { return bytes.end(); }

        /// S-9.2 This standard covers the format of the information sent via Digital Command Stations to Digital Decoders.
        ///
        /// Byte Three: Error Detection Data Byte = EEEEEEEE The error detection decoded_data byte is a decoded_data byte used to detect the
        /// presence of transmission errors.The contents of the Error Detection Data Byte shall be the bitwise
        /// exclusive OR of the contents of the Address Data Byte and the Instruction Data Byte in the packet
        /// concerned. (e.g.the exclusive OR of bit 0 of the address decoded_data byte and bit 0 of the instruction decoded_data byte
        /// will be placed in bit 0 of the error detection decoded_data byte...) Digital Decoders receiving a Baseline Packet
        /// shall compare the received error detection decoded_data byte with the bitwise exclusive OR of the received address
        /// and instruction decoded_data bytes and ignore the contents of the packet if this comparison is not identical
        ///
        /// Precondition: getNrBytes() > 0
        /// Precondition: checksum is in last used byte
        ///
        /// @return true if the checksum test succeeded, false otherwise.
        bool testChecksum()
        {
            auto b = begin();
            const auto e = end_used() - 1;
            uint8 tmp;

            // start with first byte
            tmp = *b;
            b++;
            while (b < e)
            {
                // exclusive OR with second byte,third byte, etc
                tmp ^= *b;
                b++;
            }
            // b now points to the last used byte which is the checksum byte
            return (tmp == *b);
        }

        /// Returns the type of this packet. Returns Init until decode() is called.
        packet_type get_type() noexcept 
        { 
            if (decoded_data.type == packet_type::Init)
            {
                // Type is not valid yet. Try to decode it.
                decoded_data.type = decode_type();
            }
            return decoded_data.type; 
        }

        /// The first decoded_data byte of an Extended Packet Format packet contains the primary address.
        /// 00000000              0         Broadcast address
        /// 00000001-01111111     1 - 127   Multi-Function decoders with 7 bit addresses (locomotives)
        /// 10000000-10111111   128 - 191   Basic Accessory Decoders with 9 bit addresses and Extended Accessory Decoders with 11-bit addresses
        /// 11000000-11100111   192 - 231   Multi Function Decoders with 14 bit addresses (locomotives)
        /// 11101000-11111110   232 - 254   Reserved for Future Use
        /// 11111111            255         Idle Packet
        uint8 get_primary_address() const noexcept { return refByte(0); }

        /**
         * @brief Returns the decoded address. Returns 0 until decode() is called.
         * 
         * Depending on the primary address (which defines the packet type to be Multi-Function, 
         * Basic Accessory, or Extended Accessory), the address can be:
         * - Multi-Function 7 bit address (1-127)  
         * - Multi-Function 14 bit address
         * - Basic Accessory decoder address (9 bit) - CV29, bit 6 (output address mode) = 0
         * - Basic Accessory output address (11 bit) - CV29, bit 6 (output address mode) = 1
         * - Extended Accessory decoder address (11 bit)
         * - Broadcast address (0)
         * 
         * @param cv29 The value of CV29 from the decoder's configuration.
         * 
         * @return The decoded address or kInvalidAddress if the address is not valid.
         * 
         * @see @ref cfg::kBitMask_Cv29_OutputAddressMethod for CV29 bit 6: Output Address Method
         */
        address_type get_address(uint8 cv29) noexcept 
        { 
            if (decoded_data.address == kInvalidAddress)
            {
                // Address is not valid yet. Try to decode it.
                decoded_data.address = decode_address(cv29);
            }

            return decoded_data.address; 
        }

        /**
         * @defgroup Get functions for Basic Accessory Decoders
         * @{
         */
        /**
         * @brief Returns the aspect from an Extended Accessory packet.
         * 
         * Extended accessory: returns the aspect XXXXX
         * {preamble} 0 10AAAAAA 0 0AAA0AA1 0 000XXXXX 0 EEEEEEEE 1
         * 
         * @return uint8 [0-31]: the aspect.
         */
        uint8 ea_get_aspect() const { return static_cast<uint8>(refByte(2) & 0b00011111U); }
        /**
         * @}
         */

        /**
         * @defgroup Get functions for Basic Accessory Decoders
         * @{
         */
        /**
         * @brief Returns the output power (corresponds to bit 3 in the second byte, the `D` bit 
         * [S-9.2.1 2025] or the `C` bit [S-9.2.1 2012]).
         * 
         * Bit 3 of the second byte `D` is used to activate or deactivate the addressed device.  
         * 
         * @note If the duration the device intended to be on is less than or equal the set 
         * duration, no deactivation is necessary.
         * 
         * [S-9.2.1 2012]: `{preamble} 0 10AAAAAA 0 1AAACDDD 0 EEEEEEEE 1`
         * 
         * [S-9.2.1 2025]: `{preamble} 0 10AAAAAA 0 1AAADAAR 0 EEEEEEEE 1`
         * 
         * @return uint8 0 or 1: bit 3 of the second byte.
         * 
         * @note This bit is typically not used but has the constant value 1.
         */
        uint8 ba_get_output_power() const { return static_cast<uint8>((refByte(1) & 0b00001000U) >> 3U); }
        /**
         * @brief Returns the lowest three bits of the second byte.
         *
         * Convenience function for Basic Accessory decoders according to [S-9.2.1 2012]:
         * `{preamble} 0 10AAAAAA 0 1AAACDDD 0 EEEEEEEE 1`
         * 
         * @note [S-9.2.1 2025] changed the bit names of Basic Accessory decoders and does not use `DDD` 
         * anymore: `{preamble} 0 10AAAAAA 0 1AAADAAR 0 EEEEEEEE 1`
         *
         * @return uint8 [0-7]: the bits `DDD`.
         */
        uint8 ba_get_DDD() const { return static_cast<uint8>(refByte(1) & 0b00000111U); }
        /**
         * @brief Returns the bits 1 and 2 of the second byte.
         *
         * Convenience function for Basic Accessory decoders with **decoder address method** 
         * (CV29, bit 6 = 0).
         * 
         * With **decoder address method** (CV29, bit 6 = 0), bits 1 and 2 are the output pair.
         * Each decoder can control four output pairs. Each output pair consists of two outputs.
         * The output pair is selected by the two bits `DD` as follows:
         * - `00`: output pair 1
         * - `01`: output pair 2
         * - `10`: output pair 3
         * - `11`: output pair 4
         * 
         * @return uint8 [0-3]: the output pair.
         */
        uint8 ba_get_output_pair() const { return static_cast<uint8>((refByte(1) & 0b00000110U) >> 1U); }
        /**
         * @brief Returns the lowest bit of the second byte which is the output direction.
         * 
         * By convention, output direction = 0 means diverging, direction of travel to the left, 
         * or signal to stop and output direction = 1 means normal, direction of travel to the 
         * right, or signal to proceed.
         * 
         * - [S-9.2.1 2012]: `{preamble} 0 10AAAAAA 0 1AAACDDD 0 EEEEEEEE 1`
         * - [S-9.2.1 2025]: `{preamble} 0 10AAAAAA 0 1AAADAAR 0 EEEEEEEE 1`
         * 
         * @return uint8 [0 or 1]: the output direction.
         */
        uint8 ba_get_output_direction() const { return static_cast<uint8>(refByte(1) & 0b00000001U); }
        /**
         * @}
         */
    };

} // namespace dcc

#endif // DCC_PACKET_H