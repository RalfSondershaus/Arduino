/**
 * @file Rte_Type.h
 * @author Ralf Sondershaus
 *
 * @brief RTE declaration for RTE unit test.
 *
 * This file has no include guards.
 *
 * @copyright Copyright 2018 - 2022 Ralf Sondershaus
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

#include <A.h>
#include <B.h>
#include <Ifc_A.h>

RTE_DEF_START

RTE_DEF_OBJ_START
RTE_DEF_OBJ(A, a1)
RTE_DEF_OBJ(B, b1)
RTE_DEF_OBJ_END

RTE_DEF_INIT_RUNABLE_START
RTE_DEF_INIT_RUNABLE(A, a1, init)
RTE_DEF_INIT_RUNABLE_END

RTE_DEF_CYCLIC_RUNABLE_START
RTE_DEF_CYCLIC_RUNABLE(A, a1, func,    0, 10000)
RTE_DEF_CYCLIC_RUNABLE(B, b1, func, 1000, 20000)
RTE_DEF_CYCLIC_RUNABLE_END

RTE_DEF_EVENT_RUNABLE_START
RTE_DEF_EVENT_RUNABLE(Ifc_A_Write, B, b1, evnt)
RTE_DEF_EVENT_RUNABLE_END

RTE_DEF_PORT_SR_START
RTE_DEF_PORT_SR(Ifc_A, ifca)
RTE_DEF_PORT_SR_END

RTE_DEF_END
