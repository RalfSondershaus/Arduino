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
 * SPDX-License-Identifier: Apache-2.0
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

RTE_DEF_END
