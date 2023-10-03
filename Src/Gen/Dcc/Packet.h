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

  public:
    typedef size_t size_type;

    using iterator = typename ByteArray::iterator;
    using const_iterator = typename ByteArray::const_iterator;

    /// Maximal number of bytes (template parameter)
    static constexpr int kMaxNrBytes = kMaxBytes;

    /// number of "1" in the preamble (for debugging)
    uint8 ucNrOnePreamble;

    /// Constructor
    Packet() : ucNrNbits(0) { clear(); }
    /// clear all data
    void clear()
    {
      ucNrNbits = 0;
      bytes.fill(0);
    }
    /// add a bit (0 or 1)
    void addBit(uint8 uc_bit)
    {
      refByte(byteIdx()) = static_cast<uint8>((refByte(byteIdx()) << 1u) | uc_bit);
      ucNrNbits++;
    }
    /// get a single byte
    uint8& refByte(size_type idx) { return bytes[idx]; }
    const uint8& refByte(size_type idx) const { return bytes[idx]; }
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

    /// returns an iterator to the beginning
    iterator begin() { return bytes.begin(); }
    const_iterator begin() const { return bytes.begin(); }

    /// returns an iterator to the end
    iterator end() { return bytes.end(); }
    const_iterator end() const { return bytes.end(); }
  };
  
} // namespace dcc

#endif // DCC_PACKET_H