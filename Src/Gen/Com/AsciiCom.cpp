/**
 * @file Gen/Com/AsciiCom.cpp
 *
 * @brief
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

    /// To monitor a RTE port
    typedef struct
    {
        const rte::port_data_t *pPortData; ///< Pointer to the RTE data
        util::MilliTimer timer;            ///< Timer for next output
        uint16 unCycleTime;                ///< [ms] Cycle time for output
        uint16 unFirstIdx;                 ///< For array types: index of the first element
        uint16 unNrIdx;                    ///< For array types: number of elements to be transmitted
    } port_type;

    /// Return values of process() function family.
    /// An unscoped enum is used to simplify access to enumerator-list elements.
    enum ret_type
    {
        eOK = 0,     ///< OK
        eINV_CMD,    ///< Command invalid (or unknown)
        eINV_CV_ID,  ///< SET_CV with an invalid CV id
        eCV_VALUE_OUT_OF_RANGE, ///< SET_CV with an invalid CV value
        eINV_MONITOR_START_PARAM,
        eINV_MONITOR_START_IFC_NAME,
        eERR_UNKNOWN
    };

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
            ret_OK,                             // eOK
            ret_INV_CMD,                        // eINV_CMD
            ret_INV_CV_ID,                      // eINV_CV_ID
            ret_CV_VALUE_OUT_OF_RANGE,          // eCV_VALUE_OUT_OF_RANGE
            ret_INV_MONITOR_START_PARAM,        // eINV_MONITOR_START_PARAM
            ret_INV_MONITOR_START_IFC_NAME,     // eINV_MONITOR_START_IFC_NAME
            ret_ERR_UNKNOWN                     // has to be the last element
    };

    static ret_type process_set_cv(stringstream_type &st, string_type &response);
    static ret_type process_get_cv(stringstream_type &st, string_type &response);
    static ret_type process_monitor_list(stringstream_type &st, string_type &response);
    static ret_type process_monitor_start(stringstream_type &st, string_type &response);
    static ret_type process_monitor_stop(stringstream_type &st, string_type &response);

    static bool output_monitor_list(string_type &response);
    static bool output_port_data(port_type &pm, string_type &response);

    static bool doOutputPortList = false;
    static port_type portMonitor;

    // -----------------------------------------------------------------------------------
    /// A new telegram has been received, process it.
    // -----------------------------------------------------------------------------------
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

    // -----------------------------------------------------------------------------------
    ///
    // -----------------------------------------------------------------------------------
    void AsciiCom::process(const string_type &telegram, string_type &response)
    {
        static constexpr size_type kMaxLenToken = 20U;

        using generic_handler_type = ret_type (*)(stringstream_type &, string_type &);
        struct command_entry_type
        {
            const char *cmd;
            generic_handler_type handler;
        };

        static const char generic_cmd_set_cv[]    ROM_CONST_VAR = "SET_CV";
        static const char generic_cmd_get_cv[]    ROM_CONST_VAR = "GET_CV";
        static const char generic_cmd_mon_list[]  ROM_CONST_VAR = "MON_LIST";
        static const char generic_cmd_mon_start[] ROM_CONST_VAR = "MON_START";
        static const char generic_cmd_mon_stop[]  ROM_CONST_VAR = "MON_STOP";

        using command_table_type = util::array<command_entry_type, 5U>;
        static const command_table_type commands ROM_CONST_VAR =
            {{{generic_cmd_set_cv,    process_set_cv},
              {generic_cmd_get_cv,    process_get_cv},
              {generic_cmd_mon_list,  process_monitor_list},
              {generic_cmd_mon_start, process_monitor_start},
              {generic_cmd_mon_stop,  process_monitor_stop}}};

        stringstream_type st(telegram);
        char cmd[kMaxLenToken];
        char cmd_rom[kMaxLenToken];
        ret_type ret = eINV_CMD;
        string_type sub_response;
        boolean found = false;
        size_type idx;

        st >> util::setw(static_cast<util::streamsize>(kMaxLenToken)) >> cmd;

        util::string_view sv(cmd);
        for (idx = 0U; idx < commands.size(); idx++)
        {
            command_entry_type entry;
            ROM_READ_STRUCT(&entry, &commands[idx], sizeof(command_entry_type));
            ROM_READ_STRING(cmd_rom, entry.cmd);
            if (sv.compare(cmd_rom) == 0)
            {
                ret = entry.handler(st, sub_response);
                found = true;
                break;
            }
        }

        if (!found)
        {
            if (command_handler && command_handler->process_command(cmd, st, response))
            {
                // response fully assembled by project handler
                return;
            }
            ret = eINV_CMD;
        }

        // Assemble response: status text + optional sub-response
        const char *response_text = static_cast<const char *>(ROM_READ_PTR(&responses[static_cast<size_type>(ret)]));
        response = response_text;
        if (sub_response.size() > 0U)
        {
            response.append(" ");
            response.append(sub_response);
        }
    }

    // -----------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------
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
     * @param st [in] Contains the command string, get pointer points to first element after "SET_CV".
     * @param response [out] The response is stored here, it contains the command parameters.
     * @return ret_type eOK
     * @return ret_type eINV_CMD Ill-formed command or CV id is out-of-bounds
     * @return ret_type eCV_VALUE_OUT_OF_RANGE CV value is out-of-bounds
     * 
     */
    static ret_type process_set_cv(stringstream_type &st, string_type &response)
    {
        ret_type ret = eINV_CMD;
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
                ret = eOK;
            }
            else
            {
                ret = eCV_VALUE_OUT_OF_RANGE;
            }
        }

        return ret;
    }

    /**
     * @brief Implements command GET_CV <cv_id>
     *
     * @param st Contains the command string, get pointer points to first element after "GET_CV".
     * @param response [out] The response is stored here, it contains the command parameters.
     * @return ret_type eOK
     * @return ret_type eINV_CMD Ill-formed command or CV id is out-of-bounds
     * @return ret_type eCV_VALUE_OUT_OF_RANGE CV value is out-of-bounds
     * 
     */
    static ret_type process_get_cv(stringstream_type &st, string_type &response)
    {
        ret_type ret = eINV_CMD;
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
                ret = eOK;
            }
            else
            {
                ret = eINV_CV_ID;
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
     * @param st Contains the command string without "MON_LIST"
     * @param response [out] The response is stored here, it contains the number of RTE ports.
     * @return ret_type eOK
     */
    static ret_type process_monitor_list(stringstream_type &st, string_type &response)
    {
        util::basic_string<4, char> tmp;
        (void)st;
        util::to_string(rte::getNrPorts(), tmp);
        response.append("number of ports=").append(tmp);
        doOutputPortList = true;
        return eOK;
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
     * @param response [out] A reference to a string that will be populated with the current RTE
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
     * @param pm Reference to the RTE port monitoring structure containing timer and data buffer 
     *           information.
     * @param response Reference to a string that will be populated with formatted output data if 
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

    // -----------------------------------------------------------------------------------
    /// <MON_START> cycle-time ifc-name [first-idx nr-idx]
    ///
    /// @return eOK, eINV_MONITOR_START_IFC_NAME, eINV_MONITOR_START_PARAM
    // -----------------------------------------------------------------------------------
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
                ret = eOK;
            }
            else
            {
                ret = eINV_MONITOR_START_IFC_NAME;
            }
        }
        else
        {
            ret = eINV_MONITOR_START_PARAM;
        }
        return ret;
    }

    // -----------------------------------------------------------------------------------
    /// Stop the monitor
    // -----------------------------------------------------------------------------------
    static ret_type process_monitor_stop(stringstream_type &st, string_type &response)
    {
        (void)st;
        (void)response;

        portMonitor.pPortData = nullptr;

        return eOK;
    }

} // namespace com
