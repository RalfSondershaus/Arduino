/**
 * @file Decoder.h
 * 
 * @author Ralf Sondershaus
 * 
 * @brief DCC Decoder class for Arduino.
 *
 * @copyright Copyright (c) 2018-2025 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DCC_DECODER_H
#define DCC_DECODER_H

#include <Std_Types.h>
#include <Dcc/DecoderCfg.h>
#include <Dcc/BitExtractor.h>
#include <Dcc/PacketExtractor.h>
#include <Dcc/Filter.h>
#include <Util/Fix_Queue.h>
#include <Util/Ptr.h>

namespace dcc
{
    /**
     * @class decoder
     * @brief Main class for converting DCC signals into packets and providing FIFO access.
     *
     */
    class decoder : public packet_extractor<>::handler_ifc
    {
    public:
        using bit_extractor_type = bit_extractor<>;
        using packet_extractor_type = packet_extractor<>;
        using packet_type = packet_extractor_type::packet_type;
        using handler_ifc = packet_extractor_type::handler_ifc;
        using filter_type = dcc::filter<packet_type>;
        using filter_pointer_type = util::ptr<const filter_type>;

        static const size_t kMaxNrPackets = CFG_DCC_DECODER_FIFO_SIZE; ///< Maximal number of packets stored in FIFO

    protected:
        /// Packet FIFO buffer
        using packet_fifo_type = util::fix_queue<packet_type, kMaxNrPackets>;
        using size_type = typename packet_fifo_type::size_type;

        /**
         * @brief Constructor
         */
        decoder() : my_packet_extractor(*this), my_bit_extractor(my_packet_extractor) 
        #if CFG_DCC_DECODER_DEBUG == OPT_DCC_DECODER_DEBUG_ON
        , packet_count(0)
        #endif        
        {}

        /**
         * @brief Packet FIFO buffers for double buffering.
         * 
         * Each buffer is used to store received packets. While one buffer is being filled by the ISR,
         * the other buffer can be processed in the main loop.
         */
        class double_buffer
        {
        public:
            /** The two packet FIFOs for double buffering. */
            packet_fifo_type packet_fifo[2];
            /** Index for double buffering [0, 1] */
            uint8 idx;
            /** Indicates if an overflow has occurred in the double buffer */
            bool overflow;
            /** Constructor */
            double_buffer() : idx{0}, overflow{false} {}
            /** Toggle the buffer index between 0 and 1 */
            void toggle() noexcept { idx = static_cast<uint8>(1U - idx); }
            /** Return the index of the buffer currently used for writing */
            uint_fast8_t write_index() const noexcept { return idx; }
            /** Return the index of the buffer currently used for reading */
            uint_fast8_t read_index() const noexcept { return static_cast<uint_fast8_t>(1U - idx); }
            /** 
             * Push a packet into the current buffer. If the buffer is full, set the overflow flag.
             */
            void push(const packet_type& pkt) 
            { 
                if (packet_fifo[write_index()].size() < packet_fifo[write_index()].max_size())
                {
                    packet_fifo[write_index()].push(pkt); 
                }
                else
                {
                    overflow = true;
                }
            }
            /** Return reference to the front packet in the current read buffer */
            packet_type &front() { return packet_fifo[read_index()].front(); }
            /** Pop the front packet from the current read buffer */
            void pop() { packet_fifo[read_index()].pop(); }
            /** Check if the current read buffer is empty */
            bool empty() const { return packet_fifo[read_index()].empty(); }
            /** Return the size of the current read buffer */
            size_type size() const { return packet_fifo[read_index()].size(); }
            /** Check if an overflow has occurred in the double buffer. */
            bool is_overflow() const noexcept { return overflow; }
            /** Clear the overflow flag in the double buffer. */
            void clear_overflow() noexcept { overflow = false; }
        };

        double_buffer packet_double_buffer;

        /**
         * @brief Extractor for DCC packets from bit extractor.
         */
        packet_extractor_type my_packet_extractor;

        /**
         * @brief Extract DCC bits from signal edges and forward bits to packet extractor.
         */
        bit_extractor_type my_bit_extractor;

        /**
         * @brief Optional: use filter to filter packets.
         */
        filter_pointer_type filter_ptr;
        
        #if CFG_DCC_DECODER_DEBUG == OPT_DCC_DECODER_DEBUG_ON
        /**
         * @brief Number of packets received since system start. Can overflow.
         */
        uint32 packet_count;
        #endif

    public:
        /**
         * @brief Destructor (default)
         */
        ~decoder() = default;

        /**
         * @brief decoder is a singleton class and shall not be copied or assigned.
         */
        decoder(const decoder&) = delete;
        decoder& operator=(const decoder&) = delete;

        /**
         * @brief decoder is a singleton class and shall not be moved.
         */
        decoder(decoder&&) = delete;
        decoder& operator=(decoder&&) = delete;

        /**
         * @brief Get the singleton instance of the decoder.
         * 
         * @return Reference to the singleton decoder instance.
         */
        static decoder& get_instance()
        {
            static decoder instance;
            return instance;
        }

        /**
         * @brief Returns reference to the bit extractor.
         * 
         * @return Reference to the bit extractor.
         */
        bit_extractor_type& get_bit_extractor() noexcept { return my_bit_extractor; }

        /**
         * @brief Initialize with interrupt pin
         * 
         * @param ucIntPin Interrupt pin number
         */
        void init(uint8 ucIntPin);

        /**
         * @brief Switch the packet buffers and prepare for reading from the current read FIFO.
         */
        void fetch();
        /** 
         * @brief Returns reference to the front packet in the current read FIFO.
         */
        packet_type &front() { return packet_double_buffer.front(); }
        /** 
         * @brief Pop the front packet from the current read FIFO.
         */
        void pop() { return packet_double_buffer.pop(); }
        /** 
         * @brief Check if the current read FIFO is empty.
         */
        bool empty() const { return packet_double_buffer.empty(); }
        /** 
         * @brief Return the size of the current read FIFO.
         */
        size_type size() const { return packet_double_buffer.size(); }

        /** 
         * @brief Check if an overflow has occurred in the double buffer.
         */
        bool is_fifo_overflow() const noexcept { return packet_double_buffer.is_overflow(); }
        /** 
         * @brief Clear the overflow flag in the double buffer.
         */
        void clear_fifo_overflow() noexcept { packet_double_buffer.clear_overflow(); }

        /**
         * @brief Set the filter for incoming packets. Only packets that pass the filter are 
         * forwarded.
         * 
         * @param filter Reference to the filter to be used.
         */
        void set_filter(const filter_type &filter) { filter_ptr = &filter; }

        /**
         * @brief Called when a new packet is received. Can be called from an ISR context.
         * 
         * @note pkt is not const to allow modification, e.g., for type calculation.
         * @note Can be called from an ISR context.
         * 
         * @param pkt Reference to the received DCC packet
         */
        virtual void packet_received(packet_type& pkt) override 
        {
            bool process_packet = true;
            if (filter_ptr && (!filter_ptr->do_filter(pkt)))
            {
                process_packet = false;
            }
            if (process_packet)
            {
                packet_double_buffer.push(pkt);
            }
            #if CFG_DCC_DECODER_DEBUG == OPT_DCC_DECODER_DEBUG_ON
            packet_count++;
            #endif
        }

        /**
         * @brief Handles the reception of basic DCC accessory packets.
         * 
         * This function processes basic DCC packets. It calculates the command from the packet's
         * address and the output direction, and forwards the result to the RTE. The position on RTE 
         * is calculated from the DCC address of the packet minus the first output address.
         * 
         * @param pkt Reference to the received DCC packet
         */
        virtual void basic_packet_received(packet_type& pkt) { (void) pkt; }
        
        /**
         * @brief Handles the reception of extended DCC accessory packets.
         * 
         * This function processes extended DCC packets. It forwards the aspect value to the RTE.
         * The position on RTE is calculated from the DCC address of the packet minus the first output
         * address.
         * 
         * @param pkt Reference to the received DCC packet
         */
        virtual void extended_packet_received(packet_type& pkt) { (void) pkt; }
        /**
         * @brief 
         * 
         * @param pkt 
         */
        virtual void any_packet_received(packet_type& pkt) { (void) pkt; }

        /**
         * @brief Check if an overflow has occurred in the ISR context.
         * 
         * @return true if an overflow has occurred; false otherwise.
         */
        bool isr_overflow() const noexcept;

        #if CFG_DCC_DECODER_DEBUG == OPT_DCC_DECODER_DEBUG_ON
        /// For debugging: get number of interrupts called since system start.
        /// Counter can overflow and start at 0 again.
        uint32 get_interrupt_count() const;
        uint32 get_ones_count() const { return my_packet_extractor.ones_count; }
        uint32 get_zeros_count() const { return my_packet_extractor.zeros_count; }
        uint32 get_invalids_count() const { return my_packet_extractor.invalids_count; }
        uint32 get_packet_count() const noexcept { return packet_count; }
        /**
         * @brief Get the bit extractor call count object
         * @param state State index (must be less than STATE_MAX_COUNT).
         * @return uint32 The number of times the state has been entered, or -1 if state is out of 
         * range.
         */
        uint32 get_bit_extractor_call_count(unsigned int state) const
        {
            return my_bit_extractor.get_call_count(state);
        }
        #endif
    };
} // namespace dcc

#endif // DCC_DECODER_H