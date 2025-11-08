/**
 * @file Rte_Cfg_Cod.h
 *
 * @author Ralf Sondershaus
 *
 * @brief RTE for calibration data of Signal.
 *
 * This file doesn't have include guards because it is included several times in a row.
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

#ifndef RTE_CFG_COD_H
#define RTE_CFG_COD_H

#include <Platform_Types.h>
#include <Cal/CalM.h>

namespace rte
{
    extern cal::CalM calm;

    static inline uint8 get_cv(uint16 cv)               { return calm.get_cv(cv); }
    static inline void set_cv(uint16 cv_id, uint8 val)  { return calm.set_cv(cv_id, val); }
    static inline bool is_cv_id_valid(uint16 cv_id)     { return calm.is_cv_id_valid(cv_id); }

    static inline bool ifc_cal_set_defaults()           { return calm.set_defaults(); }
}

#endif // RTE_CFG_COD_H