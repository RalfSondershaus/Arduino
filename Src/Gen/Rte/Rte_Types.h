/**
 * @file Gen/Rte/Rte_Types.h
 *
 * @brief Include generic types and project specific types (if available).
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
