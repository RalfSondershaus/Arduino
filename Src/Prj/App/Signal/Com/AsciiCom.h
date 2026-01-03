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
 * ## Supported Commands
 *
 * ### Configuration Variables (CV)
 * - `SET_CV cv_id value` - Set the CV with id cv_id to the given value
 * - `GET_CV cv_id` - Get the value of the CV with id cv_id
 *
 * ### Signal Configuration
 * - `SET_SIGNAL idx id [ONB,EXT] output_pin step_size [ADC,DIG,DCC] input_pin` - Configure signal at position idx
 *   - `idx` - Signal index (0 to @ref cfg::kNrSignals - 1)
 *   - `id` - Signal type ID (built-in or user-defined)
 *   - `ONB` or `EXT` - Output type (onboard or external)
 *   - `output_pin` - First output pin number
 *   - `step_size` - Step size for output pins (-2, -1, 1, or 2; negative values indicate inverse order)
 *   - `ADC`, `DIG`, or `DCC` - Input type (analog, digital, or DCC)
 *   - `input_pin` - Input pin number
 * - `GET_SIGNAL idx` - Get the configuration of signal at position idx
 *
 * ### ETO Signal Control
 * - `ETO_SET_SIGNAL signal_idx aspect [dim_time_10ms]` - Enable/disable ETO signal aspect
 *   - `aspect` - Aspect value (0 to disable, non-zero to enable)
 *   - `dim_time_10ms` - Optional dimming time in units of 10 ms (default: 10 = 100 ms)
 *
 * ### RTE Port Monitoring
 * - `MON_LIST` - Print available RTE ports 
 * - `MON_START cycle-time ifc-name [id-first id-nr]` - Start to print current values of `ifc-name`
 *   - `cycle-time` - Update interval in milliseconds
 *   - `ifc-name` - Name of the RTE interface to monitor
 *   - `id-first` - Optional: first element index for array types
 *   - `id-nr` - Optional: number of elements to transmit for array types
 * - `MON_STOP` - Stop monitoring RTE port
 * 
 * ### System Commands
 * - `INIT` - Write default values to NVM
 * - `SET_VERBOSE level` - Set verbosity level (0 to 3)
 * - `GET_PIN_CONFIG pin` - Get the configuration of the specified pin (INPUT or OUTPUT)
 * 
 * ## Usage Examples
 *
 * ### Configure signal 0 using SET_SIGNAL
 * ```
 * SET_SIGNAL 0 1 ONB 10 -1 ADC 54
 * ```
 * This configures signal at index 0 to:
 * - Signal type ID 1 (Ausfahrsignal)
 * - Onboard output starting at pin 10
 * - Step size -1 (inverse order, decrementing pin numbers)
 * - ADC input from pin 54 (A0)
 *
 * ### Configure signal using CV commands (alternative method)
 * ```
 * SET_CV 42 1      # Assign Ausfahrsignal (1) to signal 1
 * SET_CV 50 0x0D   # First output pin of signal 1 is onboard (0) pin 13 (D)
 * SET_CV 58 0x40   # Input is ADC (4) at A0 (0)
 * SET_CV 66 0      # Classifier type is type 1 (0)
 * ```
 * 
 * ### Monitor RTE interface
 * ```
 * MON_LIST                      # List all available RTE ports
 * MON_START 100 ifc_ad_values   # Monitor ifc_ad_values every 100 ms
 * MON_STOP                      # Stop monitoring
 * ```
 *
 * ### ETO Signal Control
 * ```
 * ETO_SET_SIGNAL 0 5 20   # Enable ETO for signal 0, aspect 5, dim time 200 ms
 * ETO_SET_SIGNAL 0 0      # Disable ETO for signal 0
 * ```
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