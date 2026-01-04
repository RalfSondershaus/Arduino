/**
 * @file Rte_Cfg_Prj.h
 *
 * @author Ralf Sondershaus
 *
 * @brief RTE declaration for unit test of signal::LedRouter
 *
 * This file doesn't have include guards because it is included several times.
 *
 * @copyright Copyright 2022 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <InputClassifier.h>
#include <Cal/CalM.h>
#include <Com/ComR.h>

RTE_DEF_START

RTE_DEF_OBJ_START
RTE_DEF_OBJ(cal::CalM, calm)
RTE_DEF_OBJ(com::ComR, comr)
RTE_DEF_OBJ(signal::InputClassifier, input_classifier)
RTE_DEF_OBJ_END

RTE_DEF_INIT_RUNABLE_START
RTE_DEF_INIT_RUNABLE(cal::CalM, calm, init)
RTE_DEF_INIT_RUNABLE(com::ComR, comr, init)
RTE_DEF_INIT_RUNABLE_END

RTE_DEF_CYCLIC_RUNABLE_START
RTE_DEF_CYCLIC_RUNABLE(cal::CalM, calm, cycle100, 100, 0)
RTE_DEF_CYCLIC_RUNABLE(com::ComR, comr, cycle, 100, 10000)
RTE_DEF_CYCLIC_RUNABLE_END

RTE_DEF_PORT_SR_START
RTE_DEF_PORT_SR_END

RTE_DEF_PORT_CS_START
RTE_DEF_PORT_CS_END

RTE_DEF_END
