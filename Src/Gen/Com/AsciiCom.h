/**
 * @file AsciiCom.h
 *
 * @author Ralf Sondershaus
 *
 * @brief ASCII telegram communication interface for generic COM commands.
 *
 * The AsciiCom class receives telegrams from SerAsciiTP, handles generic commands,
 * and can delegate unknown commands to an optional project-specific handler.
 *
 * ## Supported Generic Commands
 * - `SET_CV cv_id value`
 * - `GET_CV cv_id`
 * - `MON_LIST`
 * - `MON_START cycle-time ifc-name [id-first id-nr]`
 * - `MON_STOP`
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
    * @brief Receives and processes ASCII telegrams.
     *
     * This class listens to a SerAsciiTP instance and processes incoming telegrams
     * related to generic commands. Project-specific command sets can be integrated
     * via @ref set_command_handler.
     *
     * For communication, it uses ASCII-formatted telegrams where commands and parameters
     * are separated by spaces. The class handles generic commands such as `SET_CV`,
     * `GET_CV`, `MON_LIST`, `MON_START`, and `MON_STOP`.
     * Unknown commands can be delegated to a project-specific handler.
     * It generates appropriate responses based on the processed commands. See readme documentation
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
        util::ptr<IfcAsciiCommandHandler> command_handler; ///< Optional handler for project-specific commands.
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
         * @param[in] tp Reference to the SerAsciiTP object to listen to.
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
         * @param[in] handler Project-specific command handler.
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
         * @param[in] telegram The input string containing the telegram to be processed.
         * @param[out] response Reference to a string where the generated response will be stored.
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