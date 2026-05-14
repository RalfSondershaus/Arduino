/**
 * @file Rte_Cfg_Prj.h
 * 
 * @brief RTE (Runtime Environment) configuration for the BlinkSample project.
 *
 * This header file defines the runtime environment setup for the BlinkSample application,
 * including object instantiation, initialization runnables, and cyclic tasks.
 *
 * @details
 * - Declares a Blinker object instance 'b'
 * - Defines an initialization runnable that calls Blinker::init()
 * - Defines a cyclic runnable that calls Blinker::run() with a period of 1000ms
 *
 * @note This file intentionally contains no include guards as it is meant to be
 *       included multiple times with different macro definitions for RTE processing.
 *
 * @see Blinker.h for the Blinker class definition
 * 
 * @author Ralf Sondershaus
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
