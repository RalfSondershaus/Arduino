/**
  * @file bitset.h
  *
  * @author Ralf Sondershaus
  *
  * @brief A bitset class
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

#ifndef BITSET_H_
#define BITSET_H_

#include <Std_Types.h>

namespace util
{
  // --------------------------------------------------------------------
  /// bit queries and bit manipulation
  // --------------------------------------------------------------------
  namespace bits
  {
    constexpr size_t bits_per_char() noexcept { return 8U; }

    template<typename W> constexpr size_t bits_per_word() noexcept { return bits_per_char() * sizeof(W); }
    template<typename W> constexpr W all_one() noexcept { return static_cast<W>(~static_cast<W>(0)); }
    template<typename W> constexpr W all_zero() noexcept { return static_cast<W>(0); }
    /// Returns a bit mask for a single bit such as 0b00000001U for bit_mask_n<uint8>(0U)
    template<typename W> constexpr W bit_mask(size_t bitpos) noexcept { return static_cast<W>(1) << bitpos; }
    /// Returns a bit mask for nr_bits such as 0b00001111U for bit_mask_n<uint8>(4U)
    template<typename W> constexpr W bit_mask_n(size_t nr_bits) { return ((1U << (nr_bits + 1U)) - 1U); }
    template<typename W> constexpr bool test(W w, size_t bitpos) noexcept { return (w & bit_mask<W>(bitpos)) != static_cast<W>(0); }
    template<typename W> constexpr size_t nr_LSB_zeros(W w) noexcept;
    template<typename W> constexpr size_t first(W w, size_t notfound) noexcept;
    template<typename W> constexpr size_t nr_words(size_t nbits) noexcept { return (nbits + (bits_per_word<W>() - static_cast<size_t>(1))) / bits_per_word<W>(); }
    template<typename W1, typename W2> constexpr W2 stencil(W1 w, W1 mask) { return static_cast<W2>(w & mask); }
    /// Extract the value at position pos and length nr_bits. W needs to be an unsigned type.
    template<typename W> W extract(W w, size_t pos, size_t nr_bits) { return static_cast<W>((w >> pos) & bit_mask_n<W>(nr_bits)); }
    template<typename W> constexpr size_t nr_LSB_zeros(W w) noexcept
    {
      size_t cnt = 0;
      while ((w & static_cast<W>(1)) == 0)
      {
        cnt++;
        w >>= 1U;
      }
      return cnt;
    }
    template<typename W> constexpr size_t first(W w, size_t notfound) noexcept
    {
      size_t retpos;

      if (w == static_cast<W>(0))
      {
        retpos = notfound;
      }
      else
      {
        retpos = getLSBZeros(w);
      }
      return retpos;
    }
  } // namespace bits

  // --------------------------------------------------------------------
  /// base class
  // --------------------------------------------------------------------
  template<typename W, size_t NWORDS>
  struct bitset_base
  {
    typedef W tWord;

    tWord aWords[NWORDS];

    constexpr bitset_base() noexcept : aWords() {}
    constexpr bitset_base(tWord v) noexcept : aWords{ v } {}

    static constexpr size_t bits_per_char() noexcept { return 8U; }
    static constexpr size_t bits_per_word() noexcept { return bits_per_char() * sizeof(tWord); }
    static constexpr size_t which_word(size_t pos) noexcept { return pos / bits_per_word(); }
    static constexpr size_t which_bit(size_t pos) noexcept { return pos % bits_per_word(); }
    template<size_t NBITS>
    static constexpr tWord hiword_nr_bits() noexcept { return static_cast<tWord>(NWORDS * bits_per_word() - NBITS); }

          tWord& ref_word(size_t pos) noexcept { return aWords[which_word(pos)]; }
    const tWord& ref_word(size_t pos) const noexcept { return aWords[which_word(pos)]; }
          tWord  get_word(size_t pos) const noexcept { return aWords[which_word(pos)]; }
          tWord& hiword() noexcept { return aWords[NWORDS - 1]; }
    const tWord& hiword() const noexcept { return aWords[NWORDS - 1]; }

    template<size_t NBITS>
    bool all() const noexcept
    {
      for (size_t i = 0; i < NWORDS - 1; i++)
      {
        if (aWords[i] != bits::all_one<tWord>())
        {
          return false;
        }
      }
      return (hiword() == (bits::all_one<tWord>() >> hiword_nr_bits<NBITS>()));
    }
    bool any() const noexcept
    {
      for (size_t i = 0; i < NWORDS; i++)
      {
        if (aWords[i] != bits::all_zero<tWord>())
        {
          return true;
        }
      }
      return false;
    }
    bool none() const noexcept  { return !any(); }

    template<size_t NBITS>
    void set() noexcept
    { 
      for (size_t i = 0; i < NWORDS - 1; i++)
      {
        aWords[i] = bits::all_one<tWord>();
      }
      hiword() |= bits::all_one<tWord>() >> hiword_nr_bits<NBITS>(); 
    }
    void set(size_t pos, bool value = true) noexcept
    {
      if (pos < NWORDS * bits_per_word())
      {
        if (value)
        {
          ref_word(pos) |= bits::bit_mask<tWord>(which_bit(pos));
        }
        else
        {
          ref_word(pos) &= ~bits::bit_mask<tWord>(which_bit(pos));
        }
      }
    }
    void reset() noexcept
    {
      for (size_t i = 0; i < NWORDS; i++)
      {
        aWords[i] = bits::all_zero<tWord>();
      }
    }
    void reset(size_t pos) noexcept
    {
      if (pos < NWORDS * bits_per_word())
      {
        ref_word(pos) &= ~bits::bit_mask<tWord>(which_bit(pos));
      }
    }

    size_t find_first(size_t notfound) const
    {
      for (size_t i = 0; i < NWORDS; i++)
      {
        size_t bitpos = bits::first(aWords[i], static_cast<size_t>(0), notfound);
        if (bitpos != notfound)
        {
          return i * bits_per_word() + bitpos;
        }
      }
      return notfound;
    }
    size_t find_next(size_t prevpos, size_t notfound) const
    {
      size_t pos;

      prevpos++;

      if (prevpos >= NWORDS * bits_per_word())
      {
        pos = notfound;
      }
      else 
      {
        size_t wordpos = which_word(prevpos);
        size_t bitpos = which_bit(prevpos);
        tWord w = aWords[wordpos];
        w >>= bitpos;
        pos = bits::first(w, notfound);
        if (pos == notfound)
        {
          wordpos++;
          for (; (wordpos < NWORDS) && (pos != notfound); wordpos++)
          {
            tWord w = aWords[wordpos];
            pos = bits::first(w, notfound);
          }
        }
      }
      return pos;
    }
  };

  // --------------------------------------------------------------------
  /// partial specialization of base class for important use case NWORDS = 0
  // --------------------------------------------------------------------
  template<typename W>
  struct bitset_base<W, 0U>
  {
    typedef W tWord;

    tWord w;

    constexpr bitset_base() noexcept : w() {}
    constexpr bitset_base(tWord v) noexcept : w{ v } {}

    static constexpr size_t bits_per_char() noexcept { return 8U; }
    static constexpr size_t bits_per_word() noexcept { return bits_per_char() * sizeof(tWord); }
    static constexpr size_t which_bit(size_t pos) noexcept { return pos; }
    template<size_t NBITS>
    static constexpr tWord hiword_nr_bits() noexcept { return static_cast<tWord>(0); }

    constexpr tWord& ref_word(size_t pos) noexcept { (void)pos; return w; }
    constexpr tWord  get_word(size_t pos) const noexcept { (void)pos; return static_cast<tWord>(0); }
    tWord& hiword() noexcept { return w; }
    constexpr tWord  hiword() const noexcept { return static_cast<tWord>(0); }

    template<size_t NBITS>
    bool all() const noexcept { return true; }
    bool any() const noexcept { return false; }
    bool none() const noexcept { return false; }

    template<size_t NBITS>
    void set() noexcept {}
    void set(size_t pos, bool value = true) noexcept { (void)pos; (void)value; }

    void reset() noexcept {}
    void reset(size_t pos) noexcept { (void)pos; }

    size_t find_first(size_t notfound) const { return notfound; }
    size_t find_next(size_t prevpos, size_t notfound) const { return notfound; }
  };

  // --------------------------------------------------------------------
  /// partial specialization of base class for important use case NWORDS = 1
  // --------------------------------------------------------------------
  template<typename W>
  struct bitset_base<W, 1U>
  {
    typedef W tWord;

    tWord w;

    constexpr bitset_base() noexcept : w() {}
    constexpr bitset_base(tWord v) noexcept : w{ v } {}

    static constexpr size_t bits_per_char() noexcept { return 8U; }
    static constexpr size_t bits_per_word() noexcept { return bits_per_char() * sizeof(tWord); }
    static constexpr size_t which_bit(size_t pos) noexcept { return pos; }
    template<size_t NBITS>
    static constexpr tWord hiword_nr_bits() noexcept { return static_cast<tWord>(bits_per_word() - NBITS); }

    constexpr tWord& ref_word(size_t pos) noexcept { (void)pos; return w; }
    constexpr tWord  get_word(size_t pos) const noexcept { (void)pos; return w; }
              tWord& hiword() noexcept { return w; }
    constexpr tWord  hiword() const noexcept { return w; }

    template<size_t NBITS>
    bool all() const noexcept { return w == (bits::all_one<tWord>() >> hiword_nr_bits<NBITS>()); }
    bool any() const noexcept { return w != bits::all_zero<tWord>(); }
    bool none() const noexcept { return !any(); }

    template<size_t NBITS>
    void set() noexcept { w = bits::all_one<tWord>() >> hiword_nr_bits<NBITS>(); }
    void set(size_t pos, bool value = true) noexcept 
    { 
      if (pos < bits_per_word())
      {
        if (value)
        {
          w |= bits::bit_mask<tWord>(which_bit(pos));
        }
        else
        {
          w &= ~bits::bit_mask<tWord>(which_bit(pos));
        }
      }
    }

    void reset() noexcept { w = bits::all_zero<tWord>(); }
    void reset(size_t pos) noexcept
    {
      if (pos < bits_per_word())
      {
        w &= ~bits::bit_mask<tWord>(which_bit(pos));
      }
    }

    size_t find_first(size_t notfound) const { return bits::first(w, notfound); }
    size_t find_next(size_t prevpos, size_t notfound) const 
    {
      size_t pos;

      prevpos++;

      if (prevpos >= bits_per_word())
      {
        pos = notfound;
      }
      else
      {
        pos = bits::first(w >> which_bit(prevpos), notfound);
      }
      return pos;
    }
  };

  // --------------------------------------------------------------------
  /// bitset
  // --------------------------------------------------------------------
  template<typename W, size_t NBITS>
  class bitset : private bitset_base<W, bits::nr_words<W>(NBITS)>
  {
    typedef bitset_base<W, bits::nr_words<W>(NBITS)> Base;
  public:
    typedef typename Base::tWord tWord;
    typedef bitset<W, NBITS> This;
  public:
    /// Default constructor. Constructs a bitset with all bits set to zero.
    constexpr bitset() : Base() {}
    /// Constructs a bitset, initializing the first (rightmost, least significant) M bit positions to the corresponding bit values of val.
    constexpr bitset(tWord val) : Base(val) {}

    /// Accesses the bit at position pos. Returns the value of the bit.
    constexpr bool operator[](size_t pos) const { return (Base::get_word(pos) & bits::bit_mask<tWord>(Base::which_bit(pos))) != static_cast<tWord>(0); }

    /// Returns the value of the bit at the position pos.
    bool test(size_t pos) const { return this->operator[](pos); }

    /// Checks if all bits are set to true
#ifdef _MSC_VER
    bool all() const noexcept { return Base::all<NBITS>(); }
#else
    bool all() const noexcept { return Base::all(); }
#endif
    /// Checks if any bits are set to true
    bool any() const noexcept { return Base::any(); }
    /// Checks if none of the bits are set to true
    bool none() const noexcept { return Base::none(); }

    /// Sets all bits to true or sets one bit to specified value
#ifdef _MSC_VER
    This& set() noexcept { Base::set<NBITS>(); return *this; }
#else
    This& set() noexcept { Base::set(); return *this; }
#endif
    This& set(size_t pos, bool value = true) { Base::set(pos, value); return *this; }

    /// Sets bits to false
    This& reset() noexcept { Base::reset(); return *this; }
    This& reset(size_t pos) { Base::reset(pos); return *this; }
  };
} // namespace util

#endif // BITSET_H_
