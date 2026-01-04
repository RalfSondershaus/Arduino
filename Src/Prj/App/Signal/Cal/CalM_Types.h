/**
 * @file Cal/CalM_Types.h
 *
 * @brief Project specific calibration types.
 *
 * @copyright Copyright 2022 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CALM_TYPES_H_
#define CALM_TYPES_H_

#include <Cal/CalM_Types_Prj.h>

/**
 * @brief For type safety: a CV
 */
struct CV
{
    uint16 id;
    uint8 val;
};

#endif // CALM_TYPES_H_
