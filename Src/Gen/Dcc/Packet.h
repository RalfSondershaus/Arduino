/**
  * @file Packet.h
  *
  * @author Ralf Sondershaus
  *
  * Defines class Packet in namespace dcc.
  *
  * @copyright Copyright 2018 - 2022 Ralf Sondershaus
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

#ifndef DCC_PACKET_H
#define DCC_PACKET_H

#include <Std_Types.h>
#include <Util/Math.h>
#include <Util/Array.h>
#include <Util/bitset.h>

namespace dcc
{
  // -----------------------------------------------------
  /// A packet has 0 ... kMaxBytes bytes.
  ///
  /// @tparam kMaxBytes Maximal number of bytes in a packet
  // -----------------------------------------------------
  template<int kMaxBytes = 6>
  class Packet
  {
  public:
    /// We are using 16 bit to store addresses.
    using address_type = uint16;

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

  protected:
    /// array of bytes
    typedef util::array<uint8, kMaxBytes> ByteArray;

    /// number of bits received
    uint8_least ucNrNbits;
    /// byte array
    ByteArray bytes;

    /// Return current byte index (index into array of bytes)
    uint16 byteIdx() const noexcept { return static_cast<uint16>(ucNrNbits / 8u); }
    /// Return current bit index (in current byte byteIdx())
    uint16 bitIdx() const noexcept { return static_cast<uint16>(ucNrNbits % 8u); }

    typedef struct
    {
      address_type address;
    } decoded_packet_type;

    uint8 type; ///< Any of packet_type such as BasicAccessory. Is Init until decode() is called.
    decoded_packet_type data; ///< Decoded packet data. Has default / invalid values until decode() is called.

  public:
    typedef size_t size_type;

    using iterator = typename ByteArray::iterator;
    using const_iterator = typename ByteArray::const_iterator;

    typedef enum { Init, Idle, MultiFunctionBroadcast, MultiFunction7, MultiFunction14, BasicAccessory, ExtendedAccessory, Unknown, Invalid } packet_type;

    /// Maximal number of bytes (template parameter)
    static constexpr int kMaxNrBytes = kMaxBytes;

    /// number of "1" in the preamble (for debugging)
    uint8 ucNrOnePreamble;

    /// Constructor
    Packet() { clear(); }
    /// clear all data
    void clear()
    {
      ucNrNbits = 0;
      bytes.fill(0);
      type = packet_type::Init;
      data.address = 0;
    }
    /// add a bit (0 or 1)
    void addBit(uint8 uc_bit)
    {
      refByte(byteIdx()) = static_cast<uint8>((refByte(byteIdx()) << 1u) | uc_bit);
      ucNrNbits++;
    }

    /// Returns a reference to the byte at specified location idx. No bounds checking is performed.
    uint8& refByte(size_type idx) { return bytes[idx]; }
    const uint8& refByte(size_type idx) const { return bytes[idx]; }

    /// Returns a reference to the element at specified location idx. No bounds checking is performed.
    uint8& operator[](size_type idx) { return refByte(idx); }
    const uint8& operator[](size_type idx) const { return refByte(idx); }

    /// get the number of used bytes
    size_type getNrBytes() const noexcept { return util::math::ceilt(ucNrNbits, static_cast<uint8_least>(8u)); }

    /// equality
    bool operator==(const Packet& p) const
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
    /// Byte Three: Error Detection Data Byte = EEEEEEEE The error detection data byte is a data byte used to detect the
    /// presence of transmission errors.The contents of the Error Detection Data Byte shall be the bitwise
    /// exclusive OR of the contents of the Address Data Byte and the Instruction Data Byte in the packet
    /// concerned. (e.g.the exclusive OR of bit 0 of the address data byte and bit 0 of the instruction data byte
    /// will be placed in bit 0 of the error detection data byte...) Digital Decoders receiving a Baseline Packet
    /// shall compare the received error detection data byte with the bitwise exclusive OR of the received address
    /// and instruction data bytes and ignore the contents of the packet if this comparison is not identical
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

    /// @brief For Basic Accessory Decoders, get the output address from DCC decoder address
    ///        and two address bits in DDD.
    /// Currently, we are using the so-called linear address method as defined in RCN-213.
    /// In contrast, the non-linear address method is not supported.
    /// Both address methods differ in the wrap around at AAAAAA AAA AA address 255 -> 256
    /// (511 -> 512, 767 -> 768, 1023 -> 1024, etc.). For details, see RCN-213.
    /// @param addr DCC decoder address
    /// @param dd bits 1 and 2 in DDD (which has bits 0, 1, 2)
    /// @return Output address 0 ... 2023
    static address_type convertToOutputAddress(address_type addr, address_type dd) noexcept
    {
        // {preamble} 0 10AAAAAA 0 1AAACDDD 0 EEEEEEEE 1
        // The most significant bits of the 9 - bit address are bits 4 - 6 of the
        // second data byte. By convention these bits(bits 4 - 6 of the second data byte)
        // are in ones complement.
        // Addresses are often extended with the 2 most significant bits of DDD
        // so that the 9 bit address is extended to a 11 bit address
        // (Accessory Output Address Method).
        // For example, Fleischmann Profi Boss, magnetic devices.
        // AAA AAAAAA = 1 and DD D = 00 0 means address 1
        // AAA AAAAAA = 1 and DD D = 01 0 means address 2
        // AAA AAAAAA = 1 and DD D = 10 0 means address 3
        // AAA AAAAAA = 1 and DD D = 11 0 means address 4
        // AAA AAAAAA = 2 and DD D = 00 0 means address 5
        // ...
        // AAA AAAAAA = 3 and DD D = 00 0 means address 9
        // etc
        // {preamble}  0  10AAAAAA  0  1AAACAAD  0  EEEEEEEE  1
        //
        // Remark: RCN:213 uses D instead of C and R instead of D :-)
        // So, with RCN-213:
        // {preamble}  0  10AAAAAA  0  1AAADAAR  0  EEEEEEEE  1
        // RCN-213: D is used to activate or deactivate the addressed device

        // Two possible ways to calculate:
        // addr--; addr = addr << 2 | baGetDAddr(); addr++;
        //         addr = addr << 2 | baGetDAddr(); addr -= 3;
        addr = static_cast<address_type>((addr << 2U) | dd);
        addr -= 3;

        return addr;

    }

    /// Precondition: getNrBytes() > 2
    packet_type decodeMultiFunctionOrAccessory()
    {
      if (refByte(0) == kPrimaryAddressBroadCast)
      {
        type = MultiFunctionBroadcast;
        data.address = 0U;
      }
      else if (refByte(0) <= kPrimaryAddressMultiFunction7_Hi)
      {
        // {preamble} 0 [ 0AAAAAAA ] 0 {instruction-bytes} 0 EEEEEEEE 1
        type = MultiFunction7;
        data.address = static_cast<address_type>(refByte(0));
      }
      else if (refByte(0) <= kPrimaryAddressAccessory_Hi)
      {
        if (util::bits::test(refByte(1), 7))
        {
          // {preamble} 0 10AAAAAA 0 1AAACDDD 0 EEEEEEEE 1
          type = BasicAccessory;
          // this address type is known as "decoder address method"
          data.address = static_cast<address_type>(
            (util::bits::apply_mask_as<uint8_t, address_type>( refByte(0), 0b00111111U)) |
            (util::bits::apply_mask_as<uint8_t, address_type>(~refByte(1), 0b01110000U) << 2U)
            );
        }
        else
        {
          // {preamble} 0 10AAAAAA 0 0AAA0AA1 0 000XXXXX 0 EEEEEEEE 1
          type = ExtendedAccessory;
          data.address = static_cast<uint16>(
            (util::bits::apply_mask_as<uint8, uint16>(refByte(0), 0b00111111U)) |
            (util::bits::apply_mask_as<uint8, uint16>(refByte(1), 0b01110000U) << 4U) | 
            (util::bits::apply_mask_as<uint8, uint16>(refByte(1), 0b00000110U) << 5U)
            );
        }
      }
      else if (refByte(0) <= kPrimaryAddressMultiFunction14_Hi)
      {
        // {preamble} 0 [ 11AAAAAA ] 0 {instruction-bytes} 0 EEEEEEEE 1 
        // The first address byte contains 8 bits of address information.
        // If the most significant bits of the address are "11"and the remaining bits are not 111111,
        // then a second address byte must immediately follow.
        // This second address byte will then contain an additional 8 bits of address data.
        // The most significant bit of two byte addresses is bit 5 of the first address byte. (bits #6 and 
        // #7 having the value of "1" in this case. 
        type = MultiFunction14;
        data.address = static_cast<uint16>(
          (util::bits::apply_mask_as<uint8, uint16>(refByte(0), 0b00111111U) << 8U) |
          (refByte(1))
            );
      }
      else if (refByte(0) <= kPrimaryAddressReserved_Hi)
      {
        type = Unknown;
        data.address = static_cast<uint16>(refByte(0));
      }
      else
      {
        type = Idle;
        data.address = 255;
      }

      return static_cast<packet_type>(type);
    }

    /// The first data byte of an Extended Packet Format packet contains the primary address.
    /// 00000000              0         Broadcast address
    /// 00000001-01111111     1 - 127   Multi-Function decoders with 7 bit addresses (locomotives)
    /// 10000000-10111111   128 - 191   Basic Accessory Decoders with 9 bit addresses and Extended Accessory Decoders with 11-bit addresses
    /// 11000000-11100111   192 - 231   Multi Function Decoders with 14 bit addresses (locomotives)
    /// 11101000-11111110   232 - 254   Reserved for Future Use
    /// 11111111            255         Idle Packet
    /// The first address byte contains 8 bits of address information. If the most significant bits of the address are "11"and 
    /// the remaining bits are not �111111�, then a second address byte must immediately follow.
    /// This second address byte will then contain an additional 8 bits of address data.
    /// The most significant bit of two byte addresses is bit 5 of the first address byte. (bits #6 and #7 having the value of "1" in this case.
    packet_type decode()
    {
      type = Unknown;

      if (getNrBytes() > 0)
      {
        // first, check for idle packet which can have less than three bytes
        if (refByte(0) == kPrimaryAddressIdle)
        {
          type = Idle;
          data.address = 255;
        }
        // continue with non-idle packets. These packets must have three bytes at least.
        else if (getNrBytes() > 2)
        {
          if (testChecksum())
          {
            type = static_cast<uint8>(decodeMultiFunctionOrAccessory());
          }
          else
          {
            // invalid checksum
            type = Invalid;
          }
        }
        else
        {
          // invalid number of bytes
          type = Invalid;
        }
      }
      else
      {
        // invalid number of bytes
        type = Invalid;
      }

      return static_cast<packet_type>(type);
    }

    /// Returns the type of this packet. Returns Init until decode() is called.
    packet_type getType() const noexcept { return static_cast<packet_type>(type); }

    /// The first data byte of an Extended Packet Format packet contains the primary address.
    /// 00000000              0         Broadcast address
    /// 00000001-01111111     1 - 127   Multi-Function decoders with 7 bit addresses (locomotives)
    /// 10000000-10111111   128 - 191   Basic Accessory Decoders with 9 bit addresses and Extended Accessory Decoders with 11-bit addresses
    /// 11000000-11100111   192 - 231   Multi Function Decoders with 14 bit addresses (locomotives)
    /// 11101000-11111110   232 - 254   Reserved for Future Use
    /// 11111111            255         Idle Packet
    uint8 getPrimaryAddress() const noexcept { return refByte(0); }

    /// Returns the decoder address. Returns 0 until decode() is called.
    uint16 getAddress() const noexcept { return data.address; }

    /// In accordance with the DCC standard, each DCC accessory decoder address corresponds to exactly 4 turnout numbers.
    /// Returns the output address of an output such as a turnout or signal.
    address_type getOutputAddress() const noexcept { return convertToOutputAddress(getAddress(), baGetDAddr()); }

    /// Extended accessory: returns the aspect
    /// {preamble} 0 10AAAAAA 0 0AAA0AA1 0 000XXXXX 0 EEEEEEEE 1
    uint8 eaGetAspect() const { return static_cast<uint8>(refByte(2) & 0b00011111U); }

    /// Basic accessory: returns C
    /// {preamble} 0 10AAAAAA 0 1AAACDDD 0 EEEEEEEE 1
    /// C is typically not used but has the constant value 1.
    uint8 baGetC() const { return static_cast<uint8>((refByte(1) & 0b00001000U) >> 3U); }
    /// Basic accessory: returns DDD
    /// {preamble} 0 10AAAAAA 0 1AAACDDD 0 EEEEEEEE 1
    uint8 baGetD() const       { return static_cast<uint8>( refByte(1) & 0b00000111U); }
    uint8 baGetDAddr() const   { return static_cast<uint8>((refByte(1) & 0b00000110U) >> 1U); }
    uint8 baGetDSwitch() const { return static_cast<uint8>( refByte(1) & 0b00000001U); }
  };
  
} // namespace dcc

#endif // DCC_PACKET_H