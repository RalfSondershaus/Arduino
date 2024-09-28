/**
  * @file Filter.h
  * @author Ralf Sondershaus
  *
  * @brief DCC Filter
  *
  * Declares class dcc::Filter that can filter Dcc Packets.
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
  * along with this program.  If not, see <https://www.gnu.org/licenses/>.
  */

#ifndef DCC_FILTER_H
#define DCC_FILTER_H

#include <Std_Types.h>
#include <Dcc/Packet.h>

namespace dcc
{
  // ---------------------------------------------------------------------
  /// Filter class
  // ---------------------------------------------------------------------
  template<class Packet>
  class Filter
  {
  public:
    /// The type for Dcc Packets
    typedef Packet packet_type;

  protected:

  public:
    /// Constructor
    Filter() = default;

    /// Returns true if the packet passes the filter. Returns false if the packet does not pass the filter.
    /// Override this function with your logic.
    virtual bool filter(const packet_type& pkt) const noexcept = 0;
  };

  // ---------------------------------------------------------------------
  /// This Filter class lets packets pass for a specific primary address range.
  /// 
  /// 00000000              0         Broadcast address
  /// 00000001-01111111     1 - 127   Multi-Function decoders with 7 bit addresses (locomotives)
  /// 10000000-10111111   128 - 191   Basic Accessory Decoders with 9 bit addresses and Extended Accessory Decoders with 11-bit addresses
  /// 11000000-11100111   192 - 231   Multi Function Decoders with 14 bit addresses (locomotives)
  /// 11101000-11111110   232 - 254   Reserved for Future Use
  /// 11111111            255         Idle Packet
  /// 
  /// @tparam Packet Type of Dcc Packet such as dcc::Packet<6>
  // ---------------------------------------------------------------------
  template<class Packet>
  class PassPrimaryAddressFilter : public Filter<Packet>
  {
  protected:
    /// Lower address of the address range
    uint8 ucAddressLo;
    /// Higher address of the address range
    uint8 ucAddressHi;

  public:
    /// The base class
    typedef Filter<Packet> Base;
    /// The type for Dcc Packets
    using packet_type = typename Base::packet_type;

    /// The default constructor defines a filter that does not let any packet pass.
    PassPrimaryAddressFilter() { invalidate(); }

    /// Construct with address range: addrLo <= address <= addrHi
    PassPrimaryAddressFilter(uint8 addrLo, uint8 addrHi) : ucAddressLo(addrLo), ucAddressHi(addrHi) {}

    /// Set the low address and high address
    void setLo(uint8 addr) noexcept { ucAddressLo = addr; }
    void setHi(uint8 addr) noexcept { ucAddressHi = addr; }

    /// Define a filter that does not let any packet pass.
    void invalidate() noexcept { ucAddressLo = 1U; ucAddressHi = 0U; }

    /// Returns true if the packet passes the filter. Returns false if the packet does not pass the filter.
    bool filter(const packet_type& pkt) const noexcept override
    {
      const uint8 addr = pkt.getPrimaryAddress();
      return ((addr >= ucAddressLo) && (addr <= ucAddressHi));
    }
  };

  // ---------------------------------------------------------------------
    /// This Filter class lets packets pass for a specific address range.
    /// 
    /// @tparam Packet Type of Dcc Packet such as dcc::Packet<6>
    // ---------------------------------------------------------------------
  template<class Packet>
  class PassAddressFilter : public Filter<Packet>
  {
  protected:
    /// Lower address of the address range
    uint16 unAddressLo;
    /// Higher address of the address range
    uint16 unAddressHi;

  public:
    /// The base class
    typedef Filter<Packet> Base;
    /// The type for Dcc Packets
    using packet_type = typename Base::packet_type;

    /// The default constructor defines a filter that does not let any packet pass.
    PassAddressFilter() { invalidate(); }

    /// Construct with address range: addrLo <= address <= addrHi
    PassAddressFilter(uint16 addrLo, uint16 addrHi) : unAddressLo(addrLo), unAddressHi(addrHi) {}

    /// Set the low address and high address
    void setLo(uint16 addr) noexcept { unAddressLo = addr; }
    void setHi(uint16 addr) noexcept { unAddressHi = addr; }

    /// Define a filter that does not let any packet pass.
    void invalidate() noexcept { unAddressLo = 1U; unAddressHi = 0U; }

    /// Returns true if the packet passes the filter. Returns false if the packet does not pass the filter.
    bool filter(const packet_type& pkt) const noexcept override
    {
      const uint16 addr = pkt.getAddress();
      return ((addr >= unAddressLo) && (addr <= unAddressHi));
    }
  };
} // namespace dcc

#endif // DCC_FILTER_H