/**
 * @file Rte_Cfg_Mac.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Helper file to provide #defines for configuration.
 *
 * Intentionally without header guards because this file is included several times in a row.
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

#ifndef RTE_DEF_START
#define RTE_DEF_START       namespace rte {
#endif

#ifndef RTE_DEF_END
#define RTE_DEF_END         } // namespace rte
#endif

#ifdef RTE_DEF_OBJ_START
#undef RTE_DEF_OBJ_START
#endif
#ifdef RTE_DEF_OBJ
#undef RTE_DEF_OBJ
#endif
#ifdef RTE_DEF_OBJ_END
#undef RTE_DEF_OBJ_END
#endif
#ifdef RTE_DEF_CYCLIC_RUNABLE
#undef RTE_DEF_CYCLIC_RUNABLE
#endif
#ifdef RTE_DEF_CYCLIC_RUNABLE_START
#undef RTE_DEF_CYCLIC_RUNABLE_START
#endif
#ifdef RTE_DEF_CYCLIC_RUNABLE_END
#undef RTE_DEF_CYCLIC_RUNABLE_END
#endif
#ifdef RTE_DEF_EVENT_RUNABLE
#undef RTE_DEF_EVENT_RUNABLE
#endif
#ifdef RTE_DEF_EVENT_RUNABLE_START
#undef RTE_DEF_EVENT_RUNABLE_START
#endif
#ifdef RTE_DEF_EVENT_RUNABLE_END
#undef RTE_DEF_EVENT_RUNABLE_END
#endif
#ifdef RTE_DEF_INTERFACE
#undef RTE_DEF_INTERFACE
#endif
#ifdef RTE_DEF_INTERFACE_START
#undef RTE_DEF_INTERFACE_START
#endif
#ifdef RTE_DEF_INTERFACE_END
#undef RTE_DEF_INTERFACE_END
#endif

// ----------------------------------------------------------------------
/// Class instances
// ----------------------------------------------------------------------
#ifdef RTE_DEF_MODE_OBJ
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)                                       cls obj;
#define RTE_DEF_CYCLIC_RUNABLE_START
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc)
#define RTE_DEF_CYCLIC_RUNABLE_END
#define RTE_DEF_EVENT_RUNABLE_START
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func)
#define RTE_DEF_EVENT_RUNABLE_END
#define RTE_DEF_INTERFACE(cls,obj)
#define RTE_DEF_INTERFACE_START
#define RTE_DEF_INTERFACE_END
#endif

#ifdef RTE_DEF_MODE_OBJ_EXT
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)                                       extern cls obj;
#define RTE_DEF_CYCLIC_RUNABLE_START
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc)
#define RTE_DEF_CYCLIC_RUNABLE_END
#define RTE_DEF_EVENT_RUNABLE_START
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func)
#define RTE_DEF_EVENT_RUNABLE_END
#define RTE_DEF_INTERFACE(cls,obj)
#define RTE_DEF_INTERFACE_START
#define RTE_DEF_INTERFACE_END
#endif

// ----------------------------------------------------------------------
/// Cyclic runables
// ----------------------------------------------------------------------
#ifdef RTE_DEF_MODE_CYCLIC_RUNABLES
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)
#define RTE_DEF_CYCLIC_RUNABLE_START
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc) TRunable<cls> rc_##obj##func(obj, &cls::func);
#define RTE_DEF_CYCLIC_RUNABLE_END
#define RTE_DEF_EVENT_RUNABLE_START
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func)
#define RTE_DEF_EVENT_RUNABLE_END
#define RTE_DEF_INTERFACE(cls,obj)
#define RTE_DEF_INTERFACE_START
#define RTE_DEF_INTERFACE_END
#endif

#ifdef RTE_DEF_MODE_CYCLIC_RUNABLE_ENUM
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)
#define RTE_DEF_CYCLIC_RUNABLE_START                                  enum {
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc)    kRC_##obj##func,
#define RTE_DEF_CYCLIC_RUNABLE_END                                    kRC_Max };
#define RTE_DEF_EVENT_RUNABLE_START
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func)
#define RTE_DEF_EVENT_RUNABLE_END
#define RTE_DEF_INTERFACE(cls,obj)
#define RTE_DEF_INTERFACE_START
#define RTE_DEF_INTERFACE_END
#endif

#ifdef RTE_DEF_MODE_CYCLIC_RUNABLE_ARRAY
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)
#define RTE_DEF_CYCLIC_RUNABLE_START                                  util::array<rcb_cfg_type, kRC_Max> aCyclicRunables = { {
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc)    { time_off, time_cyc, &rc_##obj##func },
#define RTE_DEF_CYCLIC_RUNABLE_END                                    } };
#define RTE_DEF_EVENT_RUNABLE_START
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func)
#define RTE_DEF_EVENT_RUNABLE_END
#define RTE_DEF_INTERFACE(cls,obj)
#define RTE_DEF_INTERFACE_START
#define RTE_DEF_INTERFACE_END
#endif

// ----------------------------------------------------------------------
/// Event runables
// ----------------------------------------------------------------------
#ifdef RTE_DEF_MODE_EVENT_RUNABLES
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)
#define RTE_DEF_CYCLIC_RUNABLE_START
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc)
#define RTE_DEF_CYCLIC_RUNABLE_END
#define RTE_DEF_EVENT_RUNABLE_START
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func) TRunable<cls> re_##obj##func(obj, &cls::func);
#define RTE_DEF_EVENT_RUNABLE_END
#define RTE_DEF_INTERFACE(cls,obj)
#define RTE_DEF_INTERFACE_START
#define RTE_DEF_INTERFACE_END
#endif

#ifdef RTE_DEF_MODE_EVENT_RUNABLE_ENUM
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)
#define RTE_DEF_CYCLIC_RUNABLE_START
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc)
#define RTE_DEF_CYCLIC_RUNABLE_END
#define RTE_DEF_EVENT_RUNABLE_START                       enum {
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func)  kEvent_##eventname,
#define RTE_DEF_EVENT_RUNABLE_END                         kEvent_Max };
#define RTE_DEF_INTERFACE(cls,obj)
#define RTE_DEF_INTERFACE_START
#define RTE_DEF_INTERFACE_END
#endif

#ifdef RTE_DEF_MODE_EVENT_RUNABLE_ARRAY
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)
#define RTE_DEF_CYCLIC_RUNABLE_START             
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc)
#define RTE_DEF_CYCLIC_RUNABLE_END
#define RTE_DEF_EVENT_RUNABLE_START                       util::array<Runable * const, kEvent_Max> aEventRunables = {
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func)  &re_##obj##func,
#define RTE_DEF_EVENT_RUNABLE_END                         };
#define RTE_DEF_INTERFACE(cls,obj)
#define RTE_DEF_INTERFACE_START
#define RTE_DEF_INTERFACE_END
#endif

// ----------------------------------------------------------------------
/// Interfaces (ports)
// ----------------------------------------------------------------------
#ifdef RTE_DEF_MODE_INTERFACES
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)
#define RTE_DEF_CYCLIC_RUNABLE_START
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc)
#define RTE_DEF_CYCLIC_RUNABLE_END
#define RTE_DEF_EVENT_RUNABLE_START
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func)
#define RTE_DEF_EVENT_RUNABLE_END
#define RTE_DEF_INTERFACE(cls,obj)                        cls obj;
#define RTE_DEF_INTERFACE_START
#define RTE_DEF_INTERFACE_END
#endif

#ifdef RTE_DEF_MODE_INTERFACES_EXT
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)
#define RTE_DEF_CYCLIC_RUNABLE_START
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc)
#define RTE_DEF_CYCLIC_RUNABLE_END
#define RTE_DEF_EVENT_RUNABLE_START
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func)
#define RTE_DEF_EVENT_RUNABLE_END
#define RTE_DEF_INTERFACE(cls,obj)                        extern cls obj;
#define RTE_DEF_INTERFACE_START
#define RTE_DEF_INTERFACE_END
#endif
