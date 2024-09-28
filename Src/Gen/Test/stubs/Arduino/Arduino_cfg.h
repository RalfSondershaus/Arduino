/**
 * @file Arduino_cfg.h
 * @author Ralf Sondershaus
 *
 * @brief Defines compiler defines to configure the stubbing
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

#ifndef STUB_ARDUINO_CFG_H_
#define STUB_ARDUINO_CFG_H_

#include <Arduino_cfg_type.h>
#include <Arduino_cfg_prj.h>

#ifndef CFG_STUB_MILLIS
/**
 * Default: Stub millis() with stubs::millis
 */
#define CFG_STUB_MILLIS     CFG_STUB_ON
#endif

#ifndef CFG_STUB_MICROS
/**
 * Default: Stub micros() with stubs::micros.
 */
#define CFG_STUB_MICROS     CFG_STUB_ON
#endif

#endif // STUB_ARDUINO_CFG_H_
