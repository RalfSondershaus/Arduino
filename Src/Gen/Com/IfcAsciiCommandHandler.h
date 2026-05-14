/**
 * @file IfcAsciiCommandHandler.h
 *
 * @brief Interface for project-specific ASCII command handler extensions.
 *
 * @copyright Copyright 2026 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IFCASCIICOMMANDHANDLER_H_
#define IFCASCIICOMMANDHANDLER_H_

#include <Com/SerAsciiTP.h>
#include <Util/Sstream.h>

namespace com
{
    /**
     * @brief Abstract interface for project-specific ASCII command extensions.
     *
     * AsciiCom handles a fixed set of generic commands (SET_CV, GET_CV, MON_LIST,
     * MON_START, MON_STOP). For application-specific commands (e.g. SET_SIGNAL),
     * a concrete subclass is registered via AsciiCom::set_command_handler().
     *
     * AsciiCom calls @ref process_command() for every unrecognised token. The
     * implementation dispatches to a project command table and returns one of:
     * - @ref kIfcOK   — AsciiCom prepends "OK" to the sub-response.
     * - @ref kIfcInvCmd — AsciiCom prepends "ERR: Invalid command".
     * - A project-specific code >= @ref kIfcProjectBase — AsciiCom calls
     *   @ref get_error_string() to obtain the error prefix.
     *
     * @note No virtual destructor is provided because AVR-GCC requires
     *       `operator delete` for virtual destructors. Objects of this class
     *       are never destroyed at run-time (stack-allocated or static).
     *
     * @see AsciiCom
     */
    class IfcAsciiCommandHandler
    {
    public:
        using char_type = SerAsciiTP::telegram_base_type;                                        ///< Character type used for telegrams
        using string_type = SerAsciiTP::string_type;                                              ///< Fixed-capacity string type for command and response text
        using stringstream_type = util::basic_istringstream<SerAsciiTP::kMaxLenTelegram, char_type>; ///< Input stream for parsing command parameters

        /// @brief Maximum length of a command token, used for parsing incoming telegrams.
        static constexpr size_t kMaxLenToken = 20U;

        /**
         * @brief Return type for @ref process_command().
         *
         * The value space is partitioned as follows:
         * - @ref kIfcOK (0): success.
         * - @ref kIfcInvCmd (1): command token not recognised.
         * - >= @ref kIfcProjectBase (2): project-specific error; index into the
         *   project's own error-string table.
         */
        using ret_type = sint8;

        /**
         * @defgroup IfcAsciiCommandHandlerReturnCodes IfcAsciiCommandHandler return codes
         * 
         * Return values of @ref process_command() function.
         * 
         * @{
         */
        static constexpr ret_type kIfcOK          = 0; ///< Command succeeded (maps to index 0 in AsciiCom's response table)
        static constexpr ret_type kIfcInvCmd      = 1; ///< Command token not recognised (maps to index 1 in AsciiCom's response table)
        static constexpr ret_type kIfcProjectBase = 2; ///< First project-specific error code; maps to index 0 in the project's response table

        /**
         * @brief Returns true if @p ret is a project-specific error code.
         *
         * @param[in] ret Return value from @ref process_command().
         * @return true  @p ret >= @ref kIfcProjectBase — project handler must supply the error string.
         * @return false @p ret is @ref kIfcOK or @ref kIfcInvCmd — AsciiCom owns the response text.
         */
        static bool is_project_specific_error(ret_type ret) { return ret >= kIfcProjectBase; }
        /**
         * @}
         */

        /**
         * @brief Destroy the command handler.
         * 
         * The destructor should be virtual to allow proper cleanup of derived classes.
         * BUT the AVR GCC compiler throws 'undefined reference to `operator delete(void*, unsigned 
         * int)' when using virtual destructors. Since dynamic memory allocation is not used and 
         * objects are destructed at shut down only (-> never), we can safely avoid virtual 
         * destructors in this case.
         */
        ~IfcAsciiCommandHandler() = default;

        /**
         * @brief Dispatch a project-specific command token and fill the data sub-response.
         *
         * AsciiCom calls this function when it cannot match @p cmd against its own command
         * table. The implementation searches a project-local command table, invokes the
         * matching handler, and returns a status code.
         *
         * The @p sub_response receives only the *data* portion of the reply (parameter
         * echo, read-back values). The status prefix ("OK" / error text) is prepended
         * by AsciiCom after this call returns.
         *
         * @param[in]  cmd          Null-terminated command token (e.g. "SET_SIGNAL"),
         *                          already extracted from the telegram stream.
         * @param[in,out] st        Telegram stream positioned immediately after @p cmd;
         *                          the handler reads its parameters from here.
         * @param[out] sub_response Receives the data portion of the response on success.
         * @return kIfcOK      Command recognised and executed successfully.
         * @return kIfcInvCmd  Command token not found in the project command table.
         * @return >= kIfcProjectBase  Project-specific error; AsciiCom calls @ref get_error_string().
         */
        virtual ret_type process_command(const char *cmd, stringstream_type &st, string_type &sub_response) = 0;

        /**
         * @brief Populate @p dst with the human-readable error text for @p ret.
         *
         * Called by AsciiCom only when @ref process_command() returned a value
         * >= @ref kIfcProjectBase. The implementation indexes the project's own
         * PROGMEM error-string table using `ret - kIfcProjectBase`.
         *
         * If @p ret is outside the valid project range the implementation must
         * leave @p dst unchanged (AsciiCom handles @ref kIfcOK and @ref kIfcInvCmd
         * itself).
         *
         * @param[in]  ret  Project-specific error code (>= @ref kIfcProjectBase)
         *                  returned by @ref process_command().
         * @param[out] dst  Receives the error text (e.g. "ERR: Invalid signal index").
         */
        virtual void get_error_string(ret_type ret, string_type &dst) = 0;
    };
}

#endif // IFCASCIICOMMANDHANDLER_H_
