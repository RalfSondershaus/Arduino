/**
 * @file Signal/Com/AsciiCom.cpp
 *
 * @brief
 *
 * @copyright Copyright 2024 Ralf Sondershaus
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

#include <Cfg_Prj.h>
#include <Platform_Limits.h>
#include <Cal/CalM_Type.h>
#include <Com/AsciiCom.h>
#include <Util/Sstream.h>
#include <Util/String_view.h>
#include <Util/Timer.h>
#include <Rte/Rte.h>

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

    /// Return values of process() function family
    typedef enum
    {
        eOK = 0,     ///< OK
        eINV_CMD,    ///< Command invalid (or unknown)
        eERR_EEPROM, ///< EEPROM update failure
        eINV_CV_ID,  ///< SET_CV with an invalid CV id
        eCV_VALUE_OUT_OF_RANGE, ///< SET_CV with an invalid CV value
        // eINV_SIGNAL_ID,             ///< SIGNAL ID invalid
        // eINV_SIGNAL_CMD,
        // eINV_SIGNAL_ASPECTS,
        // eINV_SIGNAL_BLINKS,
        // eINV_SIGNAL_TARGETS,
        // eINV_SIGNAL_INPUT,
        // eINV_SIGNAL_CHANGEOVERTIME,
        // eINV_SIGNAL_DB_AUSFAHRSIGNAL,
        // eINV_SIGNAL_DB_BLOCKSIGNAL,
        // eINV_CLASSIFIER_CMD,
        // eINV_CLASSIFIER_PIN,
        // eINV_CLASSIFIER_LIMITS,
        // eINV_CLASSIFIER_DEBOUNCE,
        eINV_MONITOR_START_PARAM,
        eINV_MONITOR_START_IFC_NAME,
        eERR_UNKNOWN
    } tRetType;

    /// For each tRetType, an error description that is transmitted after
    /// processing the command.
    static constexpr const string_type::value_type *aRetTypeStrings[] =
        {
            "OK",                   // eOK
            "ERR: Invalid command", // eINV_CMD
            "ERR: EEPROM failure",  // eERR_EEPROM
            "ERR: Invalid CV ID",   // eINV_CV_ID
            "ERR: CV value is out of range",   // eCV_VALUE_OUT_OF_RANGE
            // "ERR: Signal id invalid",               // eINV_SIGNAL_ID
            // "ERR: Unknown signal sub command",      // eINV_SIGNAL_CMD
            // "ERR: Unknown signal aspects",          // eINV_SIGNAL_ASPECTS
            // "ERR: Unknown signal blinks",           // eINV_SIGNAL_BLINKS
            // "ERR: Unknown signal targets",          // eINV_SIGNAL_TARGETS
            // "ERR: Unknown signal input",            // eINV_SIGNAL_INPUT
            // "ERR: Unknown signal change over time", // eINV_SIGNAL_CHANGEOVERTIME
            // "ERR: Unknown signal DB Ausfahrsignal", // eINV_SIGNAL_DB_AUSFAHRSIGNAL
            // "ERR: Unknown signal DB Blocksignal",   // eINV_SIGNAL_DB_BLOCKSIGNAL
            // "ERR: Unknown classifier sub command: GET_CLASSIFIER id [LIMITS slot-id,PIN,DEBOUNCE]",  // eINV_CLASSIFIER_CMD
            // "ERR: Unknown classifier pin: GET_CLASSIFIER id PIN",                            // eINV_CLASSIFIER_PIN
            // "ERR: Unknown classifier limits: GET_CLASSIFIER id LIMITS slot_id",              // eINV_CLASSIFIER_LIMITS
            // "ERR: Unknown classifier debounce",     // eINV_CLASSIFIER_DEBOUNCE
            "ERR: Unknown monitor start parameter: MONITOR_START cycle-time ifc-name",      // eINV_MONITOR_START_PARAM
            "ERR: Unknown monitor start interface name: MONITOR_START cycle-time ifc-name", // eINV_MONITOR_START_IFC_NAME
            "ERR: unknown error"                                                            // has to be the last element
    };

#if 0
  static uint8 convertStrToU8(util::string_view sv);
#endif

    static tRetType process_set_cv(stringstream_type &st, string_type &response);
    static tRetType process_get_cv(stringstream_type &st, string_type &response);
    static tRetType process_monitor_list(stringstream_type &st, string_type &response);
    static tRetType process_monitor_start(stringstream_type &st, string_type &response);
    static tRetType process_monitor_stop(stringstream_type &st, string_type &response);
    static tRetType process_set_defaults(stringstream_type &st, string_type &response);

    static bool output_monitor_list(string_type &response);
    static bool output_port_data(port_type &pm, string_type &response);

    static bool doOutputPortList = false;
    static port_type portMonitor;

    typedef tRetType (*func_type)(stringstream_type &st, string_type &response);

    typedef struct
    {
        // Length of szCmd shall never exceed kMaxLenToken
        const char *szCmd;
        func_type func;
    } tCommands;

    /// Max length of a token (how many characters)
    static constexpr util::streamsize kMaxLenToken = 20;

    // Max Length of strings: kMaxLenToken
    const util::array<tCommands, 6> commands =
        {{{"SET_CV", process_set_cv},
          {"GET_CV", process_get_cv},
          {"MON_LIST", process_monitor_list},
          {"MON_START", process_monitor_start},
          {"MON_STOP", process_monitor_stop},
          {"INIT", process_set_defaults}}};

    // -----------------------------------------------------------------------------------
    /// Process the telegram from AsciiTP.
    ///
    /// \code
    ///
    /// GET SIGNAL id ASPECTS                           returns all (5) aspects for signal 0
    /// GET SIGNAL id TARGETS                           returns all (5) target ports for signal 0
    ///
    /// CPY SIGNAL from SIGNAL to                       copies all aspects and target ports from signal 0 to signal 1
    /// CPY SIGNAL from SIGNAL to TARGETS               copies all targets from signal 0 to signal 1
    /// \code
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
    /// SET_SIGNAL ...
    // -----------------------------------------------------------------------------------
    void AsciiCom::process(const string_type &telegram, string_type &response)
    {
        stringstream_type st(telegram);
        char cmd[kMaxLenToken];
        tRetType ret = eINV_CMD;
        string_type sub_response;

        st >> util::setw(kMaxLenToken) >> cmd;
        util::string_view sv(cmd);
        for (auto cmdit = commands.begin(); cmdit != commands.end(); cmdit++)
        {
            if (sv.compare(cmdit->szCmd) == 0)
            {
                ret = cmdit->func(st, sub_response);
                break;
            }
        }

        response = aRetTypeStrings[static_cast<size_type>(ret)];
        if (sub_response.size() > 0)
        {
            response += " ";
            response += sub_response;
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
#if 0
    static uint8 ucCnt = 0;
    util::basic_string<4, char_type> str;
    if (asciiTP)
    {
      ucCnt++;
      if (ucCnt == 100)
      {
        if (rte::ifc_ad_values::boundaryCheck(monitorClassified))
        {
          telegram_response.clear();
          uint16 tmp;
          uint8 cls;
          rte::intensity8_255 intensity;
          rte::ifc_ad_values::readElement(monitorClassified, tmp);
          rte::ifc_classified_values::readElement(monitorClassified, cls);
          util::to_string(static_cast<int>(tmp), str);
          telegram_response.append(str);
          util::to_string(static_cast<int>(cls), str);
          telegram_response.append(" ");
          telegram_response.append(str);
          rte::ifc_onboard_target_duty_cycles::readElement(12, intensity);
          util::to_string(static_cast<int>(intensity.v), str);
          telegram_response.append(" ");
          telegram_response.append(str);
          rte::ifc_onboard_target_duty_cycles::readElement(11, intensity);
          util::to_string(static_cast<int>(intensity.v), str);
          telegram_response.append(" ");
          telegram_response.append(str);
          rte::ifc_onboard_target_duty_cycles::readElement(10, intensity);
          util::to_string(static_cast<int>(intensity.v), str);
          telegram_response.append(" ");
          telegram_response.append(str);
          rte::ifc_onboard_target_duty_cycles::readElement(9, intensity);
          util::to_string(static_cast<int>(intensity.v), str);
          telegram_response.append(" ");
          telegram_response.append(str);
          rte::ifc_onboard_target_duty_cycles::readElement(8, intensity);
          util::to_string(static_cast<int>(intensity.v), str);
          telegram_response.append(" ");
          telegram_response.append(str);
          if (telegram_response.size() > 0)
          {
            asciiTP->transmitTelegram(telegram_response);
          }
        }
        ucCnt = 0;
      }
    }
#endif
    }

#if 0
  // -----------------------------------------------------------------------------------
  /// Convert a string of 1's and 0's to a uint8.
  /// "11000" -> 0x18 = 24.
  ///
  /// Remark for aspects:
  /// MSB of aspect is index 0 in target intensity array
  /// LSB of aspect is index cfg::kNrSignalTargets-1 in target intensity array
  // -----------------------------------------------------------------------------------
  static uint8 convertStrToU8(util::string_view sv)
  {
    uint8 uc = 0;
    for (auto it = sv.begin(); it != sv.end(); it++)
    {
      uc <<= 1;
      if (*it == '1')
      {
        uc |= 0x01U;
      }
    }
    return uc;
  }

  // -----------------------------------------------------------------------------------
  /// Convert an uint8 to a string of 1's and 0's.
  /// 24 (0x18) -> "11000"
  ///
  /// Remark for aspects:
  /// MSB of aspect is index 0 in target intensity array
  /// LSB of aspect is index cfg::kNrSignalTargets-1 in target intensity array
  // -----------------------------------------------------------------------------------
  static util::basic_string<cfg::kNrSignalTargets, char_type> convertU8ToStr(uint8 val)
  {
    int i;
    util::basic_string<cfg::kNrSignalTargets, char_type> str;

    for (i = cfg::kNrSignalTargets-1; i >= 0; i--)
    {
      if (util::bits::test<uint8>(val, i))
      {
        str.append(1, '1');
      }
      else
      {
        str.append(1, '0');
      }
    }

    return str;
  }
#endif

    /**
     * @brief Implements command SET_CV <cv_id> <value>
     *
     * @param st Contains the command string, get pointer points to first element after "SET_CV".
     * @param response [out] The response is stored here, it is the contains the command parameters.
     * @return tRetType eOK
     * @return tRetType eINV_CMD Ill-formed command or CV id is out-of-bounds
     * @return tRetType eCV_VALUE_OUT_OF_RANGE CV value is out-of-bounds
     * 
     */
    static tRetType process_set_cv(stringstream_type &st, string_type &response)
    {
        tRetType ret = eINV_CMD;
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
                if (rte::ifc_cal_set_cv::call(new_cv.id, new_cv.val) == rte::ret_type::OK)
                {
                    ret = eOK;
                }
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
     * @param response [out] The response is stored here, it is the contains the command parameters.
     * @return tRetType eOK
     * @return tRetType eINV_CMD Ill-formed command or CV id is out-of-bounds
     * @return tRetType eCV_VALUE_OUT_OF_RANGE CV value is out-of-bounds
     * 
     */
    static tRetType process_get_cv(stringstream_type &st, string_type &response)
    {
        tRetType ret = eINV_CMD;
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
            if (rte::ifc_cal_get_cv::call(cv.id, &cv.val) == rte::ret_type::OK)
            {
                util::basic_string<4, char> tmp;
                util::to_string(static_cast<int>(cv.val), tmp);
                response.append(" ");
                response.append(tmp);
                hal::serial::print("GET_CV");
                hal::serial::print(" ");hal::serial::print(static_cast<int>(cv.id));
                hal::serial::print(" ");hal::serial::print(static_cast<int>(cv.val));
                hal::serial::println();
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
     * @return tRetType eOK
     */
    static tRetType process_monitor_list(stringstream_type &st, string_type &response)
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
            response.append("[").append(tmp).append("] ");
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
    static tRetType process_monitor_start(stringstream_type &st, string_type &response)
    {
        char ifc_name[32];
        uint16 unCycleTime;
        uint16 unFirstIdx;
        uint16 unNrIdx;
        tRetType ret;
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
    static tRetType process_monitor_stop(stringstream_type &st, string_type &response)
    {
        (void)st;
        (void)response;

        portMonitor.pPortData = nullptr;

        return eOK;
    }

    // -----------------------------------------------------------------------------------
    /// Write default values to NVM
    // -----------------------------------------------------------------------------------
    static tRetType process_set_defaults(stringstream_type &st, string_type &response)
    {
        (void)st;
        (void)response;

        return (rte::ifc_cal_set_defaults::call() == rte::ret_type::OK) ? eOK : eERR_EEPROM;
    }

} // namespace com
