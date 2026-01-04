/**
 * @file Prj/UnitTest/Gen/Rte/Ut_Rte/Rte/Rte_Types_Prj.h
 *
 * @brief Defines project specific types for the RTE.
 *
 * @copyright Copyright 2026 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef RTE_TYPE_PRJ_H_
#define RTE_TYPE_PRJ_H_

#include <Std_Types.h>
#include <Rte/Rte.h>

namespace rte
{
    /// SR interface
    using Ifc_Uint16 = rte::ifc_sr<uint16>;
} // namespace rte

#endif // RTE_TYPE_PRJ_H_
