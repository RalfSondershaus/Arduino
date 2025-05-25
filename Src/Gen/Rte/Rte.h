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
#include <Rte/Rte_Type.h>

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

  // These functions return non-zero values if the RTE is configured
  // with RTE_CFG_PORT_SR_AVAILABLE, e.g. add to Rte_cfg_Prj.h:
  //   #ifndef RTE_CFG_PORT_SR_AVAILABLE
  //   #define RTE_CFG_PORT_SR_AVAILABLE
  //   #endif

  size_t getNrPorts();
  port_data_t * getPortData(size_t idx);
  port_data_t * getPortData(const char * portName);
} // namespace rte

#endif // RTE_H_
