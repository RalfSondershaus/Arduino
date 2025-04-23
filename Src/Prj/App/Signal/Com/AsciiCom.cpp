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
#include <Cal/CalM_Type.h>
#include <Com/AsciiCom.h>
#include <Util/Sstream.h>
#include <Util/String_view.h>
#include <Rte/Rte.h>

extern signal::InputClassifier rte::input_classifier;

namespace com
{
  using char_type = AsciiCom::char_type;
  using string_type = AsciiCom::string_type; // string of size 64
  using stringstream_type = util::basic_istringstream<SerAsciiTP::kMaxLenTelegram, char_type>;

    /// Return values of process() function family
    typedef enum
    {
      eOK = 0,                    ///< OK
      eINV_CMD,                   ///< Command invalid (or unknown)
      eERR_EEPROM,                ///< EEPROM update failure
      eINV_SIGNAL_ID,             ///< SIGNAL ID invalid
      eINV_SIGNAL_CMD,
      eINV_SIGNAL_ASPECTS,
      eINV_SIGNAL_BLINKS,
      eINV_SIGNAL_TARGETS,
      eINV_SIGNAL_INPUT,
      eINV_SIGNAL_CHANGEOVERTIME,
      eINV_CLASSIFIER_CMD,
      eINV_CLASSIFIER_PIN,
      eINV_CLASSIFIER_LIMITS,
      eINV_CLASSIFIER_DEBOUNCE,
      eINV_MONITOR_START,
      eERR_UNKNOWN
    } tRetType;

    /// For each tRetType, an error description that is transmitted after
    /// processing the command.
    static constexpr const string_type::value_type* aRetTypeStrings[] = 
    {
      "OK",                                   // eOK
      "ERR: Invalid command",                 // eINV_CMD
      "ERR: EEPROM failure",                  // eERR_EEPROM
      "ERR: Signal id invalid",               // eINV_SIGNAL_ID
      "ERR: Unknown signal sub command",      // eINV_SIGNAL_CMD
      "ERR: Unknown signal aspects",          // eINV_SIGNAL_ASPECTS
      "ERR: Unknown signal blinks",           // eINV_SIGNAL_BLINKS
      "ERR: Unknown signal targets",          // eINV_SIGNAL_TARGETS
      "ERR: Unknown signal input",            // eINV_SIGNAL_INPUT
      "ERR: Unknown signal change over time", // eINV_SIGNAL_CHANGEOVERTIME
      "ERR: Unknown classifier sub command GET_CLASSIFIER id [LIMITS slot-id,PIN,DEBOUNCE]",  // eINV_CLASSIFIER_CMD
      "ERR: Unknown classifier pin GET_CLASSIFIER id PIN",                            // eINV_CLASSIFIER_PIN
      "ERR: Unknown classifier limits GET_CLASSIFIER id LIMITS slot_id",              // eINV_CLASSIFIER_LIMITS
      "ERR: Unknown classifier debounce",     // eINV_CLASSIFIER_DEBOUNCE
      "ERR: Unknown monitor start MONITOR_START [classifier id]",           // eINV_MONITOR_START
      "ERR: unknown error"                    // has to be the last element
    };

  static uint8 convertStrToU8(util::string_view sv);

  static tRetType process_set_signal(stringstream_type& st, string_type& response);
  static tRetType process_get_signal(stringstream_type& st, string_type& response);
  static tRetType process_set_classifier(stringstream_type& st, string_type& response);
  static tRetType process_get_classifier(stringstream_type& st, string_type& response);
  static tRetType process_monitor_start(stringstream_type& st, string_type& response);
  static tRetType process_monitor_stop(stringstream_type& st, string_type& response);

  static int process_set_signal_aspects(stringstream_type& st, cal::signal_type& cal_sig);
  static int process_set_signal_blinks(stringstream_type& st, cal::signal_type& cal_sig);
  static int process_set_signal_targets(stringstream_type& st, cal::signal_type& cal_sig);
  static int process_set_signal_input(stringstream_type& st, cal::signal_type& cal_sig);
  static int process_set_signal_cot(stringstream_type& st, cal::signal_type& cal_sig);
 
  static tRetType process_get_signal_aspects(stringstream_type& st, cal::signal_type& cal_sig, string_type& response);
  static tRetType process_get_signal_blinks(stringstream_type& st, cal::signal_type& cal_sig, string_type& response);
  static tRetType process_get_signal_targets(stringstream_type& st, cal::signal_type& cal_sig, string_type& response);
  static tRetType process_get_signal_input(stringstream_type& st, cal::signal_type& cal_sig, string_type& response);
  static tRetType process_get_signal_cot(stringstream_type& st, cal::signal_type& cal_sig, string_type& response);
 
  static int process_set_classifier_pin(stringstream_type& st, cal::input_classifier_single_type& cal_cls);
  static int process_set_classifier_limits(stringstream_type& st, cal::input_classifier_single_type& cal_cls);
  static int process_set_classifier_debounce(stringstream_type& st, cal::input_classifier_single_type& cal_cls);

  static tRetType process_get_classifier_pin(stringstream_type& st, cal::input_classifier_single_type& cal_cls, string_type& response);
  static tRetType process_get_classifier_limits(stringstream_type& st, cal::input_classifier_single_type& cal_cls, string_type& response);
  static tRetType process_get_classifier_debounce(stringstream_type& st, cal::input_classifier_single_type& cal_cls, string_type& response);

  static int monitorClassified = -1;

  typedef tRetType (*func_type)(stringstream_type& st, string_type& response);

  typedef struct 
  {
    // Length of szCmd shall never exceed kMaxLenToken
    const char * szCmd;
    func_type func;
  } tCommands;

  /// Max length of a token (how many characters)
  static constexpr util::streamsize kMaxLenToken = 16;

  // Max Length of strings: kMaxLenToken
  const util::array<tCommands, 6> commands =
  { {
    { "SET_SIGNAL", process_set_signal },
    { "GET_SIGNAL", process_get_signal },
    { "SET_CLASSIFIER", process_set_classifier },
    { "GET_CLASSIFIER", process_get_classifier },
    { "MONITOR_START", process_monitor_start },
    { "MONITOR_STOP", process_monitor_stop }
  } };

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
      const string_type& telegram = asciiTP->getTelegram();
      
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
  void AsciiCom::process(const string_type& telegram, string_type& response)
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
  }

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

  // -----------------------------------------------------------------------------------
  /// <SET_SIGNAL> id [ASPECTS, BLINKS, TARGETS, INPUT, COT] ...
  // -----------------------------------------------------------------------------------
  static tRetType process_set_signal(stringstream_type& st, string_type& response)
  {
    uint16 id;
    char tmp[kMaxLenToken];
    tRetType ret;

    st >> id;
    const cal::signal_cal_type * pCalSig = rte::ifc_cal_signal::call();
    if ((pCalSig != nullptr) && (pCalSig->check_boundary(id)))
    {
      cal::signal_type cal_sig = pCalSig->at(id);
      st >> util::setw(kMaxLenToken) >> tmp;
      if (util::string_view{tmp}.compare("ASPECTS") == 0)
      {
        ret = (process_set_signal_aspects(st, cal_sig) > 0) ? eOK : eINV_SIGNAL_ASPECTS;
      }
      else if (util::string_view{tmp}.compare("BLINKS") == 0)
      {
        ret = (process_set_signal_blinks(st, cal_sig) > 0) ? eOK : eINV_SIGNAL_BLINKS;
      }
      else if (util::string_view{tmp}.compare("TARGETS") == 0)
      {
        ret = (process_set_signal_targets(st, cal_sig) > 0) ? eOK : eINV_SIGNAL_TARGETS;
      }
      else if (util::string_view{tmp}.compare("INPUT") == 0)
      {
        ret = (process_set_signal_input(st, cal_sig) > 0) ? eOK : eINV_SIGNAL_INPUT;
      }
      else if (util::string_view{tmp}.compare("COT") == 0)
      {
        ret = (process_set_signal_cot(st, cal_sig) > 0) ? eOK : eINV_SIGNAL_CHANGEOVERTIME;
      }
      else
      {
        ret = eINV_SIGNAL_CMD;
      }

      if (ret == eOK)
      {
        ret = (rte::ifc_cal_set_signal::call(id, cal_sig, true) == rte::ret_type::OK) ? eOK : eERR_EEPROM;
      }
    }
    else
    {
      ret = eINV_SIGNAL_ID;
    }
    return ret;
  }

  // -----------------------------------------------------------------------------------
  /// <SET_SIGNAL id TARGETS> ONBOARD pin ONBOARD pin ONB pin
  /// <SET_SIGNAL id TARGETS> EXTERN pin EXTERN pin EXT pin
  ///
  /// @return The number of converted pairs of ONBOARD pins.
  // -----------------------------------------------------------------------------------
  static int process_set_signal_targets(stringstream_type& st, cal::signal_type& cal_sig)
  {
    char tmp[kMaxLenToken]; // ONB or ONBOARD or EXT or EXTERN
    uint16 val;             // pin
    size_t idx = 0;         // count the pins
    cal::target_type sig_tgt;

    st >> util::setw(kMaxLenToken) >> tmp >> val;
    // Do not check for eof() since eof() is true after extracting the last element
    // (and if the last element doesn't have trailing white spaces).
    while (!st.fail() && cal_sig.targets.check_boundary(idx))
    {
      if (util::string_view{tmp}.compare("ONB") >= 0)
      {
        sig_tgt.type = cal::target_type::eOnboard;
        if (val < cfg::kNrOnboardTargets)
        {
          sig_tgt.idx = val;
        }
        else
        {
          // failure, stop, return error
          idx = 0;
          break;
        }
      }
      else if (util::string_view{tmp}.compare("EXT") >= 0)
      {
        sig_tgt.type = cal::target_type::eExternal;
        if (val < cfg::kNrExternalTargets)
        {
          sig_tgt.idx = val;
        }
        else
        {
          // failure, stop, return error
          idx = 0;
          break;
        }
      }
      else
      {
        // failure, stop, return error
        idx = 0;
        break;
      }
      cal_sig.targets[idx] = sig_tgt;
      idx++;
      st >> util::setw(kMaxLenToken) >> tmp >> val;
    }
    return idx;
  }

  // -----------------------------------------------------------------------------------
  /// <SET_SIGNAL id ASPECTS> 11000 00100 00110 11001 11111
  ///
  /// @return The number of converted bitfields (example above: 10)
  // -----------------------------------------------------------------------------------
  static int process_set_signal_aspects(stringstream_type& st, cal::signal_type& cal_sig)
  {
    char tmp[kMaxLenToken];
    size_t idx = 0;

    st >> util::setw(kMaxLenToken) >> tmp;
    // Do not check for eof() since eof() is true after extracting the last element
    // (and if the last element doesn't have trailing white spaces).
    while (!st.fail() && (cal_sig.aspects.check_boundary(idx)))
    {
      cal_sig.aspects[idx].aspect = convertStrToU8(tmp);
      idx++;
      st >> util::setw(kMaxLenToken) >> tmp;
    }
    return idx;
  }

  // -----------------------------------------------------------------------------------
  /// <SET_SIGNAL id BLINKS> 00000 00000 00000 00000 00000
  ///
  /// @return The number of converted bitfields (example above: 10)
  // -----------------------------------------------------------------------------------
  static int process_set_signal_blinks(stringstream_type& st, cal::signal_type& cal_sig)
  {
    char tmp[kMaxLenToken];
    size_t idx = 0;

    st >> util::setw(kMaxLenToken) >> tmp;
    // Do not check for eof() since eof() is true after extracting the last element
    // (and if the last element doesn't have trailing white spaces).
    while (!st.fail() && (cal_sig.aspects.check_boundary(idx)))
    {
      cal_sig.aspects[idx].blink = convertStrToU8(tmp);
      idx++;
      st >> util::setw(kMaxLenToken) >> tmp;
    }
    return idx;
  }

  // -----------------------------------------------------------------------------------
  /// <SET_SIGNAL id INPUT> CLASSIFIED id
  ///
  /// @return 1: success, 0: no success
  // -----------------------------------------------------------------------------------
  static int process_set_signal_input(stringstream_type& st, cal::signal_type& cal_sig)
  {
    char tmp[kMaxLenToken];
    size_t idx = 0;
    uint16 val;

    st >> util::setw(kMaxLenToken) >> tmp >> val;
    // Do not check for eof() since eof() is true after extracting the last element
    // (and if the last element doesn't have trailing white spaces).
    if (!st.fail() && (val < cfg::kNrClassifiers))
    {
      if (util::string_view{tmp}.compare("CLASS") >= 0)
      {
        cal_sig.input.bits.type = cal::input_type::eClassified;
        cal_sig.input.bits.idx = val;
        idx++;
      }
    }
    return idx;
  }

  // -----------------------------------------------------------------------------------
  /// <SET SIGNAL id COT> t1 t2
  ///
  /// @return 1: success, 0: no success
  // -----------------------------------------------------------------------------------
  static int process_set_signal_cot(stringstream_type& st, cal::signal_type& cal_sig)
  {
    size_t idx = 0;
    uint16 t1, t2;

    st >> t1 >> t2;
    // Do not check for eof() since eof() is true after extracting the last element
    // (and if the last element doesn't have trailing white spaces).
    if (!st.fail())
    {
      cal_sig.changeOverTime = t1;
      cal_sig.blinkChangeOverTime = t2;
      idx++;
    }
    return idx;
  }

  // -----------------------------------------------------------------------------------
  /// <GET_SIGNAL> id [ASPECTS, BLINKS, TARGETS, INPUT, COT]
  // -----------------------------------------------------------------------------------
  static tRetType process_get_signal(stringstream_type& st, string_type& response)
  {
    uint16 id;
    char tmp[kMaxLenToken];
    tRetType ret;

    st >> id;
    const cal::signal_cal_type * pCalSig = rte::ifc_cal_signal::call();
    if ((pCalSig != nullptr) && (pCalSig->check_boundary(id)))
    {
      cal::signal_type cal_sig = pCalSig->at(id);
      st >> util::setw(kMaxLenToken) >> tmp;
      if (util::string_view{tmp}.compare("ASPECTS") == 0)
      {
        ret = process_get_signal_aspects(st, cal_sig, response);
      }
      else if (util::string_view{tmp}.compare("BLINKS") == 0)
      {
        ret = process_get_signal_blinks(st, cal_sig, response);
      }
      else if (util::string_view{tmp}.compare("TARGETS") == 0)
      {
        ret = process_get_signal_targets(st, cal_sig, response);
      }
      else if (util::string_view{tmp}.compare("INPUT") == 0)
      {
        ret = process_get_signal_input(st, cal_sig, response);
      }
      else if (util::string_view{tmp}.compare("COT") == 0)
      {
        ret = process_get_signal_cot(st, cal_sig, response);
      }
      else
      {
        ret = eINV_SIGNAL_CMD;
      }
    }
    else
    {
      ret = eINV_SIGNAL_ID;
    }
    return ret;
  }

  // -----------------------------------------------------------------------------------
  /// <GET_SIGNAL id TARGETS>
  ///
  /// Response: ONBOARD pin ONBOARD pin ONBOARD pin ...
  /// Response: EXTERN pin EXTERN pin EXTERN pin ...
  ///
  /// @return eOK
  // -----------------------------------------------------------------------------------
  static tRetType process_get_signal_targets(stringstream_type& st, cal::signal_type& cal_sig, string_type& response)
  {
    util::basic_string<4, char_type> tmp;
    for (auto cit = cal_sig.targets.begin(); cit != cal_sig.targets.end(); cit++)
    {
      switch (cit->type)
      {
        case cal::target_type::eOnboard: response.append("ONBOARD "); break;
        case cal::target_type::eExternal: response.append("EXTERN "); break;
        default: response.append("NONE "); break;
      }
      util::to_string(cit->idx, tmp);
      response.append(tmp);
      response.append(" ");
    }

    return eOK;
  }

  // -----------------------------------------------------------------------------------
  /// <GET_SIGNAL id ASPECTS>
  ///
  /// Response: 11000 00100 00000 ...
  ///
  /// @return eOK
  // -----------------------------------------------------------------------------------
  static tRetType process_get_signal_aspects(stringstream_type& st, cal::signal_type& cal_sig, string_type& response)
  {
    for (auto cit = cal_sig.aspects.begin(); cit != cal_sig.aspects.end(); cit++)
    {
      response.append(convertU8ToStr(cit->aspect));
      response.append(" ");
    }

    return eOK;
  }

  // -----------------------------------------------------------------------------------
  /// <GET_SIGNAL id BLINKS>
  ///
  /// Response: 00000 00000 00000 ...
  ///
  /// @return eOK
  // -----------------------------------------------------------------------------------
  static tRetType process_get_signal_blinks(stringstream_type& st, cal::signal_type& cal_sig, string_type& response)
  {
    for (auto cit = cal_sig.aspects.begin(); cit != cal_sig.aspects.end(); cit++)
    {
      response.append(convertU8ToStr(cit->blink));
      response.append(" ");
    }

    return eOK;
  }

  // -----------------------------------------------------------------------------------
  /// <GET_SIGNAL id INPUT>
  ///
  /// Response:  CLASSIFIED classifier-id
  ///
  /// @return eOK
  // -----------------------------------------------------------------------------------
  static tRetType process_get_signal_input(stringstream_type& st, cal::signal_type& cal_sig, string_type& response)
  {
    util::basic_string<4, char_type> tmp;

    switch (cal_sig.input.bits.type)
    {
      case cal::input_type::eClassified: response.append("CLASSIFIED "); break;
      default: response.append("NONE "); break;
    }

    util::to_string(cal_sig.input.bits.idx, tmp);
    response.append(tmp);

    return eOK;
  }

  // -----------------------------------------------------------------------------------
  /// <GET_SIGNAL id COT>
  ///
  /// Response:  t1 t2
  ///
  /// @return eOK
  // -----------------------------------------------------------------------------------
  static tRetType process_get_signal_cot(stringstream_type& st, cal::signal_type& cal_sig, string_type& response)
  {
    util::basic_string<3, char_type> tmp;

    util::to_string(cal_sig.changeOverTime, tmp);
    response.append(tmp);
    response.append(" ");
    util::to_string(cal_sig.blinkChangeOverTime, tmp);
    response.append(tmp);

    return eOK;
  }

  // -----------------------------------------------------------------------------------
  /// <SET_CLASSIFIER> id [PIN, LIMITS, DEBOUNCE] ...
  ///
  /// Response:  OK
  ///
  /// @return eOK
  // -----------------------------------------------------------------------------------
  static tRetType process_set_classifier(stringstream_type& st, string_type& response)
  {
    uint16 id;
    char tmp[kMaxLenToken];
    tRetType ret;

    st >> id;
    const cal::input_classifier_cal_type * pCalCls = rte::ifc_cal_input_classifier::call();
    if ((pCalCls != nullptr) && (pCalCls->classifiers.check_boundary(id)))
    {
      cal::input_classifier_single_type cal_cls = pCalCls->classifiers.at(id);
      st >> util::setw(kMaxLenToken) >> tmp;
      if (util::string_view{tmp}.compare("PIN") == 0)
      {
        ret = (process_set_classifier_pin(st, cal_cls) > 0) ? eOK : eINV_CLASSIFIER_PIN;
      }
      else if (util::string_view{tmp}.compare("LIMITS") == 0)
      {
        ret = (process_set_classifier_limits(st, cal_cls) > 0) ? eOK : eINV_CLASSIFIER_LIMITS;
      }
      else if (util::string_view{tmp}.compare("DEBOUNCE") == 0)
      {
        ret = (process_set_classifier_debounce(st, cal_cls) > 0) ? eOK : eINV_CLASSIFIER_DEBOUNCE;
      }
      else
      {
        ret = eINV_SIGNAL_CMD;
      }

      if (ret == eOK)
      {
        ret = (rte::ifc_cal_set_input_classifier::call(id, cal_cls, true) == rte::ret_type::OK) ? eOK : eERR_EEPROM;
      }
    }
    else
    {
      ret = eINV_SIGNAL_ID;
    }
    return ret;
  }

  // -----------------------------------------------------------------------------------
  /// <SET_CLASSIFIER id PIN> pin
  ///
  /// @return 0 (failure) or 1 (ok)
  // -----------------------------------------------------------------------------------
  static int process_set_classifier_pin(stringstream_type& st, cal::input_classifier_single_type& cal_cls)
  {
    int ret = 0;
    uint16 val;

    st >> val;
    // Do not check for eof() since eof() is true after extracting the last element
    // (and if the last element doesn't have trailing white spaces).
    if (!st.fail())
    {
      cal_cls.ucPin = static_cast<uint16>(util::min_(static_cast<uint16>(platform::numeric_limits<uint8>::max_()), val));
      ret = 1;
    }

    return ret;
  }

  // -----------------------------------------------------------------------------------
  /// <SET_CLASSIFIER id LIMITS> slot-id min max
  ///
  /// @return 0 (failure) or 1 (ok)
  // -----------------------------------------------------------------------------------
  static int process_set_classifier_limits(stringstream_type& st, cal::input_classifier_single_type& cal_cls)
  {
    int ret = 0;
    uint16 slot_id;
    uint16 val_min;
    uint16 val_max;

    st >> slot_id;
    // Do not check for eof() since eof() is true after extracting the last element
    // (and if the last element doesn't have trailing white spaces).
    if (!st.fail() && cal_cls.limits.aucLo.check_boundary(slot_id))
    {
      st >> val_min >> val_max;
      if (!st.fail())
      {
        cal_cls.limits.aucLo[slot_id] = static_cast<uint16>(util::min_(static_cast<uint16>(platform::numeric_limits<uint8>::max_()), val_min));
        cal_cls.limits.aucHi[slot_id] = static_cast<uint16>(util::min_(static_cast<uint16>(platform::numeric_limits<uint8>::max_()), val_max));
        ret = 1;
      }
    }

    return ret;
  }

  // -----------------------------------------------------------------------------------
  /// <SET_CLASSIFIER id DEBOUNCE> time
  ///
  /// @return 0 (failure) or 1 (ok)
  // -----------------------------------------------------------------------------------
  static int process_set_classifier_debounce(stringstream_type& st, cal::input_classifier_single_type& cal_cls)
  {
    int ret = 0;
    uint16 val;

    st >> val;
    // Do not check for eof() since eof() is true after extracting the last element
    // (and if the last element doesn't have trailing white spaces).
    if (!st.fail())
    {
      cal_cls.limits.ucDebounce = static_cast<uint16>(util::min_(static_cast<uint16>(platform::numeric_limits<uint8>::max_()), val));
      ret = 1;
    }

    return ret;
  }

  // -----------------------------------------------------------------------------------
  /// <GET_CLASSIFIER> id [PIN, LIMITS, DEBOUNCE]
  ///
  /// @return eOK
  // -----------------------------------------------------------------------------------
  static tRetType process_get_classifier(stringstream_type& st, string_type& response)
  {
    uint16 id;
    char tmp[kMaxLenToken];
    tRetType ret;

    st >> id;
    const cal::input_classifier_cal_type * pCalCls = rte::ifc_cal_input_classifier::call();
    if ((pCalCls != nullptr) && (pCalCls->classifiers.check_boundary(id)))
    {
      cal::input_classifier_single_type cal_cls = pCalCls->classifiers.at(id);
      st >> util::setw(kMaxLenToken) >> tmp;
      if (util::string_view{tmp}.compare("PIN") == 0)
      {
        ret = process_get_classifier_pin(st, cal_cls, response);
      }
      else if (util::string_view{tmp}.compare("LIMITS") == 0)
      {
        ret = process_get_classifier_limits(st, cal_cls, response);
      }
      else if (util::string_view{tmp}.compare("DEBOUNCE") == 0)
      {
        ret = process_get_classifier_debounce(st, cal_cls, response);
      }
      else
      {
        ret = eINV_SIGNAL_CMD;
      }
    }
    else
    {
      ret = eINV_SIGNAL_ID;
    }
    return ret;

  }

  // -----------------------------------------------------------------------------------
  /// <GET_CLASSIFIER id PIN> pin
  ///
  /// @return eOK
  // -----------------------------------------------------------------------------------
  static tRetType process_get_classifier_pin(stringstream_type& st, cal::input_classifier_single_type& cal_cls, string_type& response)
  {
    util::basic_string<3, char_type> tmp;

    util::to_string(cal_cls.ucPin, tmp);
    response.append(tmp);

    return eOK;
  }
  
  // -----------------------------------------------------------------------------------
  /// <GET_CLASSIFIER id LIMITS> slot_id
  ///
  /// @return eOK, eINV_CLASSIFIER_LIMITS
  // -----------------------------------------------------------------------------------
  static tRetType process_get_classifier_limits(stringstream_type& st, cal::input_classifier_single_type& cal_cls, string_type& response)
  {
    util::basic_string<3, char_type> tmp;
    tRetType ret;
    uint16 slot_id;

    st >> slot_id;
    // Do not check for eof() since eof() is true after extracting the last element
    // (and if the last element doesn't have trailing white spaces).
    if (!st.fail() && cal_cls.limits.aucLo.check_boundary(slot_id))
    {
      util::to_string(cal_cls.limits.aucLo[slot_id], tmp);
      response.append(tmp);
      response.append(" ");
      util::to_string(cal_cls.limits.aucHi[slot_id], tmp);
      response.append(tmp);
      ret = eOK;
    }
    else
    {
      ret = eINV_CLASSIFIER_LIMITS;
    }

    return ret;

  }

  // -----------------------------------------------------------------------------------
  /// <GET_CLASSIFIER id LIMITS> slot_id
  ///
  /// @return eOK, eINV_CLASSIFIER_DEBOUNCE
  // -----------------------------------------------------------------------------------
  static tRetType process_get_classifier_debounce(stringstream_type& st, cal::input_classifier_single_type& cal_cls, string_type& response)
  {
    util::basic_string<3, char_type> tmp;

    util::to_string(cal_cls.limits.ucDebounce, tmp);
    response.append(tmp);

    return eOK;
  }

  // -----------------------------------------------------------------------------------
  /// <MONITOR_START> classifier classifier-id
  ///
  /// @return eOK
  // -----------------------------------------------------------------------------------
  static tRetType process_monitor_start(stringstream_type& st, string_type& response)
  {
    char tmp[kMaxLenToken];
    tRetType ret;
    st >> tmp;
    if (!st.fail())
    {
      if (util::string_view{tmp}.compare("classifier") == 0)
      {
        uint16 idx;
        st >> idx;
        if (!st.fail())
        {
          monitorClassified = static_cast<int>(idx);
          ret = eOK;
        }
        else
        {
          ret = eINV_MONITOR_START;
        }
      }
      else
      {
        ret = eINV_MONITOR_START;
      }
    }
    else
    {
      ret = eINV_MONITOR_START;
    }
    return ret;
  }
  static tRetType process_monitor_stop(stringstream_type& st, string_type& response)
  {
    char tmp[kMaxLenToken];
    tRetType ret;
    st >> tmp;
    if (!st.fail())
    {
      if (util::string_view{tmp}.compare("classifier") == 0)
      {
        monitorClassified = -1;
        ret = eOK;
      }
      else
      {
        ret = eINV_MONITOR_START;
      }
    }
    else
    {
      ret = eINV_MONITOR_START;
    }
    return ret;
  }

} // namespace com
