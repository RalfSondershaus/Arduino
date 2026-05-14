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
#include <Util/Algorithm.h>
#include <Util/Array.h>
#include <Util/String_view.h>
#include <Rte/Rte.h>

namespace com
{

    namespace
    {
        using ret_type = IfcAsciiCommandHandler::ret_type;

        // Aliases for the interface sentinels (used by internal handlers)
        static constexpr ret_type kIfcOK     = IfcAsciiCommandHandler::kIfcOK;
        static constexpr ret_type kIfcInvCmd = IfcAsciiCommandHandler::kIfcInvCmd;

        // Project-specific error codes, indexed from kIfcProjectBase
        static constexpr ret_type kErrEeprom               = IfcAsciiCommandHandler::kIfcProjectBase + 0;
        static constexpr ret_type kInvSignalIdx            = IfcAsciiCommandHandler::kIfcProjectBase + 1;
        static constexpr ret_type kInvSignalId             = IfcAsciiCommandHandler::kIfcProjectBase + 2;
        static constexpr ret_type kInvFirstOutputType      = IfcAsciiCommandHandler::kIfcProjectBase + 3;
        static constexpr ret_type kInvOutputConfigStepSize = IfcAsciiCommandHandler::kIfcProjectBase + 4;
        static constexpr ret_type kInvInputType            = IfcAsciiCommandHandler::kIfcProjectBase + 5;
        static constexpr ret_type kInvOutputPin            = IfcAsciiCommandHandler::kIfcProjectBase + 6;
        static constexpr ret_type kInvInputPin             = IfcAsciiCommandHandler::kIfcProjectBase + 7;
        static constexpr ret_type kInvParam                = IfcAsciiCommandHandler::kIfcProjectBase + 8;
        static constexpr ret_type kInvVerboseLevel         = IfcAsciiCommandHandler::kIfcProjectBase + 9;

        // Project-specific error strings only; "OK" and "ERR: Invalid command" are owned by AsciiCom.
        const char ret_ERR_EEPROM[] ROM_CONST_VAR = "ERR: EEPROM failure";
        const char ret_INV_SIGNAL_IDX[] ROM_CONST_VAR = "ERR: Invalid signal index";
        const char ret_INV_SIGNAL_ID[] ROM_CONST_VAR = "ERR: Invalid signal id";
        const char ret_INV_FIRST_OUTPUT_TYPE[] ROM_CONST_VAR = "ERR: Invalid first output type";
        const char ret_INV_OUTPUT_CONFIG_STEP_SIZE[] ROM_CONST_VAR = "ERR: Invalid output step size config";
        const char ret_INV_INPUT_TYPE[] ROM_CONST_VAR = "ERR: Invalid input type";
        const char ret_INV_OUTPUT_PIN[] ROM_CONST_VAR = "ERR: Invalid output pin";
        const char ret_INV_INPUT_PIN[] ROM_CONST_VAR = "ERR: Invalid input pin";
        const char ret_INV_PARAM[] ROM_CONST_VAR = "ERR: Invalid parameter";
        const char ret_INV_VERBOSE_LEVEL[] ROM_CONST_VAR = "ERR: Invalid verbose level: SET_VERBOSE 0 ... 3";

        constexpr size_t max_constexpr(size_t a) { return a; }

        template<typename... Args>
        constexpr size_t max_constexpr(size_t a, Args... rest)
        {
            return a > max_constexpr(rest...) ? a : max_constexpr(rest...);
        }

        static constexpr size_t kMaxResponseLen = max_constexpr(
            sizeof(ret_ERR_EEPROM) - 1U,
            sizeof(ret_INV_SIGNAL_IDX) - 1U,
            sizeof(ret_INV_SIGNAL_ID) - 1U,
            sizeof(ret_INV_FIRST_OUTPUT_TYPE) - 1U,
            sizeof(ret_INV_OUTPUT_CONFIG_STEP_SIZE) - 1U,
            sizeof(ret_INV_INPUT_TYPE) - 1U,
            sizeof(ret_INV_OUTPUT_PIN) - 1U,
            sizeof(ret_INV_INPUT_PIN) - 1U,
            sizeof(ret_INV_PARAM) - 1U,
            sizeof(ret_INV_VERBOSE_LEVEL) - 1U
        );

        static constexpr const char *responses[] ROM_CONST_VAR =
        {
            ret_ERR_EEPROM,                  // kIfcProjectBase + 0  (kErrEeprom)
            ret_INV_SIGNAL_IDX,              // kIfcProjectBase + 1  (kInvSignalIdx)
            ret_INV_SIGNAL_ID,               // kIfcProjectBase + 2  (kInvSignalId)
            ret_INV_FIRST_OUTPUT_TYPE,       // kIfcProjectBase + 3  (kInvFirstOutputType)
            ret_INV_OUTPUT_CONFIG_STEP_SIZE, // kIfcProjectBase + 4  (kInvOutputConfigStepSize)
            ret_INV_INPUT_TYPE,              // kIfcProjectBase + 5  (kInvInputType)
            ret_INV_OUTPUT_PIN,              // kIfcProjectBase + 6  (kInvOutputPin)
            ret_INV_INPUT_PIN,               // kIfcProjectBase + 7  (kInvInputPin)
            ret_INV_PARAM,                   // kIfcProjectBase + 8  (kInvParam)
            ret_INV_VERBOSE_LEVEL,           // kIfcProjectBase + 9  (kInvVerboseLevel)
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
            return rte::ifc_cal_set_defaults() ? kIfcOK : kErrEeprom;
        }

        static ret_type process_eto_set_signal(IfcAsciiCommandHandler::stringstream_type &st, IfcAsciiCommandHandler::string_type &response)
        {
            uint16 signal_idx;
            uint16 aspect;
            uint16 dim_time_10ms = 10;
            ret_type ret = kIfcInvCmd;

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
                    ret = kIfcOK;
                }
                else
                {
                    ret = kInvSignalIdx;
                }
            }

            return ret;
        }

        static ret_type process_set_signal(IfcAsciiCommandHandler::stringstream_type &st, IfcAsciiCommandHandler::string_type &response)
        {
            ret_type ret = kIfcInvCmd;
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
                    ret = kInvSignalIdx;
                }
                else if (!rte::sig::is_built_in(signal_id) && !rte::sig::is_user_defined(signal_id))
                {
                    ret = kInvSignalId;
                }
                else if ((output_type != cal::constants::kOnboard) && (output_type != cal::constants::kExternal))
                {
                    ret = kInvFirstOutputType;
                }
                else if ((step_size < -2) || (step_size > 2) || (step_size == 0))
                {
                    ret = kInvOutputConfigStepSize;
                }
                else if (input_type > cal::constants::kDig)
                {
                    ret = kInvInputType;
                }
                else if (first_output_pin >= platform::numeric_limits<uint8>::max_())
                {
                    ret = kInvOutputPin;
                }
                else if (input_pin >= platform::numeric_limits<uint8>::max_())
                {
                    ret = kInvInputPin;
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
                    ret = kIfcOK;
                }
            }

            return ret;
        }

        static ret_type process_get_signal(IfcAsciiCommandHandler::stringstream_type &st, IfcAsciiCommandHandler::string_type &response)
        {
            ret_type ret = kIfcInvCmd;
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
                    ret = kInvSignalIdx;
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
                    ret = kIfcOK;
                }
            }

            return ret;
        }

        static ret_type process_set_verbose(IfcAsciiCommandHandler::stringstream_type &st, IfcAsciiCommandHandler::string_type &response)
        {
            uint16 value;
            ret_type ret = kInvVerboseLevel;

            response.append(st.str());
            st >> value;
            if (!st.fail())
            {
                if (value <= debug::kVeryDetailed)
                {
                    debug::enable(static_cast<uint8>(value));
                    ret = kIfcOK;
                }
            }

            return ret;
        }

        static ret_type process_get_pin_config(IfcAsciiCommandHandler::stringstream_type &st, IfcAsciiCommandHandler::string_type &response)
        {
            uint16 pin;
            ret_type ret = kIfcInvCmd;

            response.append(st.str());
            st >> pin;
            if (!st.fail())
            {
                if (pin < cfg::kNrOnboardTargets)
                {
                    rte::sig::is_output_pin(pin) ? response.append(" OUTPUT") : response.append(" INPUT");
                    ret = kIfcOK;
                }
                else
                {
                    ret = kInvParam;
                }
            }

            return ret;
        }


    }

    IfcAsciiCommandHandler::ret_type SignalAsciiCommandHandler::process_command(
        const char *cmd, 
        stringstream_type &st, 
        string_type &sub_response)
    {
        size_t idx;
        ret_type ret = kIfcInvCmd;
        util::string_view sv(cmd);
        char cmd_rom[kMaxLenToken];

        for (idx = 0U; idx < commands.size(); idx++)
        {
            ROM_READ_STRING_N(cmd_rom, commands[idx].cmd, kMaxLenToken);
            if (sv.compare(cmd_rom) == 0)
            {
                ret = commands[idx].handler(st, sub_response);
                break;
            }
        }

        if (idx == commands.size())
        {
            // Command not found; ret is already set to kIfcInvCmd
            sub_response.append(cmd);
        }

        return ret;
    }

    void SignalAsciiCommandHandler::get_error_string(IfcAsciiCommandHandler::ret_type ret, string_type &dst)
    {
        if ((ret >= kIfcProjectBase) && 
            (static_cast<size_t>(ret - kIfcProjectBase) < util::size(responses)))
        {
            char buf[dst.max_size()];
            const char *ptr = static_cast<const char *>(ROM_READ_PTR(&responses[ret - kIfcProjectBase]));
            ROM_READ_STRING_N(buf, ptr, sizeof(buf));
            dst = buf;
        }
        else
        {
            // silently ignore invalid ret codes, AsciiCom will handle kOk and kIfcInvCmd itself.
        }
    }

} // namespace com
