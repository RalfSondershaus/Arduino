/**
 * @file Rte_Cfg_Prj.h
 *
 * @author Ralf Sondershaus
 *
 * @brief RTE declaration for BlinkSample2.
 *
 * This file has no include guards.
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

#include <Blinker.h>

RTE_DEF_START

RTE_DEF_OBJ_START
RTE_DEF_OBJ(Blinker, b)
RTE_DEF_OBJ_END

RTE_DEF_INIT_RUNABLE_START
RTE_DEF_INIT_RUNABLE(Blinker, b, init)
RTE_DEF_INIT_RUNABLE_END

RTE_DEF_CYCLIC_RUNABLE_START
RTE_DEF_CYCLIC_RUNABLE(Blinker, b, run, 0, 1000)
RTE_DEF_CYCLIC_RUNABLE_END

RTE_DEF_EVENT_RUNABLE_START
RTE_DEF_INTERFACE(Ifc_Blinker, ifcb)
RTE_DEF_EVENT_RUNABLE_END

RTE_DEF_END
