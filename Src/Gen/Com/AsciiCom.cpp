/**
 * @file AsciiCom.cpp
 *
 * @author Ralf Sondershaus
 *
 * @brief Implements ASCII command parsing and monitor output handling.
 *
 * @copyright Copyright 2024 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Compiler.h>
#include <Platform_Limits.h>
#include <Com/AsciiCom.h>
#include <Util/String_view.h>
#include <Util/Timer.h>
#include <Rte/Rte.h>
#include <Rte/Rte_Cfg_Cod.h>

namespace com
{
    using char_type = AsciiCom::char_type;
    using string_type = AsciiCom::string_type; // string of size 64
    using stringstream_type = util::basic_istringstream<SerAsciiTP::kMaxLenTelegram, char_type>;

    /// Return type shared with IfcAsciiCommandHandler.
    using ret_type = IfcAsciiCommandHandler::ret_type;

    /**
     * @brief Stores runtime state for RTE port monitoring output.
     */
    typedef struct
    {
        const rte::port_data_t *pPortData; ///< Pointer to the RTE data
        util::MilliTimer timer;            ///< Timer for next output
        uint16 unCycleTime;                ///< [ms] Cycle time for output
        uint16 unFirstIdx;                 ///< For array types: index of the first element
        uint16 unNrIdx;                    ///< For array types: number of elements to be transmitted
    } port_type;

    /**
     * @defgroup AsciiCom commands
     * @{
     */
    static const char generic_cmd_set_cv[]    ROM_CONST_VAR = "SET_CV";
    static const char generic_cmd_get_cv[]    ROM_CONST_VAR = "GET_CV";
    static const char generic_cmd_mon_list[]  ROM_CONST_VAR = "MON_LIST";
    static const char generic_cmd_mon_start[] ROM_CONST_VAR = "MON_START";
    static const char generic_cmd_mon_stop[]  ROM_CONST_VAR = "MON_STOP";

    using generic_handler_type = ret_type (*)(stringstream_type &, string_type &);

    static ret_type process_set_cv(stringstream_type &st, string_type &response);
    static ret_type process_get_cv(stringstream_type &st, string_type &response);
    static ret_type process_monitor_list(stringstream_type &st, string_type &response);
    static ret_type process_monitor_start(stringstream_type &st, string_type &response);
    static ret_type process_monitor_stop(stringstream_type &st, string_type &response);

    /**
     * @brief Maps a command token to its generic command handler.
     */
    struct command_entry_type
    {
        const char *cmd_ptr;
        generic_handler_type handler;
    };
    
    using command_table_type = util::array<command_entry_type, 5U>;

    static const command_table_type commands ROM_CONST_VAR =
        {{{generic_cmd_set_cv,    process_set_cv},
          {generic_cmd_get_cv,    process_get_cv},
          {generic_cmd_mon_list,  process_monitor_list},
          {generic_cmd_mon_start, process_monitor_start},
          {generic_cmd_mon_stop,  process_monitor_stop}}};
    /** 
     * @}
     */

    /**
     * @defgroup AsciiComReturnCodes AsciiCom return codes
     * 
     * Return values of process() function family.
     */
    
    /**
     * @ingroup AsciiComReturnCodes
     */
    static constexpr ret_type kOk                     = IfcAsciiCommandHandler::kIfcOK; ///< OK
    static constexpr ret_type kInvCmd                 = IfcAsciiCommandHandler::kIfcInvCmd; ///< Command invalid (or unknown)
    static constexpr ret_type kInvCvId                = IfcAsciiCommandHandler::kIfcProjectBase + 0; ///< SET_CV with an invalid CV id
    static constexpr ret_type kCvValueOutOfRange      = IfcAsciiCommandHandler::kIfcProjectBase + 1; ///< SET_CV with an invalid CV value
    static constexpr ret_type kInvMonitorStartParam   = IfcAsciiCommandHandler::kIfcProjectBase + 2; ///< MON_START parameters are malformed
    static constexpr ret_type kInvMonitorStartIfcName = IfcAsciiCommandHandler::kIfcProjectBase + 3; ///< MON_START interface name was not found

    /// For each ret_type, an error description that is transmitted after
    /// processing the command.
    const char ret_OK[] ROM_CONST_VAR = "OK";
    const char ret_INV_CMD[] ROM_CONST_VAR = "ERR: Invalid command";
    const char ret_INV_CV_ID[] ROM_CONST_VAR = "ERR: Invalid CV ID";
    const char ret_CV_VALUE_OUT_OF_RANGE[] ROM_CONST_VAR = "ERR: CV value is out of range";
    const char ret_INV_MONITOR_START_PARAM[] ROM_CONST_VAR = "ERR: Unknown monitor start parameter: MONITOR_START cycle-time ifc-name";
    const char ret_INV_MONITOR_START_IFC_NAME[] ROM_CONST_VAR = "ERR: Unknown monitor start interface name: MONITOR_START cycle-time ifc-name";
    const char ret_ERR_UNKNOWN[] ROM_CONST_VAR = "ERR: unknown error";

    static constexpr const string_type::value_type *responses[] ROM_CONST_VAR =
    {
        ret_OK,                             // kOk
        ret_INV_CMD,                        // kInvCmd
        ret_INV_CV_ID,                      // kInvCvId
        ret_CV_VALUE_OUT_OF_RANGE,          // kCvValueOutOfRange
        ret_INV_MONITOR_START_PARAM,        // kInvMonitorStartParam
        ret_INV_MONITOR_START_IFC_NAME,     // kInvMonitorStartIfcName
        ret_ERR_UNKNOWN                     // has to be the last element
    };
    constexpr size_t max_constexpr(size_t a) { return a; }

    template<typename... Args>
    constexpr size_t max_constexpr(size_t a, Args... rest)
    {
        return a > max_constexpr(rest...) ? a : max_constexpr(rest...);
    }

    static constexpr size_t kMaxResponseLen = max_constexpr(
        sizeof(ret_OK) - 1U,
        sizeof(ret_INV_CMD) - 1U,
        sizeof(ret_INV_CV_ID) - 1U,
        sizeof(ret_CV_VALUE_OUT_OF_RANGE) - 1U,
        sizeof(ret_INV_MONITOR_START_PARAM) - 1U,
        sizeof(ret_INV_MONITOR_START_IFC_NAME) - 1U,
        sizeof(ret_ERR_UNKNOWN) - 1U
    );

    static bool output_monitor_list(string_type &response);
    static bool output_port_data(port_type &pm, string_type &response);

    static bool doOutputPortList = false;
    static port_type portMonitor;

    /**
     * @brief Processes a newly received telegram and transmits a response.
     *
     * This callback is invoked by the observed transport layer.
     */
    void AsciiCom::update()
    {
        if (asciiTP)
        {
            const string_type &telegram = asciiTP->getTelegram();

            process(telegram, telegram_response);

            if (telegram_response.size() > 0)
            {
                asciiTP->transmitTelegram(telegram_response);
            }
        }
    }

    /**
     * @brief Parses and executes a single ASCII telegram.
     *
     * Generic commands are resolved first. Unknown commands are delegated to the
     * optional project-specific command handler.
     *
     * @param[in] telegram Input telegram including command token and parameters.
     * @param[out] response Response telegram text.
     */
    void AsciiCom::process(const string_type &telegram, string_type &response)
    {
        stringstream_type st(telegram);
        // buffer for command token, used for parsing incoming telegrams.
        char cmd[kMaxLenToken]; 
        // buffer for command token read from ROM, used for comparing with the parsed command token.
        char cmd_rom[kMaxLenToken]; 
        ret_type ret = kInvCmd;
        string_type sub_response;
        boolean found = false;
        size_type idx;

        st >> util::setw(static_cast<util::streamsize>(kMaxLenToken)) >> cmd;

        util::string_view sv(cmd);
        for (idx = 0U; idx < commands.size(); idx++)
        {
            command_entry_type entry;
            ROM_READ_STRUCT(&entry, &commands[idx], sizeof(command_entry_type));
            ROM_READ_STRING_N(cmd_rom, entry.cmd_ptr, kMaxLenToken);
            if (sv.compare(cmd_rom) == 0)
            {
                ret = entry.handler(st, sub_response);
                found = true;
                response.append(cmd_rom); // The response shall contain the command token
                break;
            }
        }

        if (!found && command_handler)
        {
            ret = command_handler->process_command(cmd, st, sub_response);
            if (IfcAsciiCommandHandler::is_project_specific_error(ret))
            {
                // Project-specific error: obtain string from the handler
                command_handler->get_error_string(ret, response);
            }
        }

        if (found || !IfcAsciiCommandHandler::is_project_specific_error(ret))
        {
            // ret is either a generic command return code or OK / Invalid command.
            // In both cases, the response string is composed of the status text plus the sub_response (if any).
            // Status text for error codes or "OK" for successful command execution, used for response composition.
            char response_text[response.max_size()];
            // First, read the pointer to the response text from ROM, then read the string itself 
            // from ROM to a RAM buffer, and finally assign it to the response.
            const char* response_text_ptr = static_cast<const char *>(ROM_READ_PTR(&responses[static_cast<size_type>(ret)]));
            ROM_READ_STRING_N(response_text, response_text_ptr, util::size(response_text));
            response = response_text;
        }

        if (sub_response.size() > 0U)
        {
            response.append(" ");
            response.append(sub_response);
        }
    }

    /**
     * @brief Executes cyclic monitor output handling.
     *
     * Sends pending MON_LIST output lines and periodic monitored port values.
     */
    void AsciiCom::cycle()
    {
        if (asciiTP)
        {
            if (doOutputPortList)
            {
                doOutputPortList = output_monitor_list(telegram_response);
                asciiTP->transmitTelegram(telegram_response);
            }

            if (portMonitor.pPortData)
            {
                if (output_port_data(portMonitor, telegram_response))
                {
                    asciiTP->transmitTelegram(telegram_response);
                }
            }
        }
    }

    /**
     * @brief Implements command SET_CV <cv_id> <value>
     *
      * @param[in] st Command stream after token "SET_CV".
      * @param[out] response Echo of command parameters for response composition.
      * @return kOk Valid command and value written to CV.
      * @return kInvCmd Ill-formed command parameters.
      * @return kCvValueOutOfRange Value is outside uint8 range.
     */
    static ret_type process_set_cv(stringstream_type &st, string_type &response)
    {
        ret_type ret = kInvCmd;
        uint16 value;
        CV new_cv;

        // The response shall contain the command parameters
        response.append(st.str());

        // Use uint16 here to ensure numeric values are extracted correctly.
        // If uint8 is used, the extraction may interpret the value as a character instead of a 
        // number.
        st >> new_cv.id;
        st >> value;
        // Do not check for eof() since eof() is true after extracting the last element
        // (and if the last element doesn't have trailing white spaces).
        if (!st.fail())
        {
            if (value < platform::numeric_limits<uint8>::max_())
            {
                new_cv.val = static_cast<uint8>(value);
                rte::set_cv(new_cv.id, new_cv.val);
                ret = kOk;
            }
            else
            {
                ret = kCvValueOutOfRange;
            }
        }

        return ret;
    }

    /**
     * @brief Implements command GET_CV <cv_id>
     *
      * @param[in] st Command stream after token "GET_CV".
      * @param[out] response Echo of command parameters plus CV value on success.
      * @return kOk CV id is valid and value appended to response.
      * @return kInvCmd Ill-formed command parameters.
      * @return kInvCvId CV id is not valid.
     */
    static ret_type process_get_cv(stringstream_type &st, string_type &response)
    {
        ret_type ret = kInvCmd;
        CV cv;

        // The response shall contain the command parameters
        response.append(st.str());

        // Use uint16 here to ensure numeric values are extracted correctly.
        // If uint8 is used, the extraction may interpret the value as a character instead of a 
        // number.
        st >> cv.id;
        // Do not check for eof() since eof() is true after extracting the last element
        // (and if the last element doesn't have trailing white spaces).
        if (!st.fail())
        {
            if (rte::is_cv_id_valid(cv.id))
            {
                cv.val = rte::get_cv(cv.id);
                util::basic_string<4, char> tmp;
                util::to_string(static_cast<int>(cv.val), tmp);
                response.append(" ");
                response.append(tmp);
                ret = kOk;
            }
            else
            {
                ret = kInvCvId;
            }
        }

        return ret;
    }

    /**
     * @brief Implements the command MON_LIST
     *
     * The cyclic process of printing is enabled. The output itself is done by
     * @ref output_monitor_list.
     *
    * @param[in] st Command stream after token "MON_LIST" (unused).
    * @param[out] response Initial response containing number of registered ports.
    * @return kOk Command accepted and list output enabled.
     */
    static ret_type process_monitor_list(stringstream_type &st, string_type &response)
    {
        util::basic_string<4, char> tmp;
        (void)st;
        util::to_string(rte::getNrPorts(), tmp);
        response.append("number of ports=").append(tmp);
        doOutputPortList = true;
        return kOk;
    }

    /**
     * @brief Outputs the next available RTE port name and index to the response string.
     *
     * This function iterates through the list of RTE ports and appends the current port's
     * index and name to the provided response string. It maintains internal state across
     * calls using a static index, allowing sequential access to all ports.
     *
     * Once all ports have been listed, the index resets to 0 and the function returns false.
     * Otherwise, it returns true to indicate that more ports remain to be listed.
     *
     * @param[out] response A reference to a string that will be populated with the current RTE
     *                 port's index and name in the format "index : name"
     * @return true: continue to next list element
     * @return false: stop, end of list
     */
    static bool output_monitor_list(string_type &response)
    {
        static size_t outputPortListIdx = 0;
        util::basic_string<4, char> tmp;
        bool ret;

        if (outputPortListIdx < rte::getNrPorts())
        {
            util::to_string(outputPortListIdx, tmp);
            response.clear();
            response.append(tmp);
            response.append(" : ");
            response.append(rte::getPortData(outputPortListIdx)->szName);
            outputPortListIdx++;
        }
        if (outputPortListIdx < rte::getNrPorts())
        {
            ret = true;
        }
        else
        {
            outputPortListIdx = 0;
            ret = false;
        }
        return ret;
    }

    /**
     * @brief Outputs formatted port monitoring data to the response string if the timer has expired.
     *
     * This function checks whether the monitoring timer associated with the given port has timed 
     * out. If so, it increments the timer, formats the current timestamp, port name, and a 
     * sequence of data values from the RTE port's buffer, and appends them to the provided 
     * response string.
     *
     * The data values are interpreted based on the element size of the port data (uint8, uint16, 
     * or uint32). If the timer has not expired, the function returns false and does not modify the 
     * response.
     *
     * @param[in,out] pm Reference to the RTE port monitoring structure containing timer and data buffer 
     *           information.
     * @param[out] response A string that will be populated with formatted output data if 
     *                 applicable.
     *
     * @return true if the timer expired and data was written to the response; false otherwise.
     */
    static bool output_port_data(port_type &pm, string_type &response)
    {
        bool ret;
        size_t i;
        util::basic_string<11, char> tmp;

        if (pm.timer.timeout())
        {
            pm.timer.increment(portMonitor.unCycleTime);
            response.clear();
            util::to_string(hal::micros(), tmp);
            response.append("[").append(tmp).append(" us] ");
            response.append(pm.pPortData->szName);
            response.append(":");
            for (i = portMonitor.unFirstIdx; i < portMonitor.unFirstIdx + portMonitor.unNrIdx; i++)
            {
                switch (portMonitor.pPortData->size_of_element)
                {
                case sizeof(uint8):
                    util::to_string(static_cast<uint8 *>(pm.pPortData->pData)[i], tmp);
                    break;
                case sizeof(uint16):
                    util::to_string(static_cast<uint16 *>(pm.pPortData->pData)[i], tmp);
                    break;
                case sizeof(uint32):
                    util::to_string(static_cast<uint32 *>(pm.pPortData->pData)[i], tmp);
                    break;
                default:
                    util::to_string(portMonitor.pPortData->size_of_element, tmp);
                    break;
                }
                response.append(" ").append(tmp);
            }
            ret = true;
        }
        else
        {
            ret = false;
        }
        return ret;
    }

    /**
     * @brief Implements command MON_START <cycle-time> <ifc-name> [first-idx nr-idx].
     *
     * Starts periodic monitoring output for the selected RTE interface.
     *
     * @param[in] st Command stream after token "MON_START".
     * @param[out] response Response containing the selected interface name on success.
     * @return kOk Monitoring started.
     * @return kInvMonitorStartIfcName Interface name is unknown.
     * @return kInvMonitorStartParam Parameters are malformed.
     */
    static ret_type process_monitor_start(stringstream_type &st, string_type &response)
    {
        char ifc_name[32];
        uint16 unCycleTime;
        uint16 unFirstIdx;
        uint16 unNrIdx;
        ret_type ret;
        st >> unCycleTime >> ifc_name;
        if (!st.fail())
        {
            const rte::port_data_t *pPortData = rte::getPortData(ifc_name);
            if (pPortData)
            {
                response.append(pPortData->szName);
                portMonitor.pPortData = pPortData;
                portMonitor.unCycleTime = unCycleTime;
                portMonitor.timer.start(unCycleTime);
                portMonitor.unFirstIdx = 0;
                portMonitor.unNrIdx = pPortData->size;
                st >> unFirstIdx;
                if (!st.fail() && (unFirstIdx < pPortData->size))
                {
                    portMonitor.unFirstIdx = unFirstIdx;
                    st >> unNrIdx;
                    if (!st.fail() && (unFirstIdx + unNrIdx < pPortData->size))
                    {
                        portMonitor.unNrIdx = unNrIdx;
                    }
                    else
                    {
                        portMonitor.unNrIdx = pPortData->size - unFirstIdx;
                    }
                }
                ret = kOk;
            }
            else
            {
                ret = kInvMonitorStartIfcName;
            }
        }
        else
        {
            ret = kInvMonitorStartParam;
        }
        return ret;
    }

    /**
     * @brief Implements command MON_STOP. 
     * 
     * The command stops the periodic monitoring output started by MON_START and clears the 
     * monitoring state.
     *
     * @param[in] st Command stream after token "MON_STOP" (unused).
     * @param[out] response Response text (unused).
     * @return kOk Monitor stopped.
     */
    static ret_type process_monitor_stop(stringstream_type &st, string_type &response)
    {
        (void)st;
        (void)response;

        portMonitor.pPortData = nullptr;

        return kOk;
    }

} // namespace com
