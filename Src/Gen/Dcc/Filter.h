/**
 * @file filter.h
 * @author Ralf Sondershaus
 *
 * @brief DCC filter
 *
 * Declares class dcc::filter that can filter Dcc Packets.
 *
 * @copyright Copyright 2023 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DCC_FILTER_H
#define DCC_FILTER_H

#include <Std_Types.h>
#include <Dcc/Packet.h>

namespace dcc
{
    // ---------------------------------------------------------------------
    /// filter class
    // ---------------------------------------------------------------------
    template <class TPacket>
    class filter
    {
    public:
        using packet_type = TPacket;

        /**
         * @brief Default constructor for the filter class.
         */
        filter() = default;

        /**
         * @brief Determines whether a given DCC packet passes the filter criteria.
         * Override this function with your logic.
         * @param pkt The DCC packet to be evaluated.
         * @return `true` if the packet passes the filter; `false` otherwise.
         */
        virtual bool do_filter(packet_type &pkt) const noexcept = 0;
    };

    // ---------------------------------------------------------------------
    /// This filter class lets packets pass for a specific primary address range.
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
    template <class Packet>
    class pass_primary_address_filter : public filter<Packet>
    {
    protected:
        /// Lower address of the address range
        uint8 ucAddressLo;
        /// Higher address of the address range
        uint8 ucAddressHi;

    public:
        /// The base class
        using parent_type = filter<Packet>;
        /// The type for Dcc Packets
        using packet_type = typename parent_type::packet_type;

        /// The default constructor defines a filter that does not let any packet pass.
        pass_primary_address_filter() { invalidate(); }

        /// Construct with address range: addrLo <= address <= addrHi
        pass_primary_address_filter(uint8 addrLo, uint8 addrHi) : ucAddressLo(addrLo), ucAddressHi(addrHi) {}

        /// Set the low address and high address
        void setLo(uint8 addr) noexcept { ucAddressLo = addr; }
        void setHi(uint8 addr) noexcept { ucAddressHi = addr; }

        /// Define a filter that does not let any packet pass.
        void invalidate() noexcept
        {
            ucAddressLo = 1U;
            ucAddressHi = 0U;
        }

        /// Returns true if the packet passes the filter. Returns false if the packet does not pass the filter.
        bool do_filter(packet_type &pkt) const noexcept override
        {
            const uint8 addr = pkt.get_primary_address();
            return ((addr >= ucAddressLo) && (addr <= ucAddressHi));
        }
    };

    // ---------------------------------------------------------------------
    /// This filter class lets packets pass for a specific address range.
    ///
    /// @tparam Packet Type of Dcc Packet such as dcc::Packet<6>
    // ---------------------------------------------------------------------
    template <class Packet>
    class pass_address_filter : public filter<Packet>
    {
    public:
        /// The base class
        using parent_type = filter<Packet>;
        using packet_type = typename parent_type::packet_type;
        using address_type = typename Packet::address_type;

    protected:
        /// Lower address of the address range
        address_type lower_address;
        /// Higher address of the address range
        address_type higher_address;
        /**
         * @brief CV29 value for the decoder. Used for address calculation.
         */
        uint8 cv29;
    public:

        /// The default constructor defines a filter that does not let any packet pass.
        pass_address_filter() { invalidate(); }

        /// Construct with address range: addrLo <= address <= addrHi
        pass_address_filter(address_type addr_lo, address_type addr_hi) : lower_address(addr_lo), higher_address(addr_hi) {}

        /**
         * @brief Set the cv29. Used for address calculation.
         * 
         * @param cv29_value The value of CV29 from the decoder's configuration.
         */
        void set_cv29(uint8 cv29_value) noexcept { cv29 = cv29_value; }

        /**
         * @brief Get the cv29
         * 
         * @return uint8 The value of CV29
         */
        uint8 get_cv29() const noexcept { return cv29; }

        /**
         * @brief Set the lower address and higher address
         * @param addr The address
         */
        void set_lo(address_type addr) noexcept { lower_address = addr; }
        void set_hi(address_type addr) noexcept { higher_address = addr; }

        /**
         * @brief Get the lower address and higher address
         * 
         * @return address_type The lower address
         * @return address_type The higher address 
         */
        address_type get_lo() const noexcept { return lower_address; }
        address_type get_hi() const noexcept { return higher_address; }

        /// Define a filter that does not let any packet pass.
        void invalidate() noexcept
        {
            lower_address = 1U;
            higher_address = 0U;
        }

        /// Returns true if the packet passes the filter. Returns false if the packet does not pass the filter.
        bool do_filter(packet_type &pkt) const noexcept override
        {
            const address_type addr = pkt.get_address(cv29);
            return ((addr >= lower_address) && (addr <= higher_address));
        }
    };

    // ---------------------------------------------------------------------
    /// This filter lets accessory packets pass for a specific address range.
    ///
    /// @tparam Packet Type of Dcc Packet such as dcc::Packet<6>
    // ---------------------------------------------------------------------
    template <class Packet>
    class pass_accessory_address_filter : public filter<Packet>
    {
    public:
        /// The base class
        using parent_type = filter<Packet>;
        using packet_type = typename parent_type::packet_type;
        using address_type = typename Packet::address_type;

    protected:
        /// Lower address of the address range
        address_type lower_address;
        /// Higher address of the address range
        address_type higher_address;
        /**
         * @brief CV29 value for the decoder. Used for address calculation.
         */
        uint8 cv29;
    public:

        /// The default constructor defines a filter that does not let any packet pass.
        pass_accessory_address_filter() { invalidate(); }

        /// Construct with address range: addrLo <= address <= addrHi
        pass_accessory_address_filter(address_type addr_lo, address_type addr_hi) : lower_address(addr_lo), higher_address(addr_hi) {}

        /**
         * @brief Set the cv29. Used for address calculation.
         * 
         * @param cv29_value The value of CV29 from the decoder's configuration.
         */
        void set_cv29(uint8 cv29_value) noexcept { cv29 = cv29_value; }

        /**
         * @brief Get the cv29
         * 
         * @return uint8 The value of CV29
         */
        uint8 get_cv29() const noexcept { return cv29; }

        /**
         * @brief Set the lower address and higher address
         * @param addr The address
         */
        void set_lo(address_type addr) noexcept { lower_address = addr; }
        void set_hi(address_type addr) noexcept { higher_address = addr; }

        /**
         * @brief Get the lower address and higher address
         * 
         * @return address_type The lower address
         * @return address_type The higher address 
         */
        address_type get_lo() const noexcept { return lower_address; }
        address_type get_hi() const noexcept { return higher_address; }

        /// Define a filter that does not let any packet pass.
        void invalidate() noexcept
        {
            lower_address = 1U;
            higher_address = 0U;
        }

        /// Returns true if the packet passes the filter. Returns false if the packet does not pass the filter.
        bool do_filter(packet_type &pkt) const noexcept override
        {
            bool does_pass = false;
            // Check if packet is an accessory packet
            if ((pkt.get_type() == packet_type::packet_type::BasicAccessory) ||
                (pkt.get_type() == packet_type::packet_type::ExtendedAccessory))
            {
                // An accessory packet
                const address_type addr = pkt.get_address(cv29);
                does_pass = ((addr >= lower_address) && (addr <= higher_address));
            }
            return does_pass;
        }
    };
} // namespace dcc

#endif // DCC_FILTER_H