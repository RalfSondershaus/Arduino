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
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef RTE_CFG_EXT_H_
#define RTE_CFG_EXT_H_

#define RTE_DEF_MODE_OBJ_EXT
#include <Rte/Rte_Cfg_Mac.h>
#include <Rte/Rte_Cfg_Prj.h>
#undef RTE_DEF_MODE_OBJ_EXT

#define RTE_DEF_MODE_PORT_EXT
#include <Rte/Rte_Cfg_Mac.h>
#include <Rte/Rte_Cfg_Prj.h>
#undef RTE_DEF_MODE_PORT_EXT

#endif // RTE_CFG_EXT_H_
