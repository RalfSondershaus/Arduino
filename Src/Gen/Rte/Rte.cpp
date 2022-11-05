/**
 * @file Rte.cpp
 *
 * @author Ralf Sondershaus
 *
 * @brief Defines the RTE from include header files.
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

#include <Util/Array.h>
#include <Util/bitset.h>
#include <Util/Timer.h>
#include <Rte/Rte.h>
#include <Rte/Rte_Type_Ifc.h>
#include <Rte/Rte_Type_Runable.h>

namespace rte
{
  /// Timer types
  typedef Util::MicroTimer::time_type   time_type;
  typedef Util::MicroTimer              timer_type;

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

#define RTE_DEF_MODE_INTERFACES
#include <Rte/Rte_Cfg_Mac.h>
#include <Rte/Rte_Cfg_Prj.h>
#undef RTE_DEF_MODE_INTERFACES

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
    auto it_rcb = aRcb.begin();
    auto it_rcb_cfg = aCyclicRunables.begin();

    while (it_rcb != aRcb.end())
    {
      (void)(it_rcb->timer.start(it_rcb_cfg->ulStartOffset));
      it_rcb++;
      it_rcb_cfg++;
    }

    aEvents.reset();

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
    if (ulEventId < kEvent_Max)
    { 
      aEvents.set(ulEventId, true);
    }
  }
} // namespace rte