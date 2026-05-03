/**
 * @file Gen/Com/AsciiCom.h
 *
 * @brief Handles ASCII-based telegram communication.
 *
 * The AsciiCom class is responsible for receiving,
 * processing, and responding to ASCII telegrams.
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
 * Project-specific commands are delegated to an optional
 * @ref IfcAsciiCommandHandler implementation.
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
 * ## Usage Examples
 *
 * ### Configure signal using CV commands
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
 * @note Parts of the documentation of this file was created by GitHub Copilot.
 *
 * @copyright Copyright 2024-2025 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ASCIICOM_H_
#define ASCIICOM_H_

#include <Std_Types.h>
#include <Com/IfcAsciiCommandHandler.h>
#include <Com/Observer.h>
#include <Com/SerAsciiTP.h>
#include <Util/Array.h>

namespace com
{
    /**
     * Receives and processes ASCII telegrams.
     *
     * This class listens to a SerAsciiTP instance and processes incoming telegrams
     * related to generic commands. Project-specific command sets can be integrated
     * via @ref set_command_handler.
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
        util::ptr<IfcAsciiCommandHandler> command_handler;
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
         * @brief Register a handler for project-specific commands.
         *
         * The generic command set is still handled by AsciiCom itself.
         * Unknown commands are delegated to the registered handler.
         *
         * @param handler Project-specific command handler.
         */
        void set_command_handler(IfcAsciiCommandHandler &handler)
        {
            command_handler = &handler;
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