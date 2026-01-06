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
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef RTE_H_
#define RTE_H_

#include <Std_Types.h>
#include <Rte/Rte_Types.h>

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
