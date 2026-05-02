/**
 * @file Prj/App/Signal/Com/SignalAsciiCommandHandler.cpp
 *
 * @brief Signal project specific ASCII command handler.
 *
 * @copyright Copyright 2024 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Com/SignalAsciiCommandHandler.h>
#include <Debug.h>
#include <Util/Array.h>
#include <Util/String_view.h>
#include <Rte/Rte.h>

namespace com
{

    namespace
    {
        enum ret_type
        {
            eOK = 0,
            eERR_EEPROM,
            eINV_CMD,
            eINV_SIGNAL_IDX,
            eINV_SIGNAL_ID,
            eINV_FIRST_OUTPUT_TYPE,
            eINV_OUTPUT_CONFIG_STEP_SIZE,
            eINV_INPUT_TYPE,
            eINV_OUTPUT_PIN,
            eINV_INPUT_PIN,
            eINV_PARAM,
            eINV_VERBOSE_LEVEL,
            eERR_UNKNOWN
        };

        const char ret_OK[] ROM_CONST_VAR = "OK";
        const char ret_ERR_EEPROM[] ROM_CONST_VAR = "ERR: EEPROM failure";
        const char ret_INV_CMD[] ROM_CONST_VAR = "ERR: Invalid command";
        const char ret_INV_SIGNAL_IDX[] ROM_CONST_VAR = "ERR: Invalid signal index";
        const char ret_INV_SIGNAL_ID[] ROM_CONST_VAR = "ERR: Invalid signal id";
        const char ret_INV_FIRST_OUTPUT_TYPE[] ROM_CONST_VAR = "ERR: Invalid first output type";
        const char ret_INV_OUTPUT_CONFIG_STEP_SIZE[] ROM_CONST_VAR = "ERR: Invalid output step size config";
        const char ret_INV_INPUT_TYPE[] ROM_CONST_VAR = "ERR: Invalid input type";
        const char ret_INV_OUTPUT_PIN[] ROM_CONST_VAR = "ERR: Invalid output pin";
        const char ret_INV_INPUT_PIN[] ROM_CONST_VAR = "ERR: Invalid input pin";
        const char ret_INV_PARAM[] ROM_CONST_VAR = "ERR: Invalid parameter";
        const char ret_INV_VERBOSE_LEVEL[] ROM_CONST_VAR = "ERR: Invalid verbose level: SET_VERBOSE 0 ... 3";
        const char ret_ERR_UNKNOWN[] ROM_CONST_VAR = "ERR: unknown error";

        static constexpr const char *responses[] ROM_CONST_VAR =
        {
            ret_OK,
            ret_ERR_EEPROM,
            ret_INV_CMD,
            ret_INV_SIGNAL_IDX,
            ret_INV_SIGNAL_ID,
            ret_INV_FIRST_OUTPUT_TYPE,
            ret_INV_OUTPUT_CONFIG_STEP_SIZE,
            ret_INV_INPUT_TYPE,
            ret_INV_OUTPUT_PIN,
            ret_INV_INPUT_PIN,
            ret_INV_PARAM,
            ret_INV_VERBOSE_LEVEL,
            ret_ERR_UNKNOWN
        };

        const char cmd_INIT[] ROM_CONST_VAR = "INIT";
        const char cmd_SET_VERBOSE[] ROM_CONST_VAR = "SET_VERBOSE";
        const char cmd_SET_SIGNAL[] ROM_CONST_VAR = "SET_SIGNAL";
        const char cmd_GET_SIGNAL[] ROM_CONST_VAR = "GET_SIGNAL";
        const char cmd_GET_PIN_CONFIG[] ROM_CONST_VAR = "GET_PIN_CONFIG";
        const char cmd_ETO_SET_SIGNAL[] ROM_CONST_VAR = "ETO_SET_SIGNAL";

        static ret_type process_set_defaults(IfcAsciiCommandHandler::stringstream_type &st, IfcAsciiCommandHandler::string_type &response);
        static ret_type process_eto_set_signal(IfcAsciiCommandHandler::stringstream_type &st, IfcAsciiCommandHandler::string_type &response);
        static ret_type process_set_signal(IfcAsciiCommandHandler::stringstream_type &st, IfcAsciiCommandHandler::string_type &response);
        static ret_type process_get_signal(IfcAsciiCommandHandler::stringstream_type &st, IfcAsciiCommandHandler::string_type &response);
        static ret_type process_set_verbose(IfcAsciiCommandHandler::stringstream_type &st, IfcAsciiCommandHandler::string_type &response);
        static ret_type process_get_pin_config(IfcAsciiCommandHandler::stringstream_type &st, IfcAsciiCommandHandler::string_type &response);

        using command_handler_type = ret_type (*)(IfcAsciiCommandHandler::stringstream_type &st, IfcAsciiCommandHandler::string_type &response);

        struct command_type
        {
            const char *cmd;
            command_handler_type handler;
        };

        using command_array_type = util::array<command_type, 6>;

        static constexpr command_array_type commands =
            {{{cmd_INIT, process_set_defaults},
              {cmd_SET_VERBOSE, process_set_verbose},
              {cmd_SET_SIGNAL, process_set_signal},
              {cmd_GET_SIGNAL, process_get_signal},
              {cmd_GET_PIN_CONFIG, process_get_pin_config},
              {cmd_ETO_SET_SIGNAL, process_eto_set_signal}}};

        static ret_type process_set_defaults(IfcAsciiCommandHandler::stringstream_type &st, IfcAsciiCommandHandler::string_type &response)
        {
            (void)st;
            response.append(st.str());
            return rte::ifc_cal_set_defaults() ? eOK : eERR_EEPROM;
        }

        static ret_type process_eto_set_signal(IfcAsciiCommandHandler::stringstream_type &st, IfcAsciiCommandHandler::string_type &response)
        {
            uint16 signal_idx;
            uint16 aspect;
            uint16 dim_time_10ms = 10;
            ret_type ret = eINV_CMD;

            st >> signal_idx;
            st >> aspect;
            response.append(st.str());

            if (!st.fail())
            {
                st >> dim_time_10ms;
                if (st.fail())
                {
                    dim_time_10ms = 10;
                }

                if (signal_idx < cfg::kNrSignals)
                {
                    const bool enabled = aspect != 0U;
                    rte::sig::eto_set_signal_aspect_for_idx(
                        signal_idx,
                        enabled,
                        static_cast<uint8>(aspect),
                        static_cast<uint8>(dim_time_10ms));
                    ret = eOK;
                }
                else
                {
                    ret = eINV_SIGNAL_IDX;
                }
            }

            return ret;
        }

        static ret_type process_set_signal(IfcAsciiCommandHandler::stringstream_type &st, IfcAsciiCommandHandler::string_type &response)
        {
            ret_type ret = eINV_CMD;
            uint16 signal_idx;
            uint16 signal_id;
            uint16 output_type;
            uint16 first_output_pin;
            sint16 step_size;
            uint16 input_type;
            uint16 input_pin;
            uint8 tmp;
            char output_type_str[4];
            char input_type_str[4];

            response.append(st.str());

            st >> signal_idx;
            st >> signal_id;
            st >> util::setw(4) >> output_type_str;
            st >> first_output_pin;
            st >> step_size;
            st >> util::setw(4) >> input_type_str;
            st >> input_pin;

            if (!st.fail())
            {
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
                    output_type = 255;
                }

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
                    input_type = 255;
                }

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
                    tmp = cal::constants::make_signal_first_output(output_type, first_output_pin);
                    rte::set_cv(cal::cv::kSignalFirstOutputBase + signal_idx, tmp);
                    tmp = cal::constants::make_signal_input(input_type, input_pin);
                    rte::set_cv(cal::cv::kSignalInputBase + signal_idx, tmp);
                    tmp = 0U;
                    if (step_size < 0)
                    {
                        tmp = 0b00000001U;
                    }
                    if ((step_size == -2) || (step_size == 2))
                    {
                        tmp |= 0b00000010U;
                    }
                    rte::set_cv(cal::cv::kSignalOutputConfigBase + signal_idx, tmp);
                    ret = eOK;
                }
            }

            return ret;
        }

        static ret_type process_get_signal(IfcAsciiCommandHandler::stringstream_type &st, IfcAsciiCommandHandler::string_type &response)
        {
            ret_type ret = eINV_CMD;
            uint16 signal_idx;
            uint16 signal_id;
            uint16 output_type;
            uint16 first_output_pin;
            sint16 step_size;
            uint16 input_type;
            uint16 input_pin;
            uint8 tmp;

            response.append(st.str());
            st >> signal_idx;
            if (!st.fail())
            {
                if (signal_idx >= cfg::kNrSignals)
                {
                    ret = eINV_SIGNAL_IDX;
                }
                else
                {
                    util::basic_string<4, char> tmp_str;

                    signal_id = rte::get_cv(cal::cv::kSignalIDBase + signal_idx);
                    tmp = rte::get_cv(cal::cv::kSignalFirstOutputBase + signal_idx);
                    output_type = cal::constants::extract_signal_first_output_type(tmp);
                    first_output_pin = cal::constants::extract_signal_first_output_pin(tmp);
                    tmp = rte::get_cv(cal::cv::kSignalInputBase + signal_idx);
                    input_type = cal::constants::extract_signal_input_type(tmp);
                    input_pin = cal::constants::extract_signal_input_pin(tmp);
                    tmp = rte::get_cv(cal::cv::kSignalOutputConfigBase + signal_idx);
                    response.append(" ");
                    util::to_string(static_cast<int>(signal_id), tmp_str);
                    response.append(tmp_str);
                    response.append(" ");
                    if (output_type == cal::constants::kOnboard)
                    {
                        response.append("ONB");
                    }
                    else
                    {
                        response.append("EXT");
                    }
                    response.append(" ");
                    util::to_string(static_cast<int>(first_output_pin), tmp_str);
                    response.append(tmp_str);
                    response.append(" ");
                    if (util::bits::test<uint8>(tmp, 0))
                    {
                        if (util::bits::test<uint8>(tmp, 1))
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
                        if (util::bits::test<uint8>(tmp, 1))
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
                    if (input_type == cal::constants::kAdc)
                    {
                        response.append("ADC");
                    }
                    else if (input_type == cal::constants::kDig)
                    {
                        response.append("DIG");
                    }
                    else
                    {
                        response.append("DCC");
                    }
                    response.append(" ");
                    util::to_string(static_cast<int>(input_pin), tmp_str);
                    response.append(tmp_str);
                    ret = eOK;
                }
            }

            return ret;
        }

        static ret_type process_set_verbose(IfcAsciiCommandHandler::stringstream_type &st, IfcAsciiCommandHandler::string_type &response)
        {
            uint16 value;
            ret_type ret = eINV_VERBOSE_LEVEL;

            response.append(st.str());
            st >> value;
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

        static ret_type process_get_pin_config(IfcAsciiCommandHandler::stringstream_type &st, IfcAsciiCommandHandler::string_type &response)
        {
            uint16 pin;
            ret_type ret = eINV_CMD;

            response.append(st.str());
            st >> pin;
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


    }

    bool SignalAsciiCommandHandler::process_command(const char *cmd, stringstream_type &st, string_type &response)
    {
        static constexpr size_t kMaxLenToken = 20U;
        size_t idx;
        boolean found = false;
        ret_type ret = eINV_CMD;
        string_type sub_response;
        util::string_view sv(cmd);
        char cmd_rom[kMaxLenToken];

        for (idx = 0U; idx < commands.size(); idx++)
        {
            ROM_READ_STRING(cmd_rom, commands[idx].cmd);
            if (sv.compare(cmd_rom) == 0)
            {
                ret = commands[idx].handler(st, sub_response);
                found = true;
                break;
            }
        }

        if (!found)
        {
            return false;
        }

        const char *response_text = static_cast<const char *>(ROM_READ_PTR(&responses[static_cast<size_t>(ret)]));
        response = response_text;
        if (sub_response.size() > 0U)
        {
            response.append(" ");
            response.append(sub_response);
        }
        return true;
    }

} // namespace com
