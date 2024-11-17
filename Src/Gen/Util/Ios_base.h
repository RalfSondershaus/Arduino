/**
  * @file Ios_base.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Declares and defines util::ios_base.
  *
  * @copyright Copyright 2023, 2024 Ralf Sondershaus
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
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <https://www.gnu.org/licenses/>.
  */

#ifndef UTIL_IOS_BASE_H
#define UTIL_IOS_BASE_H

#include <Std_Types.h>
#include <Util/Ios_Type.h>
#include <Util/Locale.h>

namespace util
{
  // ----------------------------------------------------------------------------------------------------
  /// The class ios_base is a multipurpose class that serves as the base class for all I/O stream classes. 
  /// It maintains several kinds of data
  // ----------------------------------------------------------------------------------------------------
  class ios_base
  {
  public:
    /// state of the stream type
    typedef uint8 iostate;

    /// Specifies available formatting flags
    typedef uint16 fmtflags;

    /// stream open mode type
    typedef uint8 openmode;

    /// seeking direction type 
    typedef uint8 seekdir;

    static constexpr fmtflags dec = 1;
    static constexpr fmtflags oct = 2;
    static constexpr fmtflags hex = 4;
    static constexpr fmtflags zero_ = 0;
    static constexpr fmtflags basefield = dec | oct | hex;

    static constexpr iostate goodbit = 0;
    static constexpr iostate badbit = 1;
    static constexpr iostate failbit = 2;
    static constexpr iostate eofbit = 4;

    static constexpr openmode app = 1;
    static constexpr openmode binary = 2;
    static constexpr openmode in = 4;
    static constexpr openmode out = 8;
    static constexpr openmode trunc = 16;
    static constexpr openmode ate = 32;

    static constexpr seekdir beg = 1;
    static constexpr seekdir end = 2;
    static constexpr seekdir cur = 4;

  protected:
    fmtflags my_fmtflags;
    iostate my_iostate;
    locale my_locale;

    /// Only derived classes may construct std::ios_base.
    /// The internal state is undefined after the construction.
    /// The derived class must call std::basic_ios::init() to complete initialization 
    /// before first use or before destructor; otherwise the behavior is undefined
    ios_base() = default;

  public:
    /// streams are not copyable
    ios_base(const ios_base&) = delete;

    /// Sets the formatting flags identified by flags
    fmtflags setf(fmtflags flags) { fmtflags oldflags = my_fmtflags; my_fmtflags |= flags; return oldflags; }
    /// Clears the formatting flags under mask, and sets the cleared flags to those specified by flags.
    /// Returns the formatting flags before the call of this function.
    fmtflags setf(fmtflags flags, fmtflags mask) { fmtflags oldflags = my_fmtflags; my_fmtflags &= ~mask; my_fmtflags |= (flags & mask); return oldflags; }
    /// Unsets the formatting flags identified by flags.
    void unsetf(fmtflags flags) { my_fmtflags &= ~flags; }
    /// returns current formatting setting
    fmtflags flags() const { return my_fmtflags; }

    /// Returns the current locale
    locale getloc() const { return my_locale; }
    /// Sets the locale and returns the previous locale
    locale imbue(const locale& loc)
    {
      locale old_locale = my_locale;
      my_locale = loc;
      return old_locale;
    }
  };

} // namespace util

#endif // UTIL_IOS_BASE_H