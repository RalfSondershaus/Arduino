/**
 * @file Rte_Type.h
 * @author Ralf Sondershaus
 *
 * @brief Interface class Runable
 * 
 * @copyright Copyright 2018 - 2022 Ralf Sondershaus
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

#ifndef RTE_TYPE_H_
#define RTE_TYPE_H_

#include <Rte/Rte_Type_Ifc.h>
#include <Rte/Rte_Type_Runable.h>

#include <Util/Timer.h>

namespace Rte
{
  /// Timer types
  typedef Util::MicroTimer::time_type   time_type;
  typedef Util::MicroTimer              timer_type;
} // namespace Rte

#endif /* RTE_TYPE_H_ */
