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
#include <Cal/CalM_Types.h>
#include <Com/AsciiCom.h>
#include <Debug.h>
#include <Util/Sstream.h>
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

    /// Return values of process() function family
    typedef enum
    {
        eOK = 0,     ///< OK
        eINV_CMD,    ///< Command invalid (or unknown)
        eERR_EEPROM, ///< EEPROM update failure
        eINV_CV_ID,  ///< SET_CV with an invalid CV id
        eCV_VALUE_OUT_OF_RANGE, ///< SET_CV with an invalid CV value
        eINV_SIGNAL_IDX,    ///< SET_SIGNAL with an invalid signal index
        eINV_SIGNAL_ID,     ///< SET_SIGNAL with an invalid signal id
        eINV_FIRST_OUTPUT_TYPE, ///< SET_SIGNAL with an invalid first output type
        eINV_OUTPUT_CONFIG_STEP_SIZE,   ///< SET_SIGNAL with an invalid output step size config
        eINV_INPUT_TYPE,    ///< SET_SIGNAL with an invalid input type
        eINV_OUTPUT_PIN,     ///< SET_SIGNAL with an invalid output pin
        eINV_INPUT_PIN,      ///< SET_SIGNAL with an invalid input pin
        eINV_PARAM,           ///< Generic invalid parameter
        eINV_MONITOR_START_PARAM,
        eINV_MONITOR_START_IFC_NAME,
        eINV_VERBOSE_LEVEL,
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
            "ERR: CV value is out of range",        // eCV_VALUE_OUT_OF_RANGE
            "ERR: Invalid signal index",            // eINV_SIGNAL_IDX
            "ERR: Invalid signal id",               // eINV_SIGNAL_ID
            "ERR: Invalid first output type",       // eINV_FIRST_OUTPUT_TYPE
            "ERR: Invalid output step size config", // eINV_OUTPUT_CONFIG_STEP_SIZE
            "ERR: Invalid input type",              // eINV_INPUT_TYPE
            "ERR: Invalid output pin",              // eINV_OUTPUT_PIN
            "ERR: Invalid input pin",               // eINV_INPUT_PIN
            "ERR: Invalid parameter",               // eINV_PARAM
            "ERR: Unknown monitor start parameter: MONITOR_START cycle-time ifc-name",      // eINV_MONITOR_START_PARAM
            "ERR: Unknown monitor start interface name: MONITOR_START cycle-time ifc-name", // eINV_MONITOR_START_IFC_NAME
            "ERR: Invalid verbose level: SET_VERBOSE 0 ... 3",                              // eINV_VERBOSE_LEVEL
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
    
    static tRetType process_set_signal(stringstream_type &st, string_type &response);
    static tRetType process_get_signal(stringstream_type &st, string_type &response);

    static bool output_monitor_list(string_type &response);
    static bool output_port_data(port_type &pm, string_type &response);

    static tRetType process_set_verbose(stringstream_type &st, string_type &response);
    static tRetType process_get_pin_config(stringstream_type &st, string_type &response);

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
    const util::array<tCommands, 10> commands =
        {{{"SET_CV", process_set_cv},
          {"GET_CV", process_get_cv},
          {"MON_LIST", process_monitor_list},
          {"MON_START", process_monitor_start},
          {"MON_STOP", process_monitor_stop},
          {"INIT", process_set_defaults},
          {"SET_VERBOSE", process_set_verbose},
          {"SET_SIGNAL", process_set_signal},
          {"GET_SIGNAL", process_get_signal},
          {"GET_PIN_CONFIG", process_get_pin_config}
        }};

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
    ///
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
     * @param st [in] Contains the command string, get pointer points to first element after "SET_CV".
     * @param response [out] The response is stored here, it contains the command parameters.
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
     * @brief Implements command SET_SIGNAL idx id [ONB,EXT] output_pin step_size [ADC,DIG,DCC] input_pin
     *
     * Sets for the signal at position signal_idx the following CVs:
     * - `cal::cv::kSignalIDBase + idx` to id
     * - `cal::cv::kSignalFirstOutputBase + idx` to [0 for ONB, 1 for EXT] and output_pin
     * - `cal::cv::kSignalOutputConfigBase + idx` to step_size (inverse is determined by the sign of step_size)
     * - `cal::cv::kSignalInputBase + idx` to [0 = DCC, 1 = ADC, 2 = DIG] and input_pin
     * 
     * Example: `SET_SIGNAL 0 1 ONB 10 -1 ADC 54`
     * Sets signal at index 0 to:
     * - signal id 1
     * - first output type 0 (onboard output), first output pin 10
     * - step size -1 (inverse is determined by the sign of step size)
     * - input type 1 (ADC input), input pin 54
     * 
     * @param st [in] Contains the command string, get pointer points to first element after "SET_SIGNAL".
     * @param response [out] The response is stored here, it contains the command parameters.
     * @return tRetType eOK
     * @return tRetType eINV_CMD Ill-formed command or signal id is out-of-bounds
     * @return tRetType eCV_VALUE_OUT_OF_RANGE CV value is out-of-bounds
     * 
     */
    static tRetType process_set_signal(stringstream_type &st, string_type &response)
    {
        tRetType ret = eINV_CMD;
        uint16 signal_idx;
        uint16 signal_id;
        uint16 output_type;
        uint16 first_output_pin;
        sint16 step_size;
        uint16 input_type;
        uint16 input_pin;
        uint8 tmp;
        char output_type_str[4]; // ONB / EXT plus terminating null
        char input_type_str[4]; // ADC / DIG / DCC plus terminating null

        // The response shall contain the command parameters
        response.append(st.str());

        // Use uint16 here to ensure numeric values are extracted correctly.
        // If uint8 is used, the extraction may interpret the value as a character instead of a 
        // number.
        st >> signal_idx;
        st >> signal_id;
        st >> util::setw(4) >> output_type_str;
        st >> first_output_pin;
        st >> step_size;
        st >> util::setw(4) >> input_type_str;
        st >> input_pin;

        // Do not check for eof() since eof() is true after extracting the last element
        // (and if the last element doesn't have trailing white spaces).
        if (!st.fail())
        {
            // Convert output type string to constant
            if (util::string_view(output_type_str).compare("ONB") == 0)
            {
                output_type = cal::constants::kOnboard;
            }
            else if (util::string_view(output_type_str).compare("EXT") == 0)
            {
                output_type = cal::constants::kExternal;
            }
            else
            {
                output_type = 255; // invalid
            }

            // Convert input type string to constant
            if (util::string_view(input_type_str).compare("ADC") == 0)
            {
                input_type = cal::constants::kAdc;
            }
            else if (util::string_view(input_type_str).compare("DIG") == 0)
            {
                input_type = cal::constants::kDig;
            }
            else if (util::string_view(input_type_str).compare("DCC") == 0)
            {
                input_type = cal::constants::kDcc;
            }
            else
            {
                input_type = 255; // invalid
            }

            // Validate parameters
            if (signal_idx >= cfg::kNrSignals)
            {
                ret = eINV_SIGNAL_IDX;
            }
            else if (!rte::sig::is_built_in(signal_id) && !rte::sig::is_user_defined(signal_id))    
            {
                ret = eINV_SIGNAL_ID;
            }
            else if ((output_type != cal::constants::kOnboard) && (output_type != cal::constants::kExternal))
            {
                ret = eINV_FIRST_OUTPUT_TYPE;
            }
            else if ((step_size < -2) || (step_size > 2) || (step_size == 0))
            {
                ret = eINV_OUTPUT_CONFIG_STEP_SIZE;
            }
            else if (input_type > cal::constants::kDig)
            {
                ret = eINV_INPUT_TYPE;
            }
            else if (first_output_pin >= platform::numeric_limits<uint8>::max_())
            {
                ret = eINV_OUTPUT_PIN;
            }
            else if (input_pin >= platform::numeric_limits<uint8>::max_())
            {
                ret = eINV_INPUT_PIN;
            }
            else
            {
                rte::set_cv(cal::cv::kSignalIDBase + signal_idx, static_cast<uint8>(signal_id));
                // ... with first output pin first_output_pin
                tmp = cal::constants::make_signal_first_output(output_type, first_output_pin);
                rte::set_cv(cal::cv::kSignalFirstOutputBase + signal_idx, tmp);
                // ... with ADC input pin input_pin
                tmp = cal::constants::make_signal_input(input_type, input_pin);
                rte::set_cv(cal::cv::kSignalInputBase + signal_idx, tmp);
                // ... with inverse output pin order and/or step size
                tmp = 0U;
                if (step_size < 0)
                {
                    tmp = 0b00000001U; // set inverse order bit
                }
                // ... with step size 2
                if ((step_size == -2) || (step_size == 2))
                {
                    tmp |= 0b00000010U; // set step size to 2
                } 
                rte::set_cv(cal::cv::kSignalOutputConfigBase + signal_idx, tmp);
                // ... leave classifier type unchanged

                ret = eOK;
            }
        }

        return ret;
    }

    /**
     * @brief Implements command GET_SIGNAL idx
     *
     * @param st Contains the command string, get pointer points to first element after "GET_SIGNAL".
     * @param response [out] The response is stored here, it contains the command parameters.
     * @return tRetType eOK
     * @return tRetType eINV_CMD Ill-formed command or signal index is out-of-bounds
     * @return tRetType eINV_SIGNAL_IDX signal index is out-of-bounds
     */
    static tRetType process_get_signal(stringstream_type &st, string_type &response)
    {
        tRetType ret = eINV_CMD;
        uint16 signal_idx;
        uint16 signal_id;
        uint16 output_type;
        uint16 first_output_pin;
        sint16 step_size;
        uint16 input_type;
        uint16 input_pin;
        uint8 tmp;

        // The response shall contain the command parameters
        response.append(st.str());

        // Use uint16 here to ensure numeric values are extracted correctly.
        // If uint8 is used, the extraction may interpret the value as a character instead of a 
        // number.
        st >> signal_idx;
        // Do not check for eof() since eof() is true after extracting the last element
        // (and if the last element doesn't have trailing white spaces).
        if (!st.fail())
        {
            // Validate parameters
            if (signal_idx >= cfg::kNrSignals)
            {
                ret = eINV_SIGNAL_IDX;
            }
            else
            {
                util::basic_string<4, char> tmp_str;

                signal_id = rte::get_cv(cal::cv::kSignalIDBase + signal_idx);
                // ... with first output pin first_output_pin
                tmp = cal::constants::make_signal_first_output(output_type, first_output_pin);
                tmp = rte::get_cv(cal::cv::kSignalFirstOutputBase + signal_idx);
                output_type = cal::constants::extract_signal_first_output_type(tmp);
                first_output_pin = cal::constants::extract_signal_first_output_pin(tmp);
                // ... with ADC input pin input_pin
                tmp = rte::get_cv(cal::cv::kSignalInputBase + signal_idx);
                input_type = cal::constants::extract_signal_input_type(tmp);
                input_pin = cal::constants::extract_signal_input_pin(tmp);
                // ... with inverse output pin order and/or step size
                tmp = rte::get_cv(cal::cv::kSignalOutputConfigBase + signal_idx);
                // GET_SIGNAL idx id [ONB,EXT] output_pin step_size [ADC,DIG,DCC] input_pin
                response.append(" ");
                util::to_string(static_cast<int>(signal_id), tmp_str);
                response.append(tmp_str);
                response.append(" ");
                // output type
                if (output_type == cal::constants::kOnboard)
                {
                    response.append("ONB");
                }
                else // cal::constants::kExternal
                {
                    response.append("EXT");
                }
                response.append(" ");
                // first output pin
                util::to_string(static_cast<int>(first_output_pin), tmp_str);
                response.append(tmp_str);
                response.append(" ");
                // step size
                if (util::bits::test<uint8>(tmp, 0)) // inverse order bit
                {
                    if (util::bits::test<uint8>(tmp, 1)) // step size 2
                    {
                        step_size = -2;
                    }
                    else
                    {
                        step_size = -1;
                    }
                }
                else
                {
                    if (util::bits::test<uint8>(tmp, 1)) // step size 2
                    {
                        step_size = 2;
                    }
                    else
                    {
                        step_size = 1;
                    }
                }
                util::to_string(static_cast<int>(step_size), tmp_str);
                response.append(tmp_str);
                response.append(" ");
                // input type
                if (input_type == cal::constants::kAdc)
                {
                    response.append("ADC");
                }
                else if (input_type == cal::constants::kDig)
                {
                    response.append("DIG");
                }
                else // cal::constants::kDcc
                {
                    response.append("DCC");
                }
                response.append(" ");
                // input pin
                util::to_string(static_cast<int>(input_pin), tmp_str);
                response.append(tmp_str);
                ret = eOK;
            }
        }

        return ret;
    }

    /**
     * @brief Implements command GET_CV <cv_id>
     *
     * @param st Contains the command string, get pointer points to first element after "GET_CV".
     * @param response [out] The response is stored here, it contains the command parameters.
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
            if (rte::is_cv_id_valid(cv.id))
            {
                cv.val = rte::get_cv(cv.id);
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

        // The response shall contain the command
        response.append(st.str());

        return rte::ifc_cal_set_defaults() ? eOK : eERR_EEPROM;
    }

    static tRetType process_set_verbose(stringstream_type &st, string_type &response)
    {
        uint16 value;
        tRetType ret = eINV_VERBOSE_LEVEL;

        // The response shall contain the command parameters
        response.append(st.str());

        st >> value;
        // Do not check for eof() since eof() is true after extracting the last element
        // (and if the last element doesn't have trailing white spaces).
        if (!st.fail())
        {
            if (value <= debug::kVeryDetailed)
            {
                debug::enable(static_cast<uint8>(value));
                ret = eOK;
            }
        }

        return ret;
    }

    /**
     * @brief GET_PIN_CONFIG <pin> 
     * 
     * @param st 
     * @param response 
     * @return tRetType 
     */
    static tRetType process_get_pin_config(stringstream_type &st, string_type &response)
    {
        uint16 pin;
        tRetType ret = eINV_CMD;

        // The response shall contain the command parameters
        response.append(st.str());

        st >> pin;
        // Do not check for eof() since eof() is true after extracting the last element
        // (and if the last element doesn't have trailing white spaces).
        if (!st.fail())
        {
            if (pin < cfg::kNrOnboardTargets)
            {
                rte::sig::is_output_pin(pin) ? response.append(" OUTPUT") : response.append(" INPUT");
                ret = eOK;
            }
            else
            {
                ret = eINV_PARAM;
            }
        }

        return ret;
    }

} // namespace com
