/**
 * @file Rte.h
 * @author Ralf Sondershaus
 *
 * @brief Run Time Environment (RTE)
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

#include <Rte/Rte_Type.h>
#include <Util/Array.h>

namespace Rte
{
  // -------------------------------------------------
  /// Run Time Environment (RTE)
  /// @tparam NI  Number of init runables
  /// @tparam NC  Number of cyclic runables
  // -------------------------------------------------
  template<int NI, int NC>
  class RTE
  {
  public:
    typedef RTE<NI, NC> This;
    typedef Rte::Runable& runable_reference;
    typedef Rte::Runable* runable_pointer;
    typedef typename Rte::time_type   time_type;
    typedef typename Rte::timer_type  timer_type;

    // To initialize RCBs from constructor parameters
    typedef struct
    {
      time_type         ulStartOffset;      ///< [us] Offset time at start up
      time_type         ulCycleTime;        ///< [us] Cycle time
      runable_pointer   rnbl;               ///< reference to the runable
    } rcb_cfg_type;

    /// Array for runable configuration via constructor
    typedef Util::Array<rcb_cfg_type, NC> tCyclicCfgArray;
    /// Array for init runables
    typedef Util::Array<runable_pointer, NI> tInitArray;

    typedef typename tCyclicCfgArray::const_iterator rcb_cfg_const_iterator;

    static constexpr int getNumberOfInitRunables() noexcept { return NI; }
    static constexpr int getNumberOfCyclicRunables() noexcept { return NC; }

  protected:
    // The Runable Control Block (RCB)
    typedef struct
    {
      timer_type              timer; ///< The timer for the next call
      rcb_cfg_const_iterator  cfg;   ///< the configuration is available via a const iterator
    } rcb_type;
    /// Array for RCBs
    typedef Util::Array<rcb_type, NC> tRCBArray;

    tRCBArray mRCBArray;
    tInitArray mInitArray;

  public:
    /// Construct from external arrays
    RTE(const tInitArray ira, const tCyclicCfgArray& crca) : mInitArray(ira)
    {
      auto it_crca = crca.begin();
      auto it_rcb = mRCBArray.begin();

      for (; it_crca != crca.end(); it_crca++, it_rcb++)
      { 
        it_rcb->cfg = it_crca;
      }
    }
    /// Start RTE, call init runables and initialize timers for cyclic runables
    void start()
    {
      for (auto it = mInitArray.begin(); it != mInitArray.end(); it++)
      {
        (*it)->run();
      }
      for (auto it = mRCBArray.begin(); it != mRCBArray.end(); it++)
      {
        (void)(it->timer.start(it->cfg->ulStartOffset));
      }
    }
    /// Call from TASK context or loop context (Arduino's loop()). Calls a runable
    /// when the runable's timer is elapsed.
    void exec1()
    {
      for (auto it = mRCBArray.begin(); it != mRCBArray.end(); it++)
      {
        if (it->timer.timeout())
        {
          it->cfg->rnbl->run();
          (void)(it->timer.increment(it->cfg->ulCycleTime));
        }
      }
    }
  };
} // namespace Rte

#endif /* RTE_H_ */
