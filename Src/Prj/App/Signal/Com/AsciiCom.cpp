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

namespace com
{
  using char_type = AsciiCom::char_type;
  using stringstream_type = util::basic_istringstream<SerAsciiTP::kMaxLenTelegram, char_type>;

  static uint8 convertStrToU8(util::string_view sv);

  static AsciiCom::tRetType process_set_signal(stringstream_type& st);

  static int process_set_signal_aspects(stringstream_type& st, cal::signal_type& cal_sig);
  static int process_set_signal_blinks(stringstream_type& st, cal::signal_type& cal_sig);
  static int process_set_signal_targets(stringstream_type& st, cal::signal_type& cal_sig);
  static int process_set_signal_input(stringstream_type& st, cal::signal_type& cal_sig);
  static int process_set_signal_cot(stringstream_type& st, cal::signal_type& cal_sig);
 
  typedef AsciiCom::tRetType (*func_type)(stringstream_type& st);

  typedef struct 
  {
    // Length of szCmd shall never exceed kMaxLenToken
    const char * szCmd;
    func_type func;
  } tCommands;

  /// Max length of a token (how many characters)
  static constexpr util::streamsize kMaxLenToken = 16;

  const util::array<tCommands, 1> commands =
  {
    { "SET_SIGNAL", process_set_signal },
  };

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
      const tRetType ret = process(telegram);
      if (static_cast<size_t>(ret) < sizeof(aRetTypeStrings) / sizeof(const string_type::value_type*))
      {
        asciiTP->transmitTelegram(aRetTypeStrings[static_cast<size_t>(ret)]);
      }
      else
      {
        asciiTP->transmitTelegram(aRetTypeStrings[static_cast<size_t>(eERR_UNKNOWN)]);
      }
    }
  }
  
  // -----------------------------------------------------------------------------------
  /// SET_SIGNAL ...
  // -----------------------------------------------------------------------------------
  AsciiCom::tRetType AsciiCom::process(const string_type& telegram)
  {
    stringstream_type st(telegram);
    char cmd[kMaxLenToken];
    tRetType ret = eINV_CMD;
   
    st >> util::setw(kMaxLenToken) >> cmd;
    util::string_view sv(cmd);
    for (auto cmdit = commands.begin(); cmdit != commands.end(); cmdit++)
    {
      if (sv.compare(cmdit->szCmd) == 0)
      {
        ret = cmdit->func(st);
        break;
      }
    }

    return ret;
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
  /// <SET_SIGNAL> id [ASPECTS, BLINKS, TARGETS, INPUT, COT] ...
  // -----------------------------------------------------------------------------------
  static AsciiCom::tRetType process_set_signal(stringstream_type& st)
  {
    uint16 id;
    char tmp[kMaxLenToken];
    AsciiCom::tRetType ret;

    st >> id;
    const cal::signal_cal_type * pCalSig = rte::ifc_cal_signal::call();
    if ((pCalSig != nullptr) && (pCalSig->check_boundary(id)))
    {
      cal::signal_type cal_sig = pCalSig->at(id);
      st >> util::setw(kMaxLenToken) >> tmp;
      if (util::string_view{tmp}.compare("ASPECTS") == 0)
      {
        ret = (process_set_signal_aspects(st, cal_sig) > 0) ? AsciiCom::eOK : AsciiCom::eINV_SIGNAL_ASPECTS;
      }
      else if (util::string_view{tmp}.compare("BLINKS") == 0)
      {
        ret = (process_set_signal_blinks(st, cal_sig) > 0) ? AsciiCom::eOK : AsciiCom::eINV_SIGNAL_BLINKS;
      }
      else if (util::string_view{tmp}.compare("TARGETS") == 0)
      {
        ret = (process_set_signal_targets(st, cal_sig) > 0) ? AsciiCom::eOK : AsciiCom::eINV_SIGNAL_TARGETS;
      }
      else if (util::string_view{tmp}.compare("INPUT") == 0)
      {
        ret = (process_set_signal_input(st, cal_sig) > 0) ? AsciiCom::eOK : AsciiCom::eINV_SIGNAL_INPUT;
      }
      else if (util::string_view{tmp}.compare("COT") == 0)
      {
        ret = (process_set_signal_cot(st, cal_sig) > 0) ? AsciiCom::eOK : AsciiCom::eINV_SIGNAL_CHANGEOVERTIME;
      }
      else
      {
        ret = AsciiCom::eINV_SIGNAL_CMD;
      }

      if (ret == AsciiCom::eOK)
      {
        ret = (rte::ifc_cal_set_signal::call(id, cal_sig, true) == rte::ret_type::OK) ? AsciiCom::eOK : AsciiCom::eERR_EEPROM;
      }
    }
    else
    {
      ret = AsciiCom::eINV_SIGNAL_ID;
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
      if (util::string_view{tmp}.compare("ONBOARD") == 0)
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
      else if (util::string_view{tmp}.compare("EXTERN") == 0)
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
      if (util::string_view{tmp}.compare("CLASSIFIED") == 0)
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

} // namespace com
