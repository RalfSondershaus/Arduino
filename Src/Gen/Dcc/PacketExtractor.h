/**
 * @file PacketExtractor.h
 * 
 * @author Ralf Sondershaus
 *
 * @brief Extract a DCC packet from a bit stream.
 */

#ifndef DCC_PACKETEXTRACTOR_H
#define DCC_PACKETEXTRACTOR_H

#include <Std_Types.h>
#include <Dcc/Packet.h>
#include <Dcc/DecoderCfg.h>

namespace dcc
{
  // ---------------------------------------------------
  /**
   * @brief Extract DCC packets from a bit stream.
   * 
   * From RCN‑210 (DCC bit transmission):
   * 
   * A “1” bit has a duration of 58 µs per half‑wave, i.e. 116 µs per complete bit.
   * 
   * A “0” bit is at least twice as long as a 1‑bit.
   * 
   * This means:
   *
   * 
   * | Bit | Duration | Frequency |
   * |-----|----------|-----------|
   * | 1‑bit | 116 µs | approx. 8.62 kbit/s |
   * | 0‑bit | ≥ 232 µs | ≤ 4.31 kbit/s |
   * 
   * - Preamble: At least 10 consecutive "1" bits.
   * - Separator: A single "0" bit after the preamble.
   * - Data Bytes: Each data byte is transmitted with its bits followed by a "0" bit as a separator.
   * - Checksum: The checksum byte is transmitted like a data byte, followed by a final "1" bit as
   *   a separator to indicate the end of the packet.
   * 
   * A typical locomotive speed packet has 3 data bytes.
   * 
   * Example calculation (typical speed packet)
   * 
   * 10 one‑bits → 10 × 116 µs = 1.16 ms (but typically 16 one‑bits are sent: 16 × 116 µs = 1.856 ms)
   * 4 separator bits (zero‑bits) → 4 × 232 µs = 0.928 ms
   * 3 data bytes + 1 checksum byte → 4 × 8 one‑bits = 32 × 116 µs = 3.712 ms
   *
   * Total: approx. 5.8 ms per packet (with 10 preamble bits), or approx. 6.5 ms (with 16 preamble bits)
   * 
   * This corresponds to a packet rate per:
   * 
   * | packet rate | Preamble: 10 one‑bits | Preamble: 16 one‑bits |
   * |-------------|-----------------------|-----------------------|
   * | per second  | approx. 170 packets   | approx. 150 packets   |
   * | per 10 ms    | approx. 1.7 packets   | approx. 1.5 packets   |
   * | per 20 ms    | approx. 3.4 packets   | approx. 3.0 packets   |
   * | per 50 ms    | approx. 8.5 packets   | approx. 7.5 packets   |
   * 
   * Practical values
   * 
   * - Locomotive commands: ~6–7 ms per packet
   * - Accessory commands: similar, depending on the number of bytes
   * - Broadcast packets (e.g., emergency stop): shorter
   * - Long packets (e.g., CV programming): longer
   * 
   * @tparam PreambleMinNrOnes Minimum number of "1" bits in the preamble to consider it valid (default: 10).
   */
  template<int PreambleMinNrOnes = 10>
  class packet_extractor
  {
  public:
    /// This class
    using this_class = packet_extractor<PreambleMinNrOnes>;
    /// The Packet type
    using packet_type = packet<>;

    /// Interface for a handler. Such a handler is called when a new packet is available
    class handler_ifc
    {
    public:
      /// The packets
      using packet_type = this_class::packet_type;
      /// Construct and destruct
      handler_ifc() {}

      /// Do not define (virtual) destructors
      /// - Nothing to be deleted since dynamic memory allocation is not used
      ///   and objects are destructed at shut down only (-> never).
      /// - When using virtual destructors, AVR GCC throws 'undefined reference 
      ///   to `operator delete(void*, unsigned int)'.

      /// If a new packet is available, this function is called
      /// The parameter pkt is not const to enable the handler to
      /// modify the received packet, e.g. to call decode() for
      /// address calculation.
      virtual void packet_received(packet_type& pkt) = 0;
    };

  protected:
    /// Interpretation state
    enum class eState
    {
      PREAMBLE  = 0,   ///< Receiving preamble
      DATA      = 1,   ///< Receiving adress or data bytes
      MAX_COUNT = 2
    };

    /// Bit "0" or Bit "1" bit received
    enum class eBit
    {
      ZERO = 0,
      ONE = 1
    };
    /// Current state
    eState state;

    /// A preamble is valid if this number of "1" is transmitted (at least)
    static constexpr uint8 get_preamble_min_nr_ones() { return static_cast<uint8>(PreambleMinNrOnes); }

    /// Returns true if `n` exceeds the minimal number of "1"
    constexpr bool is_preamble_valid(uint8 n) const { return n >= get_preamble_min_nr_ones(); }

    /// trigger state machine with an event (received bit)
    void execute(eBit bitRcv);

    /// State machine handle function for STATE_PREAMBLE 
    eState execute_preamble(eBit bitRcv);
    /// State machine handle function for STATE_DATA 
    eState execute_data(eBit bitRcv);

    /// Number of "1" received
    uint8_least preamble_one_count;
    /// Count received data bits for current packet per byte: first 8 bits are stored in a packet, 9th bit defines "packet finished" (1 bit) or "more bytes" (0 bit)
    /// Counts from 0 (nothing received yet) up to 9 (trailing 0 bit)
    uint8_least data_bits_count;

    /// Reference for handler_ifc interface. This interface is called as soon as a new packet is available.
    handler_ifc& handler;

    /// Packet that is processed (received) currently
    packet_type current_packet;

  public:
    #if CFG_DCC_DECODER_DEBUG == OPT_DCC_DECODER_DEBUG_ON
    uint32 ones_count;
    uint32 zeros_count;
    uint32 invalids_count;
    void inc_ones() { ones_count++; }
    void inc_zeros() { zeros_count++; }
    void inc_invalids() { invalids_count++; }
    #else
    void inc_ones() {}
    void inc_zeros() {}
    void inc_invalids() {}
    #endif

    /// constructor
    packet_extractor(handler_ifc& hifc)
      : state(eState::PREAMBLE)
      , data_bits_count(0u)
      , handler(hifc)
    {
      invalid();
    }

    /// destructor
    ~packet_extractor() = default;
    /// event trigger: "1" bit received
    void one() { inc_ones(); execute(eBit::ONE); }
    /// event trigger: "0" bit received
    void zero() { inc_zeros(); execute(eBit::ZERO); }
    /// event trigger: Invalid received, reset
    void invalid()
    {
      inc_invalids();
      state = eState::PREAMBLE;
      preamble_one_count = 0u;
      data_bits_count = 0u;
      current_packet.clear();
    }
  };

  // ---------------------------------------------------
  /// State function: check if a valid preamble is transmitted:
  /// Sequence of at least 10x "1", followed by a "0" 
  // ---------------------------------------------------
  template<int PreambleMinNrOnes>
  typename packet_extractor<PreambleMinNrOnes>::eState packet_extractor<PreambleMinNrOnes>::execute_preamble(eBit bitRcv)
  {
    eState nextState = state;

    if (bitRcv == eBit::ONE)
    {
      // prevent overflow
      if (preamble_one_count < static_cast<uint8_least>(255))
      {
        preamble_one_count++;
      }
    }
    else // eBit::ZERO
    {
      if (is_preamble_valid(preamble_one_count))
      {
        nextState = eState::DATA;
      }

      // store number of preamble "1" in the packet; can be optimized (use current_packet.preamble_one_count instead of preamble_one_count)
      current_packet.preamble_one_count = preamble_one_count;
      // reset counter because
      // - min number of "1" not reached, invalid or waiting for first "1"
      // - or valid preamble detected, switch to STATE_DATA but prepare next switch to STATE_PREAMBLE
      preamble_one_count = 0;
    }

    return nextState;
  }

  // ---------------------------------------------------
  /// State function: Interpret adress or data bytes bit by bit.
  // ---------------------------------------------------
  template<int PreambleMinNrOnes>
  typename packet_extractor<PreambleMinNrOnes>::eState packet_extractor<PreambleMinNrOnes>::execute_data(eBit bitRcv)
  {
    eState next_state = state;

    if (data_bits_count < 8u)
    {
      current_packet.addBit(static_cast<uint8>(bitRcv));
      data_bits_count++;
    }
    else
    {
      data_bits_count = 0u;

      // Bit 0 is expected at the end of a data / address byte
      // If a 1 bit is received instead of a 0 bit, the packet is finished and the next packet is to be received
      if (bitRcv == eBit::ONE)
      {
        next_state = eState::PREAMBLE;
        // notify handler about new packet
        handler.packet_received(current_packet);
        // prepare next reception
        current_packet.clear();
      }
    }

    return next_state;
  }

  // ---------------------------------------------------
  /// trigger state machine
  // ---------------------------------------------------
  template<int PreambleMinNrOnes>
  void packet_extractor<PreambleMinNrOnes>::execute(eBit bit_rcv)
  {
    switch (state)
    {
    case eState::PREAMBLE: { state = execute_preamble(bit_rcv); } break;
    case eState::DATA: { state = execute_data(bit_rcv); } break;
    default: {} break;
    }
  }

} // namespace dcc

#endif // DCC_PACKETEXTRACTOR_H