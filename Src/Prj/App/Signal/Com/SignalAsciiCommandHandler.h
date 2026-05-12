/**
 * @file Prj/App/Signal/Com/SignalAsciiCommandHandler.h
 *
 * @brief Signal project specific ASCII command handler.
 *
 * @copyright Copyright 2024 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SIGNALASCIICOMMANDHANDLER_H_
#define SIGNALASCIICOMMANDHANDLER_H_

#include <Com/IfcAsciiCommandHandler.h>

namespace com
{
    class SignalAsciiCommandHandler : public IfcAsciiCommandHandler
    {
    public:
        /**
         * @brief Process a project-specific command.
         * 
         * This function implements the processing of project-specific commands for the Signal 
         * application. It takes a command token, a stringstream containing the command parameters, 
         * and a string for the sub-response. The function returns a ret_type value indicating the 
         * result of the command processing, which can be kIfcOK for success, kIfcInvCmd for 
         * unrecognized command tokens, or a project-specific error code for other failures.
         * 
         * @param[in] cmd The command token to be processed
         * @param[in] st The stringstream containing the command parameters
         * @param[out] sub_response The string where the sub-response will be stored
         * @return kIfcOK if the command was processed successfully
         * @return kIfcInvCmd if the command token was not recognized
         * @return A project-specific error code for other failures (to be used with 
         *         @ref get_error_string())
         */
        ret_type process_command(const char *cmd, stringstream_type &st, string_type &sub_response) override;

        /**
         * @brief Get the error string object
         * 
         * The function retrieves the error string corresponding to the provided return code (ret) 
         * and stores it in the dst string object. It checks if the return code is within the valid 
         * range of project-specific error codes and reads the corresponding error string from ROM.
         * If the return code is invalid, it silently ignores it, as AsciiCom will handle kIfcOK 
         * and kIfcInvCmd itself. So the function will not fill OK or "ERR: Invalid command" 
         * strings, but only project-specific error strings.
         * 
         * @param[in] ret The return code for which the error string is requested
         * @param[out] dst The string object where the error string will be stored
         */
        void get_error_string(ret_type ret, string_type &dst) override;
    };
}

#endif // SIGNALASCIICOMMANDHANDLER_H_
