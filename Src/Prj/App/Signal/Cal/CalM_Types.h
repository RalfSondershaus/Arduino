/**
 * @file Cal/CalM_Types.h
 *
 * @brief Project specific calibration types.
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

#ifndef CALM_TYPES_H_
#define CALM_TYPES_H_

#include <Cal/CalM_Types_Prj.h>

/**
 * @brief For type safety: a CV
 */
struct CV
{
    uint16 id;
    uint8 val;
};

#endif // CALM_TYPES_H_
