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
#include <Dcc/Filter.h>
#include <Util/Fix_Queue.h>
#include <Util/Ptr.h>

namespace dcc
{
  // ---------------------------------------------------------------------
  /// The Decoder class is the main class. It converts received DCC signals
  /// into packets and pushes the packets into an internal FIFO buffer.
  /// 
  /// Clients can access the FIFO buffer with an interface that is similar to 
  /// a std::deque interface with functions such as
  /// - empty()
  /// - front()
  /// - pop()
  /// - size()
  /// 
  /// Typical application:
  /// 
  /// <CODE>
  /// Decoder myDec;
  /// Decoder::PacketType packet;
  /// int DccPinNr = 2;
  /// 
  /// myDec.init(DccPinNr);
  /// 
  /// while (1)
  /// {
  ///   myDec.fetch();
  /// 
  ///   while (!myDec.empty())
  ///   {
  ///     packet = myDec.front();
  ///     myDec.pop();
  ///     // do something useful witch packet
  ///   }
  /// }
  /// </CODE>
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
    /// Stores received packets into a FIFO buffer.
    // ---------------------------------------------------
    template<uint8 MaxNrPackets>
    class FifoHandlerIfc : public HandlerIfc
    {
    public:
      /// Packet FIFO buffer
      typedef util::fix_queue<PacketType, MaxNrPackets> PacketFifoType;
      typedef dcc::Filter<PacketType> FilterType;
      typedef util::ptr<const FilterType> FilterPointerType;
      using size_type = typename PacketFifoType::size_type;

    protected:
      PacketFifoType packetFifo;
      FilterPointerType filterPtr;
      bool bOverflow;

      /// Returns true if the packet passes a filter. Returns false otherwise.
      /// Returns true if no filter has been assigned.
      bool filter(const PacketType& pkt) const
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
      FifoHandlerIfc() : bOverflow(false)
      {}
      /// Destructor
      ~FifoHandlerIfc() override = default;

      /// Store the received packet into the list of packets (if it does not exist already)
      void packetReceived(const PacketType& pkt) override
      {
        if (packetFifo.size() < packetFifo.MaxSize)
        {
          if (filter(pkt))
          {
            packetFifo.push(pkt);
          }
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

      void setFilter(const FilterType& filter) { filterPtr = &filter; }
    };

    typedef FifoHandlerIfc<kMaxNrPackets> FifoHandlerIfcType;

    /// Handler for available packets
    FifoHandlerIfcType fifoHandler;
    /// The packet extractor
    PacketExtractorType pktExtr;
    /// The state machine for short and long half bits (-> detecting "1" and "0" and INVALID)
    BitExtractorType bitExtr;

  public:
    using size_type = typename FifoHandlerIfcType::PacketFifoType::size_type;
    using FilterType = typename FifoHandlerIfcType::FilterType;
    using FilterPointerType = typename FifoHandlerIfcType::FilterPointerType;

    /// Constructor
    Decoder() : pktExtr(fifoHandler), bitExtr(pktExtr) {}
    /// Desctructor
    ~Decoder() {}

    /// Initialize with interrupt pin
    void init(uint8 ucIntPin);
    /// Convert latest data into packets and push the packets into the FIFO buffer.
    void fetch();

    /// Enable the decoder to filter packets. 
    /// Only packets that pass the filter are stored in the FIFO buffer.
    void setFilter(const FilterType& filter) { fifoHandler.setFilter(filter); }

    /// Returns reference to the first packet in the FIFO buffer.
    const PacketType& front() const { return fifoHandler.front(); }
    /// Removes the first packet from the FIFO buffer.
    void pop() { fifoHandler.pop(); }
    /// Returns true if the FIFO buffer is empty, returns false otherwise.
    bool empty() const { return fifoHandler.empty(); }
    /// Returns the number of elements in the FIFO buffer
    size_type size() const { return fifoHandler.size(); }

    /// For debugging: get number of interrupts called since system start.
    /// Counter can overflow and start at 0 again.
    uint16 getNrInterrupts() const;
  };
} // namespace dcc

#endif // DCC_DECODER_H