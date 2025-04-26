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
#ifdef RTE_DEF_INIT_RUNABLE
#undef RTE_DEF_INIT_RUNABLE
#endif
#ifdef RTE_DEF_INIT_RUNABLE_START
#undef RTE_DEF_INIT_RUNABLE_START
#endif
#ifdef RTE_DEF_INIT_RUNABLE_END
#undef RTE_DEF_INIT_RUNABLE_END
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
#ifdef RTE_DEF_PORT_SR
#undef RTE_DEF_PORT_SR
#endif
#ifdef RTE_DEF_PORT_SR_CONTAINER
#undef RTE_DEF_PORT_SR_CONTAINER
#endif
#ifdef RTE_DEF_PORT_SR_START
#undef RTE_DEF_PORT_SR_START
#endif
#ifdef RTE_DEF_PORT_SR_END
#undef RTE_DEF_PORT_SR_END
#endif
#ifdef RTE_DEF_PORT_CS_END
#undef RTE_DEF_PORT_CS_END
#endif
#ifdef RTE_DEF_PORT_CS
#undef RTE_DEF_PORT_CS
#endif
#ifdef RTE_DEF_PORT_CS_START
#undef RTE_DEF_PORT_CS_START
#endif
#ifdef RTE_DEF_PORT_CS_END
#undef RTE_DEF_PORT_CS_END
#endif

// ----------------------------------------------------------------------
/// Class instances
// ----------------------------------------------------------------------
#ifdef RTE_DEF_MODE_OBJ
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)                                       cls obj;
#define RTE_DEF_INIT_RUNABLE_START
#define RTE_DEF_INIT_RUNABLE(cls, obj, func)
#define RTE_DEF_INIT_RUNABLE_END
#define RTE_DEF_CYCLIC_RUNABLE_START
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc)
#define RTE_DEF_CYCLIC_RUNABLE_END
#define RTE_DEF_EVENT_RUNABLE_START
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func)
#define RTE_DEF_EVENT_RUNABLE_END
#define RTE_DEF_PORT_SR(cls,port)
#define RTE_DEF_PORT_SR_CONTAINER(cls,port)
#define RTE_DEF_PORT_SR_START
#define RTE_DEF_PORT_SR_END
#define RTE_DEF_PORT_CS(cls,port,srvobj,func)
#define RTE_DEF_PORT_CS_START
#define RTE_DEF_PORT_CS_END
#endif

#ifdef RTE_DEF_MODE_OBJ_EXT
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)                                       extern cls obj;
#define RTE_DEF_INIT_RUNABLE_START
#define RTE_DEF_INIT_RUNABLE(cls, obj, func)
#define RTE_DEF_INIT_RUNABLE_END
#define RTE_DEF_CYCLIC_RUNABLE_START
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc)
#define RTE_DEF_CYCLIC_RUNABLE_END
#define RTE_DEF_EVENT_RUNABLE_START
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func)
#define RTE_DEF_EVENT_RUNABLE_END
#define RTE_DEF_PORT_SR(cls,port)
#define RTE_DEF_PORT_SR_CONTAINER(cls,port)
#define RTE_DEF_PORT_SR_START
#define RTE_DEF_PORT_SR_END
#define RTE_DEF_PORT_CS(cls,port,srvobj,func)
#define RTE_DEF_PORT_CS_START
#define RTE_DEF_PORT_CS_END
#endif

// ----------------------------------------------------------------------
/// Init runables
// ----------------------------------------------------------------------
#ifdef RTE_DEF_MODE_INIT_RUNABLES
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)
#define RTE_DEF_INIT_RUNABLE_START
#define RTE_DEF_INIT_RUNABLE(cls, obj, func)                        TRunable<cls> ri_##obj##func(obj, &cls::func);
#define RTE_DEF_INIT_RUNABLE_END
#define RTE_DEF_CYCLIC_RUNABLE_START
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc)
#define RTE_DEF_CYCLIC_RUNABLE_END
#define RTE_DEF_EVENT_RUNABLE_START
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func)
#define RTE_DEF_EVENT_RUNABLE_END
#define RTE_DEF_PORT_SR(cls,port)
#define RTE_DEF_PORT_SR_CONTAINER(cls,port)
#define RTE_DEF_PORT_SR_START
#define RTE_DEF_PORT_SR_END
#define RTE_DEF_PORT_CS(cls,port,srvobj,func)
#define RTE_DEF_PORT_CS_START
#define RTE_DEF_PORT_CS_END
#endif

#ifdef RTE_DEF_MODE_INIT_RUNABLE_ENUM
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)
#define RTE_DEF_INIT_RUNABLE_START                                    enum {
#define RTE_DEF_INIT_RUNABLE(cls, obj, func)                          kRI_##obj##func,
#define RTE_DEF_INIT_RUNABLE_END                                      kRI_Max };
#define RTE_DEF_CYCLIC_RUNABLE_START                                  
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc)
#define RTE_DEF_CYCLIC_RUNABLE_END
#define RTE_DEF_EVENT_RUNABLE_START
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func)
#define RTE_DEF_EVENT_RUNABLE_END
#define RTE_DEF_PORT_SR(cls,port)
#define RTE_DEF_PORT_SR_CONTAINER(cls,port)
#define RTE_DEF_PORT_SR_START
#define RTE_DEF_PORT_SR_END
#define RTE_DEF_PORT_CS(cls,port,srvobj,func)
#define RTE_DEF_PORT_CS_START
#define RTE_DEF_PORT_CS_END
#endif

#ifdef RTE_DEF_MODE_INIT_RUNABLE_ARRAY
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)
#define RTE_DEF_INIT_RUNABLE_START                                  util::array<runable_const_pointer, kRI_Max> aInitRunables = {
#define RTE_DEF_INIT_RUNABLE(cls, obj, func)                        &ri_##obj##func,
#define RTE_DEF_INIT_RUNABLE_END                                    };
#define RTE_DEF_CYCLIC_RUNABLE_START
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc)
#define RTE_DEF_CYCLIC_RUNABLE_END
#define RTE_DEF_EVENT_RUNABLE_START
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func)
#define RTE_DEF_EVENT_RUNABLE_END
#define RTE_DEF_PORT_SR(cls,port)
#define RTE_DEF_PORT_SR_CONTAINER(cls,port)
#define RTE_DEF_PORT_SR_START
#define RTE_DEF_PORT_SR_END
#define RTE_DEF_PORT_CS(cls,port,srvobj,func)
#define RTE_DEF_PORT_CS_START
#define RTE_DEF_PORT_CS_END
#endif

// ----------------------------------------------------------------------
/// Cyclic runables
// ----------------------------------------------------------------------
#ifdef RTE_DEF_MODE_CYCLIC_RUNABLES
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)
#define RTE_DEF_INIT_RUNABLE_START
#define RTE_DEF_INIT_RUNABLE(cls, obj, func)
#define RTE_DEF_INIT_RUNABLE_END
#define RTE_DEF_CYCLIC_RUNABLE_START
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc) TRunable<cls> rc_##obj##func(obj, &cls::func);
#define RTE_DEF_CYCLIC_RUNABLE_END
#define RTE_DEF_EVENT_RUNABLE_START
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func)
#define RTE_DEF_EVENT_RUNABLE_END
#define RTE_DEF_PORT_SR(cls,port)
#define RTE_DEF_PORT_SR_CONTAINER(cls,port)
#define RTE_DEF_PORT_SR_START
#define RTE_DEF_PORT_SR_END
#define RTE_DEF_PORT_CS(cls,port,srvobj,func)
#define RTE_DEF_PORT_CS_START
#define RTE_DEF_PORT_CS_END
#endif

#ifdef RTE_DEF_MODE_CYCLIC_RUNABLE_ENUM
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)
#define RTE_DEF_INIT_RUNABLE_START
#define RTE_DEF_INIT_RUNABLE(cls, obj, func)
#define RTE_DEF_INIT_RUNABLE_END
#define RTE_DEF_CYCLIC_RUNABLE_START                                  enum {
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc)    kRC_##obj##func,
#define RTE_DEF_CYCLIC_RUNABLE_END                                    kRC_Max };
#define RTE_DEF_EVENT_RUNABLE_START
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func)
#define RTE_DEF_EVENT_RUNABLE_END
#define RTE_DEF_PORT_SR(cls,port)
#define RTE_DEF_PORT_SR_CONTAINER(cls,port)
#define RTE_DEF_PORT_SR_START
#define RTE_DEF_PORT_SR_END
#define RTE_DEF_PORT_CS(cls,port,srvobj,func)
#define RTE_DEF_PORT_CS_START
#define RTE_DEF_PORT_CS_END
#endif

#ifdef RTE_DEF_MODE_CYCLIC_RUNABLE_ARRAY
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)
#define RTE_DEF_INIT_RUNABLE_START
#define RTE_DEF_INIT_RUNABLE(cls, obj, func)
#define RTE_DEF_INIT_RUNABLE_END
#define RTE_DEF_CYCLIC_RUNABLE_START                                  util::array<rcb_cfg_type, kRC_Max> aCyclicRunables = { {
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc)    { time_off, time_cyc, &rc_##obj##func },
#define RTE_DEF_CYCLIC_RUNABLE_END                                    } };
#define RTE_DEF_EVENT_RUNABLE_START
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func)
#define RTE_DEF_EVENT_RUNABLE_END
#define RTE_DEF_PORT_SR(cls,port)
#define RTE_DEF_PORT_SR_CONTAINER(cls,port)
#define RTE_DEF_PORT_SR_START
#define RTE_DEF_PORT_SR_END
#define RTE_DEF_PORT_CS(cls,port,srvobj,func)
#define RTE_DEF_PORT_CS_START
#define RTE_DEF_PORT_CS_END
#endif

// ----------------------------------------------------------------------
/// Event runables
// ----------------------------------------------------------------------
#ifdef RTE_DEF_MODE_EVENT_RUNABLES
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)
#define RTE_DEF_INIT_RUNABLE_START
#define RTE_DEF_INIT_RUNABLE(cls, obj, func)
#define RTE_DEF_INIT_RUNABLE_END
#define RTE_DEF_CYCLIC_RUNABLE_START
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc)
#define RTE_DEF_CYCLIC_RUNABLE_END
#define RTE_DEF_EVENT_RUNABLE_START
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func) TRunable<cls> re_##obj##func(obj, &cls::func);
#define RTE_DEF_EVENT_RUNABLE_END
#define RTE_DEF_PORT_SR(cls,port)
#define RTE_DEF_PORT_SR_CONTAINER(cls,port)
#define RTE_DEF_PORT_SR_START
#define RTE_DEF_PORT_SR_END
#define RTE_DEF_PORT_CS(cls,port,srvobj,func)
#define RTE_DEF_PORT_CS_START
#define RTE_DEF_PORT_CS_END
#endif

#ifdef RTE_DEF_MODE_EVENT_RUNABLE_ENUM
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)
#define RTE_DEF_INIT_RUNABLE_START
#define RTE_DEF_INIT_RUNABLE(cls, obj, func)
#define RTE_DEF_INIT_RUNABLE_END
#define RTE_DEF_CYCLIC_RUNABLE_START
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc)
#define RTE_DEF_CYCLIC_RUNABLE_END
#define RTE_DEF_EVENT_RUNABLE_START                       enum {
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func)  kEvent_##eventname,
#define RTE_DEF_EVENT_RUNABLE_END                         kEvent_Max };
#define RTE_DEF_PORT_SR(cls,port)
#define RTE_DEF_PORT_SR_CONTAINER(cls,port)
#define RTE_DEF_PORT_SR_START
#define RTE_DEF_PORT_SR_END
#define RTE_DEF_PORT_CS(cls,port,srvobj,func)
#define RTE_DEF_PORT_CS_START
#define RTE_DEF_PORT_CS_END
#endif

#ifdef RTE_DEF_MODE_EVENT_RUNABLE_ARRAY
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)
#define RTE_DEF_INIT_RUNABLE_START
#define RTE_DEF_INIT_RUNABLE(cls, obj, func)
#define RTE_DEF_INIT_RUNABLE_END
#define RTE_DEF_CYCLIC_RUNABLE_START             
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc)
#define RTE_DEF_CYCLIC_RUNABLE_END
#define RTE_DEF_EVENT_RUNABLE_START                       util::array<Runable * const, kEvent_Max> aEventRunables = {
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func)  &re_##obj##func,
#define RTE_DEF_EVENT_RUNABLE_END                         };
#define RTE_DEF_PORT_SR(cls,port)
#define RTE_DEF_PORT_SR_CONTAINER(cls,port)
#define RTE_DEF_PORT_SR_START
#define RTE_DEF_PORT_SR_END
#define RTE_DEF_PORT_CS(cls,port,srvobj,func)
#define RTE_DEF_PORT_CS_START
#define RTE_DEF_PORT_CS_END
#endif

// ----------------------------------------------------------------------
/// Interfaces (ports)
// ----------------------------------------------------------------------
#ifdef RTE_DEF_MODE_PORT
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)
#define RTE_DEF_INIT_RUNABLE_START
#define RTE_DEF_INIT_RUNABLE(cls, obj, func)
#define RTE_DEF_INIT_RUNABLE_END
#define RTE_DEF_CYCLIC_RUNABLE_START
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc)
#define RTE_DEF_CYCLIC_RUNABLE_END
#define RTE_DEF_EVENT_RUNABLE_START
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func)
#define RTE_DEF_EVENT_RUNABLE_END
#define RTE_DEF_PORT_SR(cls,port) \
  namespace port \
  { \
    cls obj; \
  }
#define RTE_DEF_PORT_SR_CONTAINER(cls,port) \
  namespace port \
  { \
    cls obj; \
  }
#define RTE_DEF_PORT_SR_START
#define RTE_DEF_PORT_SR_END
#define RTE_DEF_PORT_CS(cls,port,srvobj,func) \
  namespace port \
  { \
     cls obj(srvobj,func); \
  }
#define RTE_DEF_PORT_CS_START
#define RTE_DEF_PORT_CS_END
#endif

#ifdef RTE_DEF_MODE_PORT_EXT
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)
#define RTE_DEF_INIT_RUNABLE_START
#define RTE_DEF_INIT_RUNABLE(cls, obj, func)
#define RTE_DEF_INIT_RUNABLE_END
#define RTE_DEF_CYCLIC_RUNABLE_START
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc)
#define RTE_DEF_CYCLIC_RUNABLE_END
#define RTE_DEF_EVENT_RUNABLE_START
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func)
#define RTE_DEF_EVENT_RUNABLE_END
#define RTE_DEF_PORT_SR(cls,port) \
  namespace port \
  { \
    extern cls obj; \
    inline ifc_base::ret_type write(const cls::data_type& param) { return obj.write(param); } \
    inline ifc_base::ret_type read(cls::data_type& param) { return obj.read(param); } \
  }
#define RTE_DEF_PORT_SR_CONTAINER(cls,port) \
  namespace port \
  { \
    extern cls obj; \
    inline ifc_base::ret_type write(const cls::array_type& param) { return obj.write(param); } \
    inline ifc_base::ret_type read(cls::array_type& param) { return obj.read(param); } \
    inline ifc_base::ret_type writeElement(cls::size_type pos, const cls::value_type& param) { return obj.writeElement(pos, param); } \
    inline ifc_base::ret_type readElement(cls::size_type pos, cls::value_type& param) { return obj.readElement(pos, param); } \
    /*inline cls::const_iterator begin() { return obj.begin(); }*/ \
    /*inline cls::const_iterator end() { return obj.end(); }*/ \
    inline bool boundaryCheck(cls::size_type pos) { return obj.boundaryCheck(pos); } \
    inline cls::size_type size() { return obj.size(); } \
  }
#define RTE_DEF_PORT_SR_START
#define RTE_DEF_PORT_SR_END
#define RTE_DEF_PORT_CS(cls,port,srvobj,func) \
  namespace port \
  { \
    extern cls obj; \
    template<typename ... Args> inline typename cls::ret_type call(Args... args) { return obj.call(args...); } \
  }

#define RTE_DEF_PORT_CS_START
#define RTE_DEF_PORT_CS_END
#endif

// 16.3.2 The # operator [cpp.stringize]
//
// A character string literal is a string-literal with no prefix. 
// If, in the replacement list, a parameter is immediately preceded by a # preprocessing token, 
// both are replaced by a single character string literal preprocessing token that contains the
// spelling of the preprocessing token sequence for the corresponding argument
#ifdef RTE_DEF_MODE_PORT_ARRAY
#define RTE_DEF_OBJ_START
#define RTE_DEF_OBJ_END
#define RTE_DEF_OBJ(cls, obj)
#define RTE_DEF_INIT_RUNABLE_START
#define RTE_DEF_INIT_RUNABLE(cls, obj, func)
#define RTE_DEF_INIT_RUNABLE_END
#define RTE_DEF_CYCLIC_RUNABLE_START
#define RTE_DEF_CYCLIC_RUNABLE(cls, obj, func, time_off, time_cyc)
#define RTE_DEF_CYCLIC_RUNABLE_END
#define RTE_DEF_EVENT_RUNABLE_START
#define RTE_DEF_EVENT_RUNABLE(eventname, cls, obj, func)
#define RTE_DEF_EVENT_RUNABLE_END
#define RTE_DEF_PORT_SR(cls,port)            { port::obj.data(), #port, port::obj.size(), sizeof(cls::data_type) },
#define RTE_DEF_PORT_SR_CONTAINER(cls,port)  { port::obj.data(), #port, port::obj.size(), sizeof(cls::value_type) },
#define RTE_DEF_PORT_SR_START     port_data_t aPorts[] = {
#define RTE_DEF_PORT_SR_END       };
#define RTE_DEF_PORT_CS(cls,port,srvobj,func)
#define RTE_DEF_PORT_CS_START
#define RTE_DEF_PORT_CS_END
#endif
