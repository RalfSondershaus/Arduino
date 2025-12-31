/**
 * @file DccSniffer.cpp
 * @author Ralf Sondershaus
 *
 * @brief DCC Sniffer for Arduino - captures DCC packets and outputs them via a simple binary protocol over serial.
 *
 * This application listens to DCC signals on a specified interrupt pin, decodes them into packets using the dcc::Decoder,
 * and transmits the packets over the serial interface in a compact binary format. It also provides periodic status output
 * with statistics such as interrupt count, fetches, and buffer usage.
 *
 * @section BinaryProtocol Binary Protocol Description
 *
 * Each decoded DCC packet is sent as a binary message with the following structure:
 *   <0x2E> <len> <data bytes> <0x00>
 *
 * - 0x2E: Start-of-packet marker (kCodeResponse)
 * - len:  Number of data bytes in the packet (1 byte)
 * - data bytes: The raw DCC packet bytes (len bytes)
 * - 0x00: End-of-packet marker
 *
 * Example (for a 3-byte DCC packet 0xAA 0xBB 0xCC):
 *   2E 03 AA BB CC 00
 *
 * @note The ASCII output function (PrintAscii) is provided for debugging and prints each byte as two hex digits separated by '-'.
 *
 * @section Usage
 * - Connect the DCC signal to the configured interrupt pin (default: pin 2).
 * - Open a serial terminal at the configured baud rate (default: 115200).
 * - Each received DCC packet will be output in the binary protocol format described above.
 * - You can use the Windows application DccSniffer to visualize the packets in real-time.
 * - You can connect with a Windows terminal program (e.g., PuTTY or HTerm) to see the
 *   ASCII output (between the binary packets) for debugging purposes.
 *
 * @note This documentation was partly generated from the source code using Gen AI (GitHub Copilot).
 *
 * @copyright Copyright 2018 - 2025 Ralf Sondershaus
 */

#include <Std_Types.h>
#include <Dcc/Decoder.h>
#include <Util/Array.h>
#include <Util/Fix_Queue.h>
#include <Util/Algorithm.h>
#include <Util/Timer.h>
#include <Arduino.h>
#include <Hal/Serial.h>
#include <Hal/Timer.h>

/// The Arduino pin for DCC interrupts
static constexpr uint8 kInterruptPin = 2U;
static constexpr uint8 kBlinkLedPin = 13U;
static constexpr util::MilliTimer::time_type kBlinkLedPeriod_ms = 1000U;
static constexpr uint32 kSerialBaudRate = 115200; // 9600, 19200, 28800, 56000, 115200
static constexpr char kCodeResponse = 0x2E;

using packet_type = dcc::decoder::packet_type;
using size_type = packet_type::size_type;

/**
 * @brief Toggle the LED pin.
 */
void toggleLedPin()
{
    static uint8 lastWrite = LOW;

    if (lastWrite == LOW)
    {
        lastWrite = HIGH;
    }
    else
    {
        lastWrite = LOW;
    }

    digitalWrite(kBlinkLedPin, lastWrite);
}

// ---------------------------------------------------
/// Arduino's setup() function.
/// Setup DCC decoder.
/// Setup serial connection.
// ---------------------------------------------------
void setup()
{
    dcc::decoder::get_instance().init(kInterruptPin);

    pinMode(kBlinkLedPin, OUTPUT);

    hal::serial::begin(kSerialBaudRate);
}

static inline uint8 convertNibbleToHex(uint8 ucNibble)
{
    static const uint8 aucChars[16] =
        {
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    // look up and prevent index-out-of-bounds
    return aucChars[ucNibble % 16U];
}

static void convertToHex(uint8 ucByte, char pcBuf[3])
{
    pcBuf[0] = convertNibbleToHex((ucByte >> 4U) & 0x0FU);
    pcBuf[1] = convertNibbleToHex((ucByte >> 0U) & 0x0FU);
    pcBuf[2] = 0;
}

// ----------------------------------------------------
/// Formats bytes to ASCII and prints to Serial
// ----------------------------------------------------
void PrintAscii(const packet_type &pkt)
{
    char pcBuf[3];
    size_type i;

    const size_type n = pkt.getNrBytes();

    for (i = 0; i < n; i++)
    {
        convertToHex(pkt.refByte(i), pcBuf);
        hal::serial::print(pcBuf);
        hal::serial::print("-");
    }
    hal::serial::println();
}

// ----------------------------------------------------
/// Prints bytes to Serial:
/// <0x2E> <len> <bytes> <0>
// ----------------------------------------------------
void PrintBin(const packet_type &pkt)
{
    size_type i;
    // Response byte + length + kMaxNrBytes + 0
    uint8 pcBuf[packet_type::kMaxNrBytes + 3];
    const size_type n = pkt.getNrBytes();

    i = 0;
    pcBuf[i++] = kCodeResponse;
    pcBuf[i++] = n;
    util::copy(pkt.begin(), pkt.end(), &pcBuf[i]);
    i += n;
    pcBuf[i] = 0x00;

    hal::serial::println(reinterpret_cast<const char *>(pcBuf));
}

// ---------------------------------------------------
/// Arduino's loop() function.
// ---------------------------------------------------
void loop()
{
    static util::MilliTimer LedTimer;
    static util::MilliTimer DccTimer;

    // alive LED
    if (LedTimer.timeout())
    {
        toggleLedPin();
        LedTimer.start(kBlinkLedPeriod_ms);
        hal::serial::print(hal::micros());
        hal::serial::print(" isr=");
        hal::serial::print(dcc::decoder::get_instance().get_interrupt_count());
        hal::serial::print(" ones=");
        hal::serial::print(dcc::decoder::get_instance().get_ones_count());
        hal::serial::print(" zeros=");
        hal::serial::print(dcc::decoder::get_instance().get_zeros_count());
        hal::serial::print(" inv=");
        hal::serial::print(dcc::decoder::get_instance().get_invalids_count());
        hal::serial::print(" pkt=");
        hal::serial::println(dcc::decoder::get_instance().get_packet_count());
    }

    if (DccTimer.timeout())
    {
        dcc::decoder::get_instance().fetch();
        while (!dcc::decoder::get_instance().empty())
        {
            const packet_type &pkt = dcc::decoder::get_instance().front();
            // PrintAscii(pkt);
            PrintBin(pkt);
            dcc::decoder::get_instance().pop();
        }
        DccTimer.start(10);
    }
}