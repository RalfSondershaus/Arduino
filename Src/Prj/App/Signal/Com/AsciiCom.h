/**
 * @file Signal/Com/AsciiCom.h
 *
 * @brief Handles ASCII-based telegram communication for signal control.
 *
 * The AsciiCom class is responsible for receiving,
 * processing, and responding to ASCII telegrams related to signal configuration
 * and status queries. The supported telegrams allow setting and getting signal
 * aspects, blinks, targets, input classification, and change-over times, as well
 * as copying signal configurations between IDs.
 *
 * Capital letters and lower letters can be used. Capital letters are used here to support
 * readability.
 *
 * Set CV
 * - `SET_CV cv_id value` Set the CV with id cv_id to the given value
 *
 * Monitor RTE
 * - `MON_LIST` Print available RTE ports 
 * - `MON_START cycle-time ifc-name [id-first id-nr]` Start to print current values of `ifc-name`
 * - `MON_STOP` Stop to print RTE port
 * 
 * Set to defaults
 * - `INIT` Write default values to NVM
 * 
 * Example to configure signal 1 as Ausfahrsignal
 * - `SET_CV 42 1`            Assign Ausfahrsignal (1) to signal 1 (42)
 * - `SET_CV 50 0x0D`         First output pin of signal 1 is internal (0) pin 13 (D)
 * - `SET_CV 58 0x40`         Input is ADC (4) at A0 (0)
 * - `SET_CV 66 0`            Classifier type is type 1 (0)
 * 
 * Example to configure signal 1 as Ausfahrsignal
 * - `MON_LIST`
 * - `MON_START 100 ifc_ad_values`
 * - `MON_STOP`
 * 
 * Ideas for future add-ons:
 *
 * Set signal id (Ausfahrsignal, Blocksignal, ...)
 * - SET_SIGNAL <signal_pos> ID <signal_id>
 * with 1 <= signal_pos <= @ref cfg::kNrSignals
 *
 * Set first output pin for a signal
 * - SET_SIGNAL <signal_pos> FIRST_OUPUT ONBOARD <pin_nr>
 * - SET_SIGNAL <signal_pos> FIRST_OUPUT EXTERNAL <pin_nr>
 *
 * Set source of commands for a signal
 * - SET_SIGNAL <signal_pos> INPUT ADC <pin_nr>
 * - SET_SIGNAL <signal_pos> INPUT DCC
 * - SET_SIGNAL <signal_pos> INPUT DIG <first_pin_nr> (not implemented yet)
 *
 * Define a user-defined signal
 * - SET_USER_DEFINED_SIGNAL <signal_id> ASPECTS 11000 00100 00000 00000 00000
 * - SET_USER_DEFINED_SIGNAL <signal_id> BLINKS 11000 00100 00000 00000 00000
 * - SET_USER_DEFINED_SIGNAL <signal_id> COT 10 20
 * with 128 <= signal_id <= @ref cfg::kNrUserDefinedSignals
 * The number of aspects is defined by the number of elements in the ASPECTS / BLINKS command.
 * Per signal, a maximum of 8 aspects is supported.
 * The number of LEDs is defined by the number of bits in an element (such as 11000) in the
 * ASPECTS / BLINKS command.
 * Per signal, a maximum of 8 LEDs is supported.
 *
 * Example telegrams can look like:
 * - SET_SIGNAL 1 ID 0      Assign Ausfahrsignal to signal 1
 * - SET_SIGNAL 1 ID 128    Assign first user-defined signal id to signal 1
 * - SET_SIGNAL 1 FIRST_OUTPUT ONBOARD 13  First pin is onboard pin 13
 * - SET_SIGNAL 1 INPUT ADC 54             Input is calculated from ADC input pin 54
 * - SET_SIGNAL 1 INPUT ADC A0
 * - SET_SIGNAL 1 INPUT DCC
 * - SET_USER_DEFINED_SIGNAL 128 ASPECTS 11000 00100 00000 00000 00000
 * 
 * @note Parts of the documentation of this file was created by GitHub Copilot.
 *
 * @copyright Copyright 2024-2025 Ralf Sondershaus
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
 * See <https://www.gnu.org/licenses/>.
 */

#ifndef ASCIICOM_H_
#define ASCIICOM_H_

#include <Std_Types.h>
#include <Com/Observer.h>
#include <Com/SerAsciiTP.h>
#include <Util/Array.h>

namespace com
{
    /**
     * Receives and processes ASCII telegrams for signal control.
     *
     * This class listens to a SerAsciiTP instance and processes incoming telegrams
     * related to signal configuration and status queries. It supports commands for
     * setting and getting signal aspects, blinks, targets, input classification,
     * and change-over times, as well as configuring classifiers.
     *
     * For communication, it uses ASCII-formatted telegrams where commands and parameters
     * are separated by spaces. The class can handle various commands such as `SET_CV`,
     * `MON_LIST`, `MON_START`, `MON_STOP`, and `INIT`. It generates appropriate responses
     * based on the processed commands. See the readme.md documentation for telegram descriptions 
     * and usage examples.
     */
    class AsciiCom : public Observer
    {
    public:
        using char_type = SerAsciiTP::telegram_base_type;
        using string_type = SerAsciiTP::string_type;
        using size_type = string_type::size_type;

    protected:
        /**
         * @brief Smart pointer to a SerAsciiTP object.
         *
         * Currently, just one observer is supported, so this pointer
         * is used to listen to the SerAsciiTP instance for incoming telegrams.
         */
        util::ptr<SerAsciiTP> asciiTP;
        /**
         * @brief Stores the response telegram as a string.
         *
         * This variable holds the response message received or to be sent
         * in ASCII communication. The type string_type is an alias
         * for util::basic_string class.
         * @note The length of this string is limited to kMaxLenTelegram.
         * @see SerAsciiTP::kMaxLenTelegram
         */
        string_type telegram_response;

    public:
        /**
         * @brief Default constructor for the AsciiCom class.
         *
         * Initializes a new instance of the AsciiCom class with default values.
         */
        AsciiCom() = default;

        /**
         * @brief Processes data for the ASCII communication interface.
         *
         * This method is called by AsciiTP to notify this observer of new data.
         *
         * @note This function overrides a virtual method from the base class.
         */
        void update() override;

        /**
         * @brief Attaches this object as a listener to the specified SerAsciiTP instance.
         *
         * This method sets the internal pointer to the provided SerAsciiTP object and
         * registers this object as an observer by calling the attach method on the SerAsciiTP instance.
         *
         * @param tp Reference to the SerAsciiTP object to listen to.
         */
        void listen_to(SerAsciiTP &tp)
        {
            asciiTP = &tp;
            tp.attach(*this);
        }

        /**
         * @brief Processes the given telegram and generates a response.
         *
         * This function takes an input telegram, performs the necessary processing,
         * and writes the result to the response parameter.
         *
         * @param telegram The input string containing the telegram to be processed.
         * @param response Reference to a string where the generated response will be stored.
         */
        void process(const string_type &telegram, string_type &response);

        /**
         * @brief Executes a single processing cycle for the ASCII communication interface.
         *
         * This method should be called periodically to handle communication tasks such as
         * receiving, parsing, and transmitting ASCII-formatted data. Typical actions performed
         * during the cycle include transmitting RTE monitoring values or outputting long lists.
         */
        void cycle();
    };
} // namespace com

#endif // ASCIICOM_H_