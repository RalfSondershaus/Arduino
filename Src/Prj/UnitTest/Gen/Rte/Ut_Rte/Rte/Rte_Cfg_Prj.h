/**
 * @file Rte_Cfg_Prj.h
 * 
 * @author Ralf Sondershaus
 *
 * @brief RTE declaration for RTE unit test.
 *
 * This file doesn't have include guards because it is included several times in a row.
 *
 * @copyright Copyright 2018 - 2022 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <A.h>
#include <B.h>

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

RTE_DEF_PORT_SR_START
RTE_DEF_PORT_SR(rte::Ifc_Uint16, ifc_uint16)
RTE_DEF_PORT_SR_END

RTE_DEF_END
