/**
 * @file Gen/Rte/Rte_Types.h
 *
 * @brief Include generic types and project specific types (if available).
 *
 * @copyright Copyright 2022 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef RTE_TYPES_H_
#define RTE_TYPES_H_

/// Call a member function of an object (instance of a class).
/// We use this macro because AVR GCC doesn't seem to support std::invoke.
#define CALL_MEMBER_FUNC(obj,ptrToMemberFunc)  ((obj).*(ptrToMemberFunc))

#include <Rte/Rte_Types_Ifc.h>
#include <Rte/Rte_Types_Runable.h>
#include <Rte/Rte_Types_Gen.h>
#include <Rte/Rte_Types_Prj.h>

#endif // RTE_TYPES_H_
