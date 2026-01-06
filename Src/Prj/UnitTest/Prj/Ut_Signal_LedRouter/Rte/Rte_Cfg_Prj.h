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

#include <LedRouter.h>

RTE_DEF_START

RTE_DEF_OBJ_START
RTE_DEF_OBJ(signal::LedRouter, led_router)
RTE_DEF_OBJ_END

RTE_DEF_INIT_RUNABLE_START
RTE_DEF_INIT_RUNABLE(signal::LedRouter, led_router, init)
RTE_DEF_INIT_RUNABLE_END

RTE_DEF_CYCLIC_RUNABLE_START
RTE_DEF_CYCLIC_RUNABLE(signal::LedRouter, led_router, cycle, 200, 10000)
RTE_DEF_CYCLIC_RUNABLE_END

RTE_DEF_PORT_SR_START
RTE_DEF_PORT_SR_CONTAINER(rte::Ifc_OnboardTargetDutyCycles, ifc_onboard_target_duty_cycles)
RTE_DEF_PORT_SR_CONTAINER(rte::Ifc_ExternalTargetDutyCycles, ifc_external_target_duty_cycles)
RTE_DEF_PORT_SR_END

RTE_DEF_PORT_CS_START
RTE_DEF_PORT_CS(Ifc_Rte_LedSetIntensityAndSpeed, ifc_rte_set_intensity_and_speed, led_router, &signal::LedRouter::setIntensityAndSpeed)
RTE_DEF_PORT_CS(Ifc_Rte_LedSetIntensity        , ifc_rte_set_intensity          , led_router, &signal::LedRouter::setIntensity)
RTE_DEF_PORT_CS(Ifc_Rte_LedSetSpeed            , ifc_rte_set_speed              , led_router, &signal::LedRouter::setSpeed)
RTE_DEF_PORT_CS_END

RTE_DEF_END
