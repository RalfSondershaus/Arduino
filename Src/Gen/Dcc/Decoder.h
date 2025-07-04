/**
 * @file Decoder.h
 * @author Ralf Sondershaus
 * @date 2018-2023
 * @brief DCC Decoder class for Arduino.
 *
 * This header defines the @c dcc::Decoder class, which receives and decodes DCC (Digital Command Control) signals,
 * extracts packets, and stores them in an internal FIFO buffer for client access. The interface provides methods
 * similar to @c std::deque, such as @c empty(), @c front(), @c pop(), and @c size().
 *
 * Typical usage:
 * @code
 * dcc::Decoder decoder;
 * decoder.init(DccPinNr);
 * while (1) {
 *   decoder.fetch();
 *   while (!decoder.empty()) {
 *     auto packet = decoder.front();
 *     decoder.pop();
 *     // Process packet
 *   }
 * }
 * @endcode
 *
 * Features:
 * - Converts DCC signals into packets.
 * - Stores packets in a fixed-size FIFO buffer.
 * - Optional packet filtering.
 * - Supports extended addressing and output address method.
 * - Provides statistics and overflow detection.
 *
 * @copyright
 * Copyright (c) 2018-2023 Ralf Sondershaus
 *
 * @par License
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 */

#ifndef DCC_DECODER_H
#define DCC_DECODER_H

#include <Std_Types.h>
#include <Dcc/BitExtractor.h>
#include <Dcc/PacketExtractor.h>
#include <Dcc/Filter.h>
#include <Util/Fix_Queue.h>
#include <Util/Ptr.h>
#include <Hal/Serial.h>

namespace dcc
{
  /**
   * @class Decoder
   * @brief Main class for converting DCC signals into packets and providing FIFO access.
   *
   * The Decoder class receives DCC signals, decodes them into packets, and stores the packets in an internal FIFO buffer.
   * Clients can access the FIFO buffer using an interface similar to @c std::deque, with functions such as:
   * - empty()
   * - front()
   * - pop()
   * - size()
   *
   * Typical usage:
   * @code
   * dcc::Decoder myDec;
   * dcc::Decoder::PacketType packet;
   * int DccPinNr = 2;
   *
   * myDec.init(DccPinNr);
   *
   * while (1)
   * {
   *   myDec.fetch();
   *
   *   while (!myDec.empty())
   *   {
   *     packet = myDec.front();
   *     myDec.pop();
   *     // Process packet
   *   }
   * }
   * @endcode
   *
   * @note The FIFO buffer has a fixed maximum size. If more packets are received than can be stored, an overflow flag is set.
   * @note Optional packet filtering is supported.
   */
  class Decoder
  {
  public:
    using PacketExtractorType = PacketExtractor<>;
    using PacketType = PacketExtractorType::PacketType;
    /// Such a handler is called when a new packet is received
    using HandlerIfc = PacketExtractorType::HandlerIfc;

    /// This number of packets can be stored in the FIFO
    static constexpr uint8 kPacketsFifoSize = 10;

    /// Statistics for internal timer buffer (for debugging purposes)
    typedef struct
    {
      uint16 maxSize;
      uint16 curSize;
    } IsrStats;

  protected:
    /**
     * @class FifoHandlerIfc
     * @brief Handler interface for received DCC packets with FIFO buffering and optional filtering.
     *
     * This class implements the HandlerIfc interface and is responsible for storing received DCC packets
     * in a fixed-size FIFO buffer. Optionally, a filter can be assigned to only store packets that match
     * specific criteria. The handler also provides overflow detection and configuration options for
     * packet decoding (such as extended addressing and output address method).
     *
     * @tparam MaxNrPackets Maximum number of packets that can be stored in the FIFO buffer.
     *
     * **Features:**
     * - Stores received packets in a FIFO buffer of fixed size.
     * - Optional packet filtering via setFilter().
     * - Overflow detection if more packets are received than can be stored.
     * - Methods to enable/disable extended addressing and output address method.
     * - Provides access to the front packet, buffer size, and empty state.
     *
     * **Interface:**
     * - void packetReceived(PacketType &pkt) override
     * - const PacketType &front() const
     * - void pop()
     * - bool empty() const
     * - size_type size() const
     * - bool isOverflow() const noexcept
     * - void clearOverflow() noexcept
     * - void setFilter(const FilterType &filter)
     * - void enableExtendedAddressing() noexcept
     * - void disableExtendedAddressing() noexcept
     * - void enableOutputAddressMethod() noexcept
     * - void disableOutputAddressMethod() noexcept
     *
     * @note No dynamic memory allocation is used. No virtual destructor is defined for compatibility with AVR GCC.
     */
    template <uint8 MaxNrPackets>
    class FifoHandlerIfc : public HandlerIfc
    {
    public:
      /// Packet FIFO buffer
      using PacketFifoType = util::fix_queue<PacketType, MaxNrPackets>;
      using FilterType = dcc::Filter<PacketType>;
      using FilterPointerType = util::ptr<const FilterType>;
      using size_type = typename PacketFifoType::size_type;
      using config_type = typename PacketType::config_type;

      uint32 packetCnt;

    protected:
      PacketFifoType packetFifo;
      FilterPointerType filterPtr;
      config_type packetConfig;
      bool bOverflow;

      /// Returns true if the packet passes a filter. Returns false otherwise.
      /// Returns true if no filter has been assigned.
      bool filter(const PacketType &pkt) const
      {
        bool bRet = true;

        if (filterPtr)
        {
          bRet = filterPtr->filter(pkt);
        }

        return bRet;
      }

    public:
      /// Default constructor
      FifoHandlerIfc() : packetCnt(0), bOverflow(false)
      {
        packetConfig.Multifunction_ExtendedAddressing = false;
        packetConfig.Accessory_OutputAddressMethod = false;
      }

      /// Do not define (virtual) destructors
      /// - Nothing to be deleted since dynamic memory allocation is not used
      ///   and objects are destructed at shut down only (-> never).
      /// - When using virtual destructors, AVR GCC throws 'undefined reference
      ///   to `operator delete(void*, unsigned int)'.
      ///   Non-virtual destructors don't throw this error.

      /// Decode the packet, apply the filter (if any) and (if filter is passed)
      /// store it in the packet FIFO.
      /// TBD Filter out subsequent duplicates
      void packetReceived(PacketType &pkt) override
      {
        if (packetFifo.size() < packetFifo.MaxSize)
        {
          pkt.decode(packetConfig);
          // hal::serial::print(pkt.refByte(0), HEX);
          // hal::serial::print(pkt.refByte(1), HEX);
          // hal::serial::print(pkt.refByte(2), HEX);
          if (filter(pkt))
          {
            // hal::serial::println(" pass");
            packetFifo.push(pkt);
          }
          else
          {
            // hal::serial::println(" stop");
          }
        }
        else
        {
          bOverflow = true;
        }
        // for debugging
        packetCnt++;
      }

      const PacketType &front() const { return packetFifo.front(); }
      void pop() { packetFifo.pop(); }
      bool empty() const { return packetFifo.empty(); }
      size_type size() const { return packetFifo.size(); }

      bool isOverflow() const noexcept { return bOverflow; }
      void clearOverflow() noexcept { bOverflow = false; }

      void setFilter(const FilterType &filter) { filterPtr = &filter; }

      void enableExtendedAddressing() noexcept { packetConfig.Multifunction_ExtendedAddressing = true; }
      void disableExtendedAddressing() noexcept { packetConfig.Multifunction_ExtendedAddressing = false; }
      void enableOutputAddressMethod() noexcept { packetConfig.Accessory_OutputAddressMethod = true; }
      void disableOutputAddressMethod() noexcept { packetConfig.Accessory_OutputAddressMethod = false; }
    };

    using FifoHandlerIfcType = FifoHandlerIfc<kPacketsFifoSize>;

    /// Handler for available packets
    FifoHandlerIfcType fifoHandler;
    /// The packet extractor
    PacketExtractorType pktExtr;

  public:
    using size_type = typename FifoHandlerIfcType::PacketFifoType::size_type;
    using FilterType = typename FifoHandlerIfcType::FilterType;
    using FilterPointerType = typename FifoHandlerIfcType::FilterPointerType;

    /// Constructor
    Decoder() : pktExtr(fifoHandler) {}
    /// Desctructor
    ~Decoder() {}

    /// Initialize with interrupt pin
    void init(uint8 ucIntPin);
    /// Convert latest data into packets and push the packets into the FIFO buffer.
    void fetch();

    /// Enable the decoder to filter packets.
    /// Only packets that pass the filter are stored in the FIFO buffer.
    void setFilter(const FilterType &filter) { fifoHandler.setFilter(filter); }

    /// For Multi-function decoders: enable extended addressing
    /// (and disable basic addressing).
    /// Controlled with bit 5 of CV 29 (Configuration Variable).
    void enableExtendedAddressing() noexcept { fifoHandler.enableExtendedAddressing(); }
    /// For Multi-function decoders: disable extended addressing
    /// (and enable basic addressing).
    /// Controlled with bit 5 of CV 29 (Configuration Variable).
    void disableExtendedAddressing() noexcept { fifoHandler.disableExtendedAddressing(); }
    /// For Accessory decoders: endable output adressing method
    /// (and disable decoder addressing).
    /// Controlled with bit 6 of CV 29 (Configuration Variable).
    void enableOutputAddressMethod() noexcept { fifoHandler.enableOutputAddressMethod(); }
    /// For Accessory decoders: disable output adressing method
    /// (and enable decoder addressing).
    /// Controlled with bit 6 of CV 29 (Configuration Variable).
    void disableOutputAddressMethod() noexcept { fifoHandler.disableOutputAddressMethod(); }

    /// Returns reference to the first packet in the FIFO buffer.
    const PacketType &front() const { return fifoHandler.front(); }
    /// Removes the first packet from the FIFO buffer.
    void pop() { fifoHandler.pop(); }
    /// Returns true if the FIFO buffer is empty, returns false otherwise.
    bool empty() const { return fifoHandler.empty(); }
    /// Returns the number of elements in the FIFO buffer
    size_type size() const { return fifoHandler.size(); }

    /// Returns true if the packet FIFO had an overflow
    /// (more than kPacketsFifoSize need to be stored).
    bool fifoOverflow() const noexcept { return fifoHandler.isOverflow(); }
    size_t fifoSize() const noexcept { return fifoHandler.size(); }

    /// Returns statistics for internal time stamp buffer
    void isrGetStats(IsrStats &stat) const noexcept;
    bool isrOverflow() const noexcept;

    /// For debugging: get number of interrupts called since system start.
    /// Counter can overflow and start at 0 again.
    uint32 getNrInterrupts() const;
    uint32 getNrFetches() const;
    uint32 getNrOnes() const { return pktExtr.ulOnes; }
    uint32 getNrZeros() const { return pktExtr.ulZeros; }
    uint32 getNrInvalids() const { return pktExtr.ulInvalids; }

    uint32 getPacketCount() const noexcept { return fifoHandler.packetCnt; }
  };
} // namespace dcc

#endif // DCC_DECODER_H