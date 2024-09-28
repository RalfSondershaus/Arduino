/**
 * @file Signal/Cal/CalM.cpp
 *
 * @author Ralf Sondershaus
 *
 * @brief  Access to calibration parameters
 *
 * @copyright Copyright 2022 Ralf Sondershaus
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

#include <Cal/CalM.h>
#include <Rte/Rte.h>
#include <Arduino.h>
#include <EEPROM.h>

#include <Cal/CalM_config.h>

namespace cal
{
  /// CV Name                                     CV#    CV#       Required  Default  Read
  ///                                                    optional            Value    Only
  /// Decoder Address LSB                         1      513       M         1        Y     LSB of accessory decoder address
  /// Auxiliary Activation                        2      514       O                        Auxiliary activation of outputs
  /// Time On F1                                  3      515       O
  /// Time On F2                                  4      516       O
  /// Time On F3                                  5      517       O
  /// Time On F4                                  6      518       O
  /// Manufacturer Version Info                   7      519       M
  /// ManufacturerID                              8      520       M                  Y     Values assigned by NMRA
  /// Decoder Address MSB                         9      521       M         0        Y     3 MSB of accessory decoder address
  /// Bi-Directional Communication Configuration 28      540       O
  /// Accessory Decoder Configuration            29      541       M                        similar to CV#29; for acc. decoders
  /// Indexed Area Pointers                      31, 32                                     Index High and Low Address
  /// Manufacturer Unique                        112-128 (17 bytes)
  /// Manufacturer Unique                        129-256 (128 bytes)
  /// Manufacturer Unique                        513-895 (383 bytes)
  
  // 129        Checksum
  // 130 - 147	Signal 0
  // 148 - 165	Signal 1
  // 166 - 183	Signal 2
  // 184 - 201	Signal 3
  // 202 - 219	Signal 4
  // 220 - 237	Signal 5    108 bytes for 6 signals
  // 238 - 249	Classifier 0    
  // 250 - 261	Classifier 1
  // 262 - 273	Classifier 2
  // 274 - 285	Classifier 3
  // 286 - 297	Classifier 4
  // 298 - 309	Classifier 5     72 bytes for 6 classifiers

  /// Index type for EEPROM
  typedef uint16 nvm_block_id_type;

  /// EEPROM base address (129) for CRC
  static constexpr nvm_block_id_type kCrcBaseNvmId = 129U;

  /// EEPROM base address (130) and 18 bytes per signal
  static constexpr nvm_block_id_type kSignalBaseNvmId = 130U;
  static constexpr nvm_block_id_type kSignalLenNvm = 18U;

  /// EEPROM base address (238) and 12 bytes per classifier
  static constexpr nvm_block_id_type kInputClassifierBaseNvmId = 238U;
  static constexpr nvm_block_id_type kInputClassifierLenNvm = 12U;

  // first NVM index per signal
  static const util::array<nvm_block_id_type, cfg::kNrSignals> aSignalBaseNvmIdx = {
    kSignalBaseNvmId,                     // signal 1
    kSignalBaseNvmId + 1U * kSignalLenNvm,  // signal 2
    kSignalBaseNvmId + 2U * kSignalLenNvm,  // signal 3
    kSignalBaseNvmId + 3U * kSignalLenNvm,  // signal 4
    kSignalBaseNvmId + 4U * kSignalLenNvm,  // signal 5
    kSignalBaseNvmId + 5U * kSignalLenNvm   // signal 6
  };

  // first NVM index per classifier
  static const util::array<nvm_block_id_type, cfg::kNrSignals> aClassifierBaseNvmIdx = {
    kInputClassifierBaseNvmId,                                // 1
    kInputClassifierBaseNvmId + 1U * kInputClassifierLenNvm,  // 2
    kInputClassifierBaseNvmId + 2U * kInputClassifierLenNvm,  // 3
    kInputClassifierBaseNvmId + 3U * kInputClassifierLenNvm,  // 4
    kInputClassifierBaseNvmId + 4U * kInputClassifierLenNvm,  // 5
    kInputClassifierBaseNvmId + 5U * kInputClassifierLenNvm   // 6
  };

  // -----------------------------------------------
  /// Returns CRC8 from EEPROM addresses unStartNvmId ... unStartNvmId + unLen - 1.
  /// ucCrc is initial CRC value.
  // -----------------------------------------------
  uint8 calcSum(const void* src, uint16 unLen, uint8 ucCrc)
  {
    const uint8* pSrc = static_cast<const uint8*>(src);

    for (; unLen > 0U; unLen--)
    {
      ucCrc += *pSrc;
      pSrc++;
    }

    return ucCrc;
  }

  // -----------------------------------------------
  /// Constructor
  // -----------------------------------------------
  CalM::CalM()
    : signals{ CAL_SIGNAL_ARRAY }
    , input_classifiers{ CAL_INPUT_CLASSIFIER_CFG }
  {}

  // -----------------------------------------------
  /// Returns true if the EEPROM values are valid based on a checksum.
  // -----------------------------------------------
  bool CalM::isValid()
  {
    uint8 ucCrcNvm;
    uint8 ucCrc;

    ucCrcNvm = EEPROM.read(kCrcBaseNvmId);
    ucCrc = calcChecksum();

    return ucCrc == ucCrcNvm;
  }

  // -----------------------------------------------
  /// Returns checksum for all calibration data
  /// - signals
  /// - classifiers
  // -----------------------------------------------
  uint8 CalM::calcChecksum()
  {
    uint8 ucCrc = 0; // start value

    // signals
    for (auto it = signals.begin(); it < signals.end(); it++)
    {
      ucCrc = calcSum(it, sizeof(signal_type), ucCrc);
    }

    // classifiers
    for (auto it = input_classifiers.classifiers.begin(); it < input_classifiers.classifiers.end(); it++)
    {
      ucCrc = calcSum(it, sizeof(input_classifier_single_type), ucCrc);
    }

    return ucCrc;
  }

  // -----------------------------------------------
  /// Read values from EEPROM
  // -----------------------------------------------
  void CalM::readSignals()
  {
    nvm_block_id_type unNvmIdx; // local index
    auto base_nvm_it = aSignalBaseNvmIdx.begin(); // first (start) index (per signal)

    for (auto it = signals.begin(); it < signals.end(); it++)
    {
      unNvmIdx = *base_nvm_it;

      // byte 1: input
      it->input.raw = EEPROM.read(unNvmIdx); unNvmIdx++;

      // bytes 2 - 11: aspect and blinking
      for (auto aspect_it = it->aspects.begin(); aspect_it < it->aspects.end(); aspect_it++)
      {
        aspect_it->aspect = EEPROM.read(unNvmIdx); unNvmIdx++;
        aspect_it->blink = EEPROM.read(unNvmIdx); unNvmIdx++;
      }

      // bytes 12 - 16: targets
      for (auto target_it = it->targets.begin(); target_it < it->targets.end(); target_it++)
      {
        target_it->type = EEPROM.read(unNvmIdx); unNvmIdx++;
      }

      // bytes 17, 18: change over time and blink change over time
      it->changeOverTime = EEPROM.read(unNvmIdx); unNvmIdx++;
      it->blinkChangeOverTime = EEPROM.read(unNvmIdx);

      base_nvm_it++;
    }
  }

  // -----------------------------------------------
  /// Read values from EEPROM
  // -----------------------------------------------
  void CalM::readClassifiers()
  {
    nvm_block_id_type unNvmIdx; // local index
    auto base_nvm_it = aClassifierBaseNvmIdx.begin(); // first index

    for (auto it = input_classifiers.classifiers.begin(); it < input_classifiers.classifiers.end(); it++)
    {
      unNvmIdx = *base_nvm_it;

      // byte 1: pin
      it->ucPin = EEPROM.read(unNvmIdx); unNvmIdx++;

      // byte 2: debounce
      it->limits.ucDebounce = EEPROM.read(unNvmIdx); unNvmIdx++;

      // bytes 3 - 7: lower limits
      for (auto limit_it = it->limits.aucLo.begin(); limit_it < it->limits.aucLo.end(); limit_it++)
      {
        *limit_it = EEPROM.read(unNvmIdx); unNvmIdx++;
      }

      // bytes 8 - 12: upper limits
      for (auto limit_it = it->limits.aucHi.begin(); limit_it < it->limits.aucHi.end(); limit_it++)
      {
        *limit_it = EEPROM.read(unNvmIdx); unNvmIdx++;
      }

      base_nvm_it++;
    }
  }

  // -----------------------------------------------
  /// Save data to EEPROM if a value differs from the value already stored in the EEPROM.
  // -----------------------------------------------
  bool CalM::updateSignals()
  {
    nvm_block_id_type unNvmIdx; // local index
    auto base_nvm_it = aSignalBaseNvmIdx.begin(); // first index (per signal)

    for (auto it = signals.begin(); it < signals.end(); it++)
    {
      unNvmIdx = *base_nvm_it;

      // byte 1: input
      EEPROM.update(unNvmIdx, it->input.raw); unNvmIdx++;

      // bytes 2 - 11: aspect and blinking
      for (auto aspect_it = it->aspects.begin(); aspect_it < it->aspects.end(); aspect_it++)
      {
        EEPROM.update(unNvmIdx, aspect_it->aspect); unNvmIdx++;
        EEPROM.update(unNvmIdx, aspect_it->blink); unNvmIdx++;
      }

      // bytes 12 - 16: targets
      for (auto target_it = it->targets.begin(); target_it < it->targets.end(); target_it++)
      {
        EEPROM.update(unNvmIdx, target_it->type); unNvmIdx++;
      }

      // bytes 17, 18: change over time and blink change over time
      EEPROM.update(unNvmIdx, it->changeOverTime); unNvmIdx++;
      EEPROM.update(unNvmIdx, it->blinkChangeOverTime);

      base_nvm_it++;
    }

    return true;
  }

  // -----------------------------------------------
  /// Save data to EEPROM if a value differs from the value already stored in the EEPROM.
  // -----------------------------------------------
  bool CalM::updateClassifiers()
  {
    nvm_block_id_type unNvmIdx; // local index
    auto base_nvm_it = aClassifierBaseNvmIdx.begin(); // first index

    for (auto it = input_classifiers.classifiers.begin(); it < input_classifiers.classifiers.end(); it++)
    {
      unNvmIdx = *base_nvm_it;

      // byte 1: pin
      EEPROM.update(unNvmIdx, it->ucPin); unNvmIdx++;

      // byte 2: debounce
      EEPROM.update(unNvmIdx, it->limits.ucDebounce); unNvmIdx++;

      // bytes 3 - 7: lower limits
      for (auto limit_it = it->limits.aucLo.begin(); limit_it < it->limits.aucLo.end(); limit_it++)
      {
        EEPROM.update(unNvmIdx, *limit_it); unNvmIdx++;
      }

      // bytes 8 - 12: upper limits
      for (auto limit_it = it->limits.aucHi.begin(); limit_it < it->limits.aucHi.end(); limit_it++)
      {
        EEPROM.update(unNvmIdx, *limit_it); unNvmIdx++;
      }

      base_nvm_it++;
    }

    return true;
  }

  // -----------------------------------------------
  /// Read data from EEPROM
  // -----------------------------------------------
  bool CalM::readAll()
  {
    readSignals();
    readClassifiers();

    valid = isValid();
  }

  // -----------------------------------------------
  /// Save data to EEPROM if a value differs from the value already stored in the EEPROM.
  /// Validate the data after write.
  /// Returns true if successful, returns false otherwise.
  // -----------------------------------------------
  bool CalM::update()
  {
    updateSignals();
    updateClassifiers();

    // verify
    return readAll();
  }

  // -----------------------------------------------
  /// Store data in RAM. Doesn't not store data in EEPROM (call update() for this).
  /// Return true is successful, returns false otherwise.
  // -----------------------------------------------
  bool CalM::set_signal(uint8 ucSignalId, const signal_type& values)
  {
    bool bRet = false;

    if (signals.check_boundary(ucSignalId))
    {
      util::memcpy(&signals.at(ucSignalId), &values, sizeof(signal_type));
      calcLeds();
      bRet = true;
    }

    return bRet;
  }
  
  // -----------------------------------------------
  /// Store data in RAM. Doesn't not store data in EEPROM (call update() for this).
  /// Return true is successful, returns false otherwise.
  // -----------------------------------------------
  bool CalM::set_classifier(uint8 ucClassifierId, const input_classifier_single_type& values)
  {
    bool bRet = false;

    if (input_classifiers.classifiers.check_boundary(ucClassifierId))
    {
      util::memcpy(&input_classifiers.classifiers.at(ucClassifierId), &values, sizeof(input_classifier_single_type));
      bRet = true;
    }

    return bRet;
  }

  // -----------------------------------------------
  /// Calculate bit field: for each port, set a bit to 1 if the port is used by a LED
  /// or clear the bit to 0 if the port is not used by a LED.
  // -----------------------------------------------
  void CalM::calcLeds()
  {
    const signal_cal_type * pSignals = get_signal();
    if (pSignals != nullptr)
    {
      // set all bits to zero
      leds.reset();
      for (auto it = pSignals->begin(); it != pSignals->end(); it++)
      {
        for (auto tgtit = it->targets.begin(); tgtit != it->targets.end(); tgtit++)
        {
          if (tgtit->type == target_type::kOnboard)
          {
            // set idx-th bit to one
            leds.set(tgtit->idx);
          }
        }
      }
    }
  }

  // -----------------------------------------------
  /// Init runable
  // -----------------------------------------------
  void CalM::init()
  {
    if (readAll())
    {
      calcLeds();
    }
  }

  // -----------------------------------------------
  /// Cyclic runable
  // -----------------------------------------------
  void CalM::cycle100()
  {
  }

} // namespace cal
