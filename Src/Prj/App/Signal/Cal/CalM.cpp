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
#include <Hal/EEPROM.h>

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

  namespace eeprom
  {
    namespace default_values
    {
      static const signal_cal_type signals{ CAL_SIGNAL_ARRAY };
      static const input_classifier_cal_type input_classifiers{ CAL_INPUT_CLASSIFIER_CFG };
      static const base_cv_cal_type base_cv{ CAL_BASE_CV_CFG };
    }
  }

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
    , base_cv{ CAL_BASE_CV_CFG }
    // member element leds is calculated in init() runable
  {}

  // -----------------------------------------------
  /// Returns true if the EEPROM values are valid.
  /// That is, if Manufacturer ID is not the EEPROM initial value
  /// (EEPROM not programmed yet).
  // -----------------------------------------------
  bool CalM::isValid()
  {
    return hal::eeprom::read(eeprom::eManufacturerID) != hal::eeprom::kInitial;
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
  /// Initialize internal data structures and EEPROM values with default values
  /// from ROM.    
  // -----------------------------------------------
  void CalM::initBaseCV()
  {
    util::memcpy(&base_cv, &eeprom::default_values::base_cv, sizeof(base_cv_cal_type));
    
    (void)updateBaseCV();
  }

  // -----------------------------------------------
  /// Initialize internal data structures and EEPROM values with default values
  /// from ROM.    
  // -----------------------------------------------
  void CalM::initSignals()
  {
    const auto it_begin = eeprom::default_values::signals.begin();
    for (auto it = it_begin; it < eeprom::default_values::signals.end(); it++)
    {
      set_signal(it - it_begin, *it);
    }

    (void)updateSignals();
  }

  // -----------------------------------------------
  /// Initialize internal data structures and EEPROM values with default values
  /// from ROM.    
  // -----------------------------------------------
  void CalM::initClassifiers()
  {
    const auto it_begin = eeprom::default_values::input_classifiers.classifiers.begin();
    for (auto it = it_begin; it < eeprom::default_values::input_classifiers.classifiers.end(); it++)
    {
      set_input_classifier(it - it_begin, *it);
    }

    (void)updateClassifiers();
  }

  // -----------------------------------------------
  /// Read values from EEPROM
  // -----------------------------------------------
  void CalM::readBaseCV()
  {
    base_cv.AddressLSB = hal::eeprom::read(eeprom::eDecoderAddressLSB);
    base_cv.AddressMSB = hal::eeprom::read(eeprom::eDecoderAddressMSB);
    base_cv.AuxAct = hal::eeprom::read(eeprom::eAuxiliaryActivattion);
    for (int i = 0; i < 4; i++)
    {
      base_cv.TimeOn[i] = hal::eeprom::read(eeprom::eTimeOnBase + i);
    }
    base_cv.ManufacturerID = hal::eeprom::read(eeprom::eManufacturerID);
    base_cv.ManufacturerVersionID = hal::eeprom::read(eeprom::eManufacturerVersionID);
    base_cv.Configuration = hal::eeprom::read(eeprom::eConfiguration);
  }

  // -----------------------------------------------
  /// Read values from EEPROM and store them in member variable signals.
  // -----------------------------------------------
  void CalM::readSignals()
  {
    uint16 unEepIdx = eeprom::eSignalBase; // local index, starts with first signal

    for (auto it = signals.begin(); it < signals.end(); it++)
    {
      // byte 1: input
      it->input.raw = hal::eeprom::read(unEepIdx); unEepIdx++;

      // bytes 2 - 11: aspect and blinking
      for (auto aspect_it = it->aspects.begin(); aspect_it < it->aspects.end(); aspect_it++)
      {
        aspect_it->aspect = hal::eeprom::read(unEepIdx); unEepIdx++;
        aspect_it->blink = hal::eeprom::read(unEepIdx); unEepIdx++;
      }

      // bytes 12 - 16: targets
      for (auto target_it = it->targets.begin(); target_it < it->targets.end(); target_it++)
      {
        uint8 val = hal::eeprom::read(unEepIdx); unEepIdx++;
        target_it->type = util::bits::extract(val, 0, 2);
        target_it->idx  = util::bits::extract(val, 2, 6);
      }

      // bytes 17, 18: change over time and blink change over time
      it->changeOverTime = hal::eeprom::read(unEepIdx); unEepIdx++;
      it->blinkChangeOverTime = hal::eeprom::read(unEepIdx); unEepIdx++;
    }
  }

  // -----------------------------------------------
  /// Read values from EEPROM and store them in member variable input_classifiers.
  // -----------------------------------------------
  void CalM::readClassifiers()
  {
    uint16 unEepIdx = eeprom::eClassifierBase; // local index, starts with first classifier

    for (auto it = input_classifiers.classifiers.begin(); it < input_classifiers.classifiers.end(); it++)
    {
      // byte 1: pin
      it->ucPin = hal::eeprom::read(unEepIdx); unEepIdx++;

      // byte 2: debounce
      it->limits.ucDebounce = hal::eeprom::read(unEepIdx); unEepIdx++;

      // bytes 3 - 7: lower limits
      for (auto limit_it = it->limits.aucLo.begin(); limit_it < it->limits.aucLo.end(); limit_it++)
      {
        *limit_it = hal::eeprom::read(unEepIdx); unEepIdx++;
      }

      // bytes 8 - 12: upper limits
      for (auto limit_it = it->limits.aucHi.begin(); limit_it < it->limits.aucHi.end(); limit_it++)
      {
        *limit_it = hal::eeprom::read(unEepIdx); unEepIdx++;
      }
    }
  }

  // -----------------------------------------------
  /// Save data to EEPROM if a value differs from the value already stored in the EEPROM.
  // -----------------------------------------------
  bool CalM::updateBaseCV()
  {
    hal::eeprom::update(eeprom::eDecoderAddressLSB, base_cv.AddressLSB);
    hal::eeprom::update(eeprom::eDecoderAddressMSB, base_cv.AddressMSB);
    hal::eeprom::update(eeprom::eAuxiliaryActivattion, base_cv.AuxAct);
    for (int i = 0; i < 4; i++)
    {
      hal::eeprom::update(eeprom::eTimeOnBase + i, base_cv.TimeOn[i]);
    }
    hal::eeprom::update(eeprom::eManufacturerID, base_cv.ManufacturerID);
    hal::eeprom::update(eeprom::eManufacturerVersionID, base_cv.ManufacturerVersionID);
    hal::eeprom::update(eeprom::eConfiguration, base_cv.Configuration);

    return true;
  }

  // -----------------------------------------------
  /// Save data to EEPROM if a value differs from the value already stored in the EEPROM.
  // -----------------------------------------------
  bool CalM::updateSignals()
  {
    uint16 unEepIdx = eeprom::eSignalBase;

    for (auto it = signals.begin(); it < signals.end(); it++)
    {
      // byte 1: input
      hal::eeprom::update(unEepIdx, it->input.raw); unEepIdx++;

      // bytes 2 - 11: aspect and blinking
      for (auto aspect_it = it->aspects.begin(); aspect_it < it->aspects.end(); aspect_it++)
      {
        hal::eeprom::update(unEepIdx, aspect_it->aspect); unEepIdx++;
        hal::eeprom::update(unEepIdx, aspect_it->blink); unEepIdx++;
      }

      // bytes 12 - 16: targets
      for (auto target_it = it->targets.begin(); target_it < it->targets.end(); target_it++)
      {
        uint8 val = (target_it->idx << 2) | target_it->type;
        hal::eeprom::update(unEepIdx, val); unEepIdx++;
      }

      // bytes 17, 18: change over time and blink change over time
      hal::eeprom::update(unEepIdx, it->changeOverTime); unEepIdx++;
      hal::eeprom::update(unEepIdx, it->blinkChangeOverTime); unEepIdx++;
    }

    return true;
  }

  // -----------------------------------------------
  /// Save data to EEPROM if a value differs from the value already stored in the EEPROM.
  // -----------------------------------------------
  bool CalM::updateClassifiers()
  {
    uint16 unEepIdx = eeprom::eClassifierBase;

    for (auto it = input_classifiers.classifiers.begin(); it < input_classifiers.classifiers.end(); it++)
    {
      // byte 1: pin
      hal::eeprom::update(unEepIdx, it->ucPin); unEepIdx++;

      // byte 2: debounce
      hal::eeprom::update(unEepIdx, it->limits.ucDebounce); unEepIdx++;

      // bytes 3 - 7: lower limits
      for (auto limit_it = it->limits.aucLo.begin(); limit_it < it->limits.aucLo.end(); limit_it++)
      {
        hal::eeprom::update(unEepIdx, *limit_it); unEepIdx++;
      }

      // bytes 8 - 12: upper limits
      for (auto limit_it = it->limits.aucHi.begin(); limit_it < it->limits.aucHi.end(); limit_it++)
      {
        hal::eeprom::update(unEepIdx, *limit_it); unEepIdx++;
      }
    }

    return true;
  }

  // -----------------------------------------------
  /// Initialize internal data structures and EEPROM values with default values
  /// from ROM.    
  // -----------------------------------------------
  void CalM::initAll()
  {
    initBaseCV();
    initSignals();
    initClassifiers();
  }

  // -----------------------------------------------
  /// Read data from EEPROM
  // -----------------------------------------------
  bool CalM::readAll()
  {
    readBaseCV();
    readSignals();
    readClassifiers();
    return isValid();
  }

  // -----------------------------------------------
  /// Save data to EEPROM if a value differs from the value already stored in the EEPROM.
  /// Validate the data after write.
  /// Returns true if successful, returns false otherwise.
  // -----------------------------------------------
  bool CalM::update()
  {
    updateBaseCV();
    updateSignals();
    updateClassifiers();

    // verify
    return readAll();
  }

  // -----------------------------------------------
  /// Server function: Store data in RAM. If doUpdate is true, stores data
  /// in EEPROM. Otherwise, call update() for this.
  /// Return OK is successful, returns NOK otherwise.
  // -----------------------------------------------
  rte::ret_type CalM::set_signal(uint8 ucSignalId, const signal_type& values, bool doUpdate)
  {
    rte::ret_type ret = rte::ret_type::NOK;

    if (signals.check_boundary(ucSignalId))
    {
      util::memcpy(&signals.at(ucSignalId), &values, sizeof(signal_type));
      calcLeds();
      if (doUpdate)
      {
        ret = updateSignals() ? rte::ret_type::OK : rte::ret_type::NOK;
      }
      else
      {
        ret = rte::ret_type::OK;
      }
    }

    return ret;
  }
  
  // -----------------------------------------------
  /// Server function: Store data in RAM. If doUpdate is true, stores data
  /// in EEPROM. Otherwise, call update() for this.
  /// Return OK is successful, returns NOK otherwise.
  // -----------------------------------------------
  rte::ret_type CalM::set_input_classifier(uint8 ucClassifierId, const input_classifier_single_type& values, bool doUpdate)
  {
    rte::ret_type ret = rte::ret_type::NOK;

    if (input_classifiers.classifiers.check_boundary(ucClassifierId))
    {
      util::memcpy(&input_classifiers.classifiers.at(ucClassifierId), &values, sizeof(input_classifier_single_type));
      if (doUpdate)
      {
        ret = updateClassifiers() ? rte::ret_type::OK : rte::ret_type::NOK;
      }
      else
      {
        ret = rte::ret_type::OK;
      }
    }

    return ret;
  }

  // -----------------------------------------------
  /// Server function: Store data in RAM. If doUpdate is true, stores data
  /// in EEPROM. Otherwise, call update() for this.
  /// Return OK is successful, returns NOK otherwise.
  // -----------------------------------------------
  rte::ret_type CalM::set_base_cv(const base_cv_cal_type& p, bool doUpdate)
  {
    rte::ret_type ret;

    base_cv = p;

    if (doUpdate)
    {
      ret = updateBaseCV() ? rte::ret_type::OK : rte::ret_type::NOK;
    }
    else
    {
      ret = rte::ret_type::OK;
    }
    
    return ret;
  }

  // -----------------------------------------------
  /// Server function: initialize EEPROM with ROM default values
  // -----------------------------------------------
  rte::ret_type CalM::init_all()
  {
    initAll();
    return rte::ret_type::OK;
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
          if (tgtit->type == target_type::eOnboard)
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
    if (!readAll())
    {
      // invalid / never programmed: initialize EEPROM with default values
      initAll();
    }

    calcLeds();
  }

  // -----------------------------------------------
  /// Cyclic runable
  // -----------------------------------------------
  void CalM::cycle100()
  {
  }

} // namespace cal
