/**
  * @file Decoder.h
  * @author Ralf Sondershaus
  *
  * @brief DCC Decoder for Arduino
  * 
  * Declares class dcc::Decoder with
  * - setup
  * - loop
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
  * along with this program.  If not, see <https://www.gnu.org/licenses/>.
  */

#ifndef DCC_DECODER_H
#define DCC_DECODER_H

#include <Std_Types.h>
#include <Dcc/BitExtractor.h>
#include <Dcc/PacketExtractor.h>
#include <Util/Fix_Queue.h>

namespace dcc
{
  // ---------------------------------------------------------------------
  /// Main class
  // ---------------------------------------------------------------------
  class Decoder
  {
  public:
    typedef BitExtractorConstants<> BitExtractorConstantsType;
    typedef PacketExtractor<> PacketExtractorType;
    typedef BitExtractor<BitExtractorConstantsType, PacketExtractorType> BitExtractorType;

    typedef PacketExtractorType::PacketType PacketType;

    /// Such a handler is called if a new packet is received
    typedef PacketExtractorType::HandlerIfc HandlerIfc;

    static constexpr uint8 kMaxNrPackets = 10;

  protected:

    // ---------------------------------------------------
    /// Interface for a handler that is called if a packet is received.
    /// Stores received packets and counts how often a packet is received.
    // ---------------------------------------------------
    template<uint8 MaxNrPackets>
    class FifoHandlerIfc : public HandlerIfc
    {
    public:
      /// Packet FIFO buffer
      typedef util::fix_queue<PacketType, MaxNrPackets> PacketFifoType;
      using size_type = typename PacketFifoType::size_type;

    protected:
      PacketFifoType packetFifo;
      bool bOverflow;

    public:
      /// Default constructor
      FifoHandlerIfc() : bOverflow(false)
      {}
      /// Destructor
      virtual ~FifoHandlerIfc() = default;

      /// Store the received packet into the list of packets (if it does not exist already)
      virtual void packetReceived(const PacketType& pkt) override
      {
        if (packetFifo.size() < packetFifo.MaxSize)
        {
          packetFifo.push(pkt);
        }
        else
        {
          bOverflow = true;
        }
      }

      const PacketType& front() const   { return packetFifo.front(); }
      void pop()                        { packetFifo.pop(); }
      bool empty() const                { return packetFifo.empty(); }
      size_type size() const            { return packetFifo.size(); }

      bool isOverflow() const noexcept  { return bOverflow; }
      void clearOverflow() noexcept     { bOverflow = false; }
    };

    typedef FifoHandlerIfc<kMaxNrPackets> FifoHandlerIfcType;

    /// Handler for available packets
    FifoHandlerIfcType fifoHandler;
    /// The state machine for short and long half bits (-> detecting "1" and "0" and INVALID)
    BitExtractorType bitExtr;
    /// The packet extractor
    PacketExtractorType pktExtr;

  public:
    using size_type = typename FifoHandlerIfcType::PacketFifoType::size_type;

    /// Constructor
    Decoder() : pktExtr(fifoHandler), bitExtr(pktExtr) {}
    /// Desctructor
    ~Decoder() {}

    /// Initialize with interrupt pin
    void init(uint8 ucIntPin);
    /// Convert latest data into packets
    void fetch();

    /// Returns reference to the first packet in the queue. 
    const PacketType& front() const { return fifoHandler.front(); }
    /// Removes an element from the front
    void pop() { fifoHandler.pop(); }
    /// Checks if the underlying container has no elements
    bool empty() const { return fifoHandler.empty(); }
    /// Returns the number of elements in the underlying fifo
    size_type size() const { return fifoHandler.size(); }

    uint16 getNrInterrupts() const;
  };
} // namespace dcc

#endif // DCC_DECODER_H