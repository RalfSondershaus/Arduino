/**
 * @file Rte_Cfg_Ext.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Helper file to define external interface of RTE. Provides project specific interfaces (ports)
 *        and objects.
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

#ifndef RTE_CFG_EXT_H_
#define RTE_CFG_EXT_H_

#define RTE_DEF_MODE_OBJ_EXT
#include <Rte/Rte_Cfg_Mac.h>
#include <Rte/Rte_Cfg_Prj.h>
#undef RTE_DEF_MODE_OBJ_EXT

#define RTE_DEF_MODE_INTERFACES_EXT
#include <Rte/Rte_Cfg_Mac.h>
#include <Rte/Rte_Cfg_Prj.h>
#undef RTE_DEF_MODE_INTERFACES_EXT

#endif // RTE_CFG_EXT_H_
