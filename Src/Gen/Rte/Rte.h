/**
 * @file Rte.h
 *
 * @author Ralf Sondershaus
 *
 * @brief External interface of RTE. Provides start(), stop(), exec(), and setEvent().
 *        Provides project specific interfaces (ports) via include header files and objects.
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

#ifndef RTE_H_
#define RTE_H_

#include <Std_Types.h>

namespace rte
{
  typedef uint32 tEvntId;

  constexpr tEvntId kInvalidEventId = static_cast<tEvntId>(0xFFFFFFFFU);
}

#include <Rte/Rte_Cfg_Ext.h>

namespace rte
{
  void start();
  void stop();
  void exec();
  void setEvent(uint32 ulEventId);
} // namespace rte

#endif // RTE_H_
