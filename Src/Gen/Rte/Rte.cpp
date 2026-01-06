/**
 * @file Rte.cpp
 *
 * @author Ralf Sondershaus
 *
 * @brief Defines the RTE from include header files.
 *
 * @copyright Copyright 2022 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Util/Array.h>
#include <Util/bitset.h>
#include <Util/String_view.h>
#include <Util/Timer.h>
#include <Rte/Rte.h>
#include <Rte/Rte_Types_Ifc.h>
#include <Rte/Rte_Types_Runable.h>

namespace rte
{
  /// Timer types
  typedef util::MicroTimer::time_type   time_type;
  typedef util::MicroTimer              timer_type;

  typedef rte::Runable * const          runable_const_pointer;

  // To initialize RCBs from constructor parameters
  typedef struct
  {
    time_type             ulStartOffset;      ///< [us] Offset time at start up
    time_type             ulCycleTime;        ///< [us] Cycle time
    runable_const_pointer pRnbl;               ///< pointer to the runable
  } rcb_cfg_type;
} // namespace rte

#define RTE_DEF_MODE_OBJ
#include <Rte/Rte_Cfg_Mac.h>
#include <Rte/Rte_Cfg_Prj.h>
#undef RTE_DEF_MODE_OBJ

#define RTE_DEF_MODE_INIT_RUNABLES
#include <Rte/Rte_Cfg_Mac.h>
#include <Rte/Rte_Cfg_Prj.h>
#undef RTE_DEF_MODE_INIT_RUNABLES

#define RTE_DEF_MODE_INIT_RUNABLE_ENUM
#include <Rte/Rte_Cfg_Mac.h>
#include <Rte/Rte_Cfg_Prj.h>
#undef RTE_DEF_MODE_INIT_RUNABLE_ENUM

#define RTE_DEF_MODE_INIT_RUNABLE_ARRAY
#include <Rte/Rte_Cfg_Mac.h>
#include <Rte/Rte_Cfg_Prj.h>
#undef RTE_DEF_MODE_INIT_RUNABLE_ARRAY

#define RTE_DEF_MODE_CYCLIC_RUNABLES
#include <Rte/Rte_Cfg_Mac.h>
#include <Rte/Rte_Cfg_Prj.h>
#undef RTE_DEF_MODE_CYCLIC_RUNABLES

#define RTE_DEF_MODE_CYCLIC_RUNABLE_ENUM
#include <Rte/Rte_Cfg_Mac.h>
#include <Rte/Rte_Cfg_Prj.h>
#undef RTE_DEF_MODE_CYCLIC_RUNABLE_ENUM

#define RTE_DEF_MODE_CYCLIC_RUNABLE_ARRAY
#include <Rte/Rte_Cfg_Mac.h>
#include <Rte/Rte_Cfg_Prj.h>
#undef RTE_DEF_MODE_CYCLIC_RUNABLE_ARRAY

#define RTE_DEF_MODE_EVENT_RUNABLES
#include <Rte/Rte_Cfg_Mac.h>
#include <Rte/Rte_Cfg_Prj.h>
#undef RTE_DEF_MODE_EVENT_RUNABLES

#define RTE_DEF_MODE_EVENT_RUNABLE_ENUM
#include <Rte/Rte_Cfg_Mac.h>
#include <Rte/Rte_Cfg_Prj.h>
#undef RTE_DEF_MODE_EVENT_RUNABLE_ENUM

#define RTE_DEF_MODE_EVENT_RUNABLE_ARRAY
#include <Rte/Rte_Cfg_Mac.h>
#include <Rte/Rte_Cfg_Prj.h>
#undef RTE_DEF_MODE_EVENT_RUNABLE_ARRAY

#define RTE_DEF_MODE_PORT
#include <Rte/Rte_Cfg_Mac.h>
#include <Rte/Rte_Cfg_Prj.h>
#undef RTE_DEF_MODE_PORT

#ifdef RTE_CFG_PORT_SR_AVAILABLE
// Just in case this feature is active, define the port array
#define RTE_DEF_MODE_PORT_ARRAY
#include <Rte/Rte_Cfg_Mac.h>
#include <Rte/Rte_Cfg_Prj.h>
#undef RTE_DEF_MODE_PORT_ARRAY
#endif

namespace rte
{
  // The Runable Control Block (RCB)
  typedef struct
  {
    timer_type timer; ///< The timer for the next call
  } rcb_type;

  // RCB array type
  typedef util::array<rcb_type, kRC_Max> rcb_array;

  // 32 events
  typedef util::bitset<uint32, 32> event_array;


  /// The RCBs
  rcb_array aRcb;
  /// Events
  event_array aEvents;

  //rcb_array::iterator it_rcb_cur;

  void start()
  {
    // load timers
    auto it_rcb = aRcb.begin();
    auto it_rcb_cfg = aCyclicRunables.begin();

    while (it_rcb != aRcb.end())
    {
      (void)(it_rcb->timer.start(it_rcb_cfg->ulStartOffset));
      it_rcb++;
      it_rcb_cfg++;
    }

    // clear events
    aEvents.reset();

    // call init runables
    auto it_init_cfg = aInitRunables.begin();
    while (it_init_cfg != aInitRunables.end())
    {
      (*it_init_cfg)->run();
      it_init_cfg++;
    }

    // restart round robin scheduling
    //it_rcb_cur = aRcb.begin();
  }

  void stop()
  {}

  void exec()
  {
    auto it_rcb = aRcb.begin();
    auto it_rcb_cfg = aCyclicRunables.begin();
    while (it_rcb != aRcb.end())
    {
      if (it_rcb->timer.timeout())
      {
        it_rcb_cfg->pRnbl->run();
        (void)(it_rcb->timer.increment(it_rcb_cfg->ulCycleTime));
      }
      it_rcb++;
      it_rcb_cfg++;
    }
  }

  void setEvent(uint32 ulEventId)
  {
    (void)ulEventId;
    //if (ulEventId < kEvent_Max)
    //{ 
    //  aEvents.set(ulEventId, true);
    //}
  }

  size_t getNrPorts()
  {
  #ifdef RTE_CFG_PORT_SR_AVAILABLE
    return sizeof(aPorts) / sizeof(port_data_t);
  #else
    return 0;
  #endif
  }

  port_data_t * getPortData(size_t idx)
  {
    port_data_t * pRet;
  #ifdef RTE_CFG_PORT_SR_AVAILABLE
    if (idx < getNrPorts())
    {
      pRet = &aPorts[idx];
    }
    else
  #else
    (void) idx;
  #endif
    {
      pRet = nullptr;
    }
    return pRet;
  }

  port_data_t * getPortData(const char * portName)
  {
    port_data_t * p = nullptr;
  #ifdef RTE_CFG_PORT_SR_AVAILABLE
    size_t idx;
    for (idx = 0; idx < getNrPorts(); idx++)
    {
      if (util::string_view{aPorts[idx].szName}.compare(portName) == 0)
      {
        p = &aPorts[idx];
        break;
      }
    }
  #else
    (void) portName;
  #endif
    return p;
  }


} // namespace rte