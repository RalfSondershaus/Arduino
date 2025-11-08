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
RTE_DEF_PORT_CS(Ifc_Cal_Signal             , ifc_cal_signal              , calm, &cal::CalM::get_signal)
RTE_DEF_PORT_CS(Ifc_Cal_Set_CV             , ifc_cal_set_cv              , calm, &cal::CalM::set_cv)
RTE_DEF_PORT_CS(Ifc_Cal_Get_CV             , ifc_cal_get_cv              , calm, &cal::CalM::get_cv)
RTE_DEF_PORT_CS(Ifc_Cal_Set_Defaults       , ifc_cal_set_defaults        , calm, &cal::CalM::set_defaults)
//RTE_DEF_PORT_CS(Ifc_Rte_UpdateConfigForClassifier, ifc_rte_update_config_for_classifier, input_classifier, &signal::InputClassifier::set_config)
RTE_DEF_PORT_CS_END

RTE_DEF_END
