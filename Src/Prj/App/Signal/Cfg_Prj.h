/**
 * @file Cfg_Prj.h
 *
 * @author Ralf Sondershaus
 *
 * @brief
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

#ifndef CFG_PRJ_H_
#define CFG_PRJ_H_

#include <Std_Types.h>

namespace cfg
{
    constexpr uint8 kNrSignals = 8; ///< Maximum number of signals
    /**
     * @brief Maximum number of signal aspects
     */
    constexpr uint8 kNrSignalAspects = 8;
    /**
     * @brief Number of DCC addresses used per signal (2 aspects per address: 1R and 1G)
     */
    constexpr uint8 kNrDccAddressesPerSignal = kNrSignalAspects / 2U;
    constexpr uint8 kNrSignalTargets = 8; ///< Maximum number of LED strings of a signal

    constexpr int kNrClassifiers = kNrSignals; ///< Number of (AD value) classifiers:
                                               ///< number of signals because each signal might have a classifier
    constexpr int kNrClassifierClasses = 5;    ///< Number of classifier classes

    constexpr uint8 kNrUserDefinedClassifierTypes = 2; ///< Number of user defineable classifier types (in EEPROM)
    constexpr uint8 kNrUserDefinedSignals = 2;         ///< Number of user defineable signal IDs (in EEPROM)
    constexpr uint8 kNrBuiltInSignals = 2;             ///< Number of built in signal IDs (in ROM)

    constexpr int kNrOnboardTargets = 54;  ///< We support up to 54 onboard LED targets (some of them with PWM))
    constexpr int kNrExternalTargets = 32; ///< We support up to 32 external outputs (such as 4 shift registers with 8 bit each)
    constexpr int kNrTargets = kNrOnboardTargets + kNrExternalTargets;
    /// Number of bits that are required to store numbers from 0 ... max(kNrOnboardTargets, kNrExternalTargets).
    /// If kCalTgtNrBits > 6, you need to modify the base type of cal::target_type (which is currently uint8).
    constexpr uint8 kCalTgtNrBits = 6U;
    constexpr uint8 kCalTgtNrBitsPinsPow2 = 64U;

    /// The decoder supports this number of addresses, first address is
    /// defined by DecoderAddressLSB and DecoderAddressMSB.
    /// Address range: [first_adress, first_adress + kNrAddresses].
    constexpr uint8 kNrAddresses = kNrSignals - 1;

} // namespace cfg

#endif // CFG_PRJ_H_
