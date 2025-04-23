/**
 * @file Rte_Cfg_Prj.h
 *
 * @author Ralf Sondershaus
 *
 * @brief RTE declaration for Signal.
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

#include <DccDecoder.h>
#include <InputClassifier.h>
#include <InputCommand.h>
#include <LedRouter.h>
#include <CddLed.h>
#include <Signal.h>
#include <Dcc/Decoder.h>
#include <Cal/CalM.h>
#include <Com/ComR.h>

RTE_DEF_START

RTE_DEF_OBJ_START
RTE_DEF_OBJ(cal::CalM, calm)
RTE_DEF_OBJ(com::ComR, comr)
RTE_DEF_OBJ(signal::DccDecoder, dcc_decoder)
RTE_DEF_OBJ(signal::InputClassifier, input_classifier)
RTE_DEF_OBJ(signal::LedRouter, led_router)
RTE_DEF_OBJ(signal::SignalHandler, signal_handler)
RTE_DEF_OBJ(signal::InputCommand, input_command)
RTE_DEF_OBJ(cdd::CddLed, cddled)
RTE_DEF_OBJ_END

RTE_DEF_INIT_RUNABLE_START
RTE_DEF_INIT_RUNABLE(cal::CalM, calm, init)
RTE_DEF_INIT_RUNABLE(com::ComR, comr, init)
RTE_DEF_INIT_RUNABLE(signal::DccDecoder, dcc_decoder, init)
RTE_DEF_INIT_RUNABLE(signal::InputClassifier, input_classifier, init)
RTE_DEF_INIT_RUNABLE(signal::SignalHandler, signal_handler, init)
RTE_DEF_INIT_RUNABLE(signal::LedRouter, led_router, init)
RTE_DEF_INIT_RUNABLE(cdd::CddLed, cddled, init)
RTE_DEF_INIT_RUNABLE_END

RTE_DEF_CYCLIC_RUNABLE_START
RTE_DEF_CYCLIC_RUNABLE(signal::InputClassifier, input_classifier, cycle   , 0  , 10000)
RTE_DEF_CYCLIC_RUNABLE(signal::DccDecoder     , dcc_decoder     , cycle   , 100, 10000)
RTE_DEF_CYCLIC_RUNABLE(signal::SignalHandler  , signal_handler  , cycle   , 200, 10000)
RTE_DEF_CYCLIC_RUNABLE(signal::LedRouter      , led_router      , cycle   , 300, 10000)
RTE_DEF_CYCLIC_RUNABLE(cdd::CddLed            , cddled          , cycle   , 400, 10000)
RTE_DEF_CYCLIC_RUNABLE(cal::CalM              , calm            , cycle100, 500, 100000)
RTE_DEF_CYCLIC_RUNABLE(com::ComR              , comr            , cycle   , 100, 10000)
RTE_DEF_CYCLIC_RUNABLE_END

RTE_DEF_PORT_SR_START
RTE_DEF_PORT_SR(rte::Ifc_Cal_DccAddress, ifc_cal_dcc_address)
RTE_DEF_PORT_SR_CONTAINER(rte::Ifc_ClassifiedValues, ifc_classified_values)
RTE_DEF_PORT_SR_CONTAINER(rte::Ifc_ADValues, ifc_ad_values)
RTE_DEF_PORT_SR_CONTAINER(rte::Ifc_DccCommands, ifc_dcc_commands)
RTE_DEF_PORT_SR_CONTAINER(rte::Ifc_OnboardTargetDutyCycles, ifc_onboard_target_duty_cycles)
RTE_DEF_PORT_SR_CONTAINER(rte::Ifc_ExternalTargetDutyCycles, ifc_external_target_duty_cycles)
RTE_DEF_PORT_SR_END

RTE_DEF_PORT_CS_START
RTE_DEF_PORT_CS(Ifc_Cal_Signal             , ifc_cal_signal              , calm, &cal::CalM::get_signal)
RTE_DEF_PORT_CS(Ifc_Cal_InputClassifier    , ifc_cal_input_classifier    , calm, &cal::CalM::get_input_classifiers)
RTE_DEF_PORT_CS(Ifc_Cal_Led                , ifc_cal_leds                , calm, &cal::CalM::get_leds)
RTE_DEF_PORT_CS(Ifc_Cal_Base_CV            , ifc_cal_base_cv             , calm, &cal::CalM::get_base_cv)
RTE_DEF_PORT_CS(Ifc_Cal_Set_Signal         , ifc_cal_set_signal          , calm, &cal::CalM::set_signal)
RTE_DEF_PORT_CS(Ifc_Cal_Set_InputClassifier, ifc_cal_set_input_classifier, calm, &cal::CalM::set_input_classifier)
RTE_DEF_PORT_CS(Ifc_Rte_GetCommand             , ifc_rte_get_cmd                , input_command, &signal::InputCommand::getCmd)
RTE_DEF_PORT_CS(Ifc_Rte_LedSetIntensityAndSpeed, ifc_rte_set_intensity_and_speed, led_router, &signal::LedRouter::setIntensityAndSpeed)
RTE_DEF_PORT_CS(Ifc_Rte_LedSetIntensity        , ifc_rte_set_intensity          , led_router, &signal::LedRouter::setIntensity)
RTE_DEF_PORT_CS(Ifc_Rte_LedSetSpeed            , ifc_rte_set_speed              , led_router, &signal::LedRouter::setSpeed)

RTE_DEF_PORT_CS_END

RTE_DEF_END
