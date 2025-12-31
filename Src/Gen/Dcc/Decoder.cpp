/**
 * @file Decoder.cpp
 *
 * @author Ralf Sondershaus
 *
 * @brief DCC Decoder for Arduino
 *
 * Declares class Dcc::Decoder with
 * - setup
 * - loop
 *
 * SPDX-License-Identifier: MIT
 */

#include <Std_Types.h>
#include <OS_Type.h> // SuspendAllInterrupts, ResumeAllInterrupts, ISR macros
#include <Dcc/BitExtractor.h>
#include <Dcc/Decoder.h>
#include <Util/Algorithm.h> // max
#include <Hal/Timer.h>
#include <Hal/Interrupt.h>

namespace dcc
{
#if CFG_DCC_DECODER_DEBUG == OPT_DCC_DECODER_DEBUG_ON
    uint16 unBitStreamMaxSize = 0;
    /// For debugging: the number of interrupt ISR_Dcc calls (can overflow)
    uint32 ISR_dcc_count;
#endif

    // ---------------------------------------------------
    /// This function is called by the ISR when a falling or rising edge has triggered the interrupt.
    ///
    /// Pushes a 0, 1, or invalid into the underlying bit stream.
    ///
    /// @note Average run time 34 usec @ATmega2560 @16 MHz with gcc -O3
    ///       Size 1342 bytes with gcc -O3
    /// @note Average run time 35 usec @ATmega2560 @16 MHz with gcc -O2
    ///       Size 1276 bytes with gcc -O2
    /// @note Average run time 52 usec @ATmega2560 @16 MHz with gcc -Os
    ///       Size 370 bytes with gcc -Os
    // ---------------------------------------------------
    void ISR_Dcc(void) noexcept
    {
        static bool bFirstCall = true;
        static unsigned long prev = 0;
        const unsigned long now = hal::micros();

        if (bFirstCall)
        {
            // first call
            bFirstCall = false;
        }
        else
        {
            // second call or beyond

            // This is the time delta in microseconds
            // Note: ULONG_MAX is the maximum value for an unsigned long, which is 4294967295 on most platforms.
            // This calculation handles the wrap-around case correctly.
            const unsigned long dt = (now >= prev) ? (now - prev) : (now + (platform::numeric_limits<unsigned long>::max_() - prev) + 1UL);
            // Execute the state machine with the time delta.
            // Calls packet_received when a full packet is received.
            decoder::get_instance().get_bit_extractor().execute(dt);
        }
        prev = now;

        #if CFG_DCC_DECODER_DEBUG == OPT_DCC_DECODER_DEBUG_ON
        // for debugging
        ISR_dcc_count++;
        #endif
    }

    // ---------------------------------------------------
    /// Initialize
    // ---------------------------------------------------
    void decoder::init(uint8 pin)
    {
        hal::attachInterrupt(digitalPinToInterrupt(static_cast<int>(pin)), ISR_Dcc, CHANGE);
    }

    /**
     * @brief Switch the double buffer for packet reception.
     */
    void decoder::fetch()
    {
        SuspendAllInterrupts();
        packet_double_buffer.toggle();
        ResumeAllInterrupts();
    }

#if CFG_DCC_DECODER_DEBUG == OPT_DCC_DECODER_DEBUG_ON
    /// for debugging: number of interrupt (ISR) calls
    uint32 decoder::get_interrupt_count() const
    {
        return ISR_dcc_count;
    }
#endif

} // namespace dcc

// EOF