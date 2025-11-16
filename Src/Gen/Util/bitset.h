/**
 * @file bitset.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Provides a fixed-size bitset class template with efficient bit manipulation utilities.
 *
 * This file defines the util::bitset class template and supporting utilities for bitwise operations,
 * allowing storage and manipulation of a sequence of bits using customizable word types and sizes.
 * It includes base classes for different storage specializations and helper functions for bit operations.
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
#include <Platform_Limits.h>

namespace util
{
  // --------------------------------------------------------------------
  /// @namespace util::bits
  /// @brief Provides low-level bit manipulation utilities for use with util::bitset.
  ///
  /// This namespace contains constexpr helper functions for bitwise operations,
  /// such as creating bit masks, testing bits, counting zeros, extracting bit fields,
  /// and determining storage requirements for bitsets.
  ///
  /// These utilities are designed for efficiency and are used internally by the bitset implementation.  // --------------------------------------------------------------------
  // --------------------------------------------------------------------
  namespace bits
  {
    /// @brief Returns the number of bits in a char (always 8).
    /// @return Number of bits in a char (8).
    constexpr size_t bits_per_char() noexcept { return 8U; }

    /// @brief Returns the number of bits in the word type W.
    /// @tparam W The word type.
    /// @return Number of bits in the word type.
    template <typename W>
    constexpr size_t bits_per_word() noexcept { return bits_per_char() * sizeof(W); }

    /// @brief Returns a value with all bits set to 1 for the given word type.
    /// @tparam W The word type.
    /// @return Value with all bits set.
    template <typename W>
    constexpr W all_one() noexcept { return static_cast<W>(~static_cast<W>(0)); }

    /// @brief Returns a value with all bits set to 0 for the given word type.
    /// @tparam W The word type.
    /// @return Value with all bits cleared.
    template <typename W>
    constexpr W all_zero() noexcept { return static_cast<W>(0); }

    /// @brief Returns a bit mask for a single bit at the given position.
    /// @tparam W The word type.
    /// @param bitpos Bit position (0 = least significant).
    /// @return Bit mask with only the specified bit set.
    template <typename W>
    constexpr W bit_mask(size_t bitpos) noexcept { return static_cast<W>(1) << bitpos; }

    /// @brief Returns a bit mask with the lowest nr_bits+1 bits set.
    /// @tparam W The word type.
    /// @param nr_bits Number of bits to set (0 = only LSB set).
    /// @return Bit mask with the lowest nr_bits+1 bits set.
    template <typename W>
    constexpr W bit_mask_n(size_t nr_bits) { return ((1U << (nr_bits + 1U)) - 1U); }

    /// @brief Tests if the bit at bitpos is set in w.
    /// @tparam W The word type.
    /// @param w The word value.
    /// @param bitpos Bit position to test.
    /// @return true if the bit is set, false otherwise.
    template <typename W>
    constexpr bool test(W w, size_t bitpos) noexcept { return (w & bit_mask<W>(bitpos)) != static_cast<W>(0); }

    /// @brief Counts the number of least significant zero bits in w.
    /// @tparam W The word type.
    /// @param w The word value.
    /// @return Number of consecutive zero bits starting from the least significant bit.
    template <typename W>
    constexpr size_t nr_LSB_zeros(W w) noexcept;

    /// @brief Returns the position of the first set bit in w, or notfound if none are set.
    /// @tparam W The word type.
    /// @param w The word value.
    /// @param notfound Value to return if no bit is set.
    /// @return Position of the first set bit, or notfound.
    template <typename W>
    constexpr size_t first(W w, size_t notfound) noexcept;

    /// @brief Calculates the number of words required to store nbits bits.
    /// @tparam W The word type.
    /// @param nbits Number of bits to store.
    /// @return Number of words required.
    template <typename W>
    constexpr size_t nr_words(size_t nbits) noexcept { return (nbits + (bits_per_word<W>() - static_cast<size_t>(1))) / bits_per_word<W>(); }

    /// @brief Applies a bitmask to the input value and returns the result as type W2 (no shifts).
    /// @tparam W1 Type of input value and mask.
    /// @tparam W2 Desired output type.
    /// @param w Input value to be masked.
    /// @param mask Bitmask to apply.
    /// @return Result of (w & mask), converted to type W2.
    template <typename W1, typename W2>
    constexpr W2 apply_mask_as(W1 w, W1 mask) { return static_cast<W2>(w & mask); }

    /// @brief Extracts a bitfield from w by (1) right shifting by pos, then (2) applying the mask to isolate nr_bits.
    /// @tparam W The word type (must be unsigned).
    /// @param w Input value.
    /// @param pos Starting bit position.
    /// @param nr_bits Number of bits to extract.
    /// @return Extracted value.
    template <typename W>
    constexpr W bitfield_at(W w, size_t pos, size_t nr_bits) { return static_cast<W>((w >> pos) & bit_mask_n<W>(nr_bits)); }

    /// @brief Extracts a bitfield from w by applying (1) the mask and (2) the shift.
    /// @tparam W The word type (must be unsigned).
    /// @param w Input value.
    /// @param mask The bit mask to be applied
    /// @param shift Value for right shift.
    /// @return Extracted value.
    template <typename W>
    constexpr W masked_shift(W w, W mask, W shift) { return static_cast<W>(apply_mask_as<W,W>(w, mask) >> shift); }

    /// @brief Counts the number of least significant zero bits in w.
    /// @tparam W The word type.
    /// @param w The word value.
    /// @return Number of consecutive zero bits starting from the least significant bit.
    template <typename W>
    constexpr size_t nr_LSB_zeros(W w) noexcept
    {
      size_t cnt = 0;
      while ((w & static_cast<W>(1)) == 0)
      {
        cnt++;
        w >>= 1U;
      }
      return cnt;
    }

    /// @brief Returns the position of the first set bit in w, or notfound if none are set.
    /// @tparam W The word type.
    /// @param w The word value.
    /// @param notfound Value to return if no bit is set.
    /// @return Position of the first set bit, or notfound.
    template <typename W>
    constexpr size_t first(W w, size_t notfound) noexcept
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

    /**
     * @brief Performs a left bitwise shift operation.
     *
     * This function shifts the value `w` to the left by `nr_bits` bits.
     * It is implemented as a constexpr and marked noexcept.
     *
     * @tparam W Integral type (e.g., uint8, int, etc.)
     * @param w The value to be shifted.
     * @param nr_bits The number of bits to shift.
     * @return The result of shifting `w` left by `nr_bits` bits.
     */
    template<typename W>
    constexpr W lshift(W w, W nr_bits) noexcept
    {
        // Shifting by a value greater than or equal to the bit width of the type (shft >= sizeof(W) * 8) 
        // is undefined behavior in C++.
        return (nr_bits < platform::numeric_limits<W>::digits) ? static_cast<W>(w << nr_bits) : 0;
    }
  } // namespace bits

  // --------------------------------------------------------------------
  /// @brief Base class for util::bitset, storing NWORDS words of type W.
  ///
  /// This struct provides the core storage and bit manipulation operations for a fixed-size bitset.
  /// It is specialized for different numbers of storage words to optimize for space and performance.
  ///
  /// @tparam W      Underlying word type (e.g., uint8_t, uint16_t, uint32_t).
  /// @tparam NWORDS Number of words used for storage.
  // --------------------------------------------------------------------
  template <typename W, size_t NWORDS>
  struct bitset_base
  {
    /// @brief Underlying word type used for storage.
    using tWord = W;

    /// @brief Type alias for this bitset_base specialization.
    using This = bitset_base<W, NWORDS>;

    /// @brief Number of words used for storage.
    static constexpr size_t kNrWords = NWORDS;

    /// @brief Storage array for the bitset words.
    tWord aWords[NWORDS];

    /// @brief Default constructor. Initializes all words to zero.
    constexpr bitset_base() noexcept = default;

    /// @brief Constructor that initializes the first word to a given value.
    /// @param v Initial value for the first word.
    constexpr bitset_base(tWord v) noexcept : aWords{v} {}

    /// @brief Returns the number of bits in a char (always 8).
    /// @return Number of bits in a char (8).
    static constexpr size_t bits_per_char() noexcept { return 8U; }

    /// @brief Returns the number of bits in the word type.
    /// @return Number of bits in the word type.
    static constexpr size_t bits_per_word() noexcept { return bits_per_char() * sizeof(tWord); }

    /// @brief Returns the word index for a given bit position.
    /// @param pos Bit position.
    /// @return Index of the word containing the bit.
    static constexpr size_t which_word(size_t pos) noexcept { return pos / bits_per_word(); }

    /// @brief Returns the bit index within a word for a given bit position.
    /// @param pos Bit position.
    /// @return Bit index within the word.
    static constexpr size_t which_bit(size_t pos) noexcept { return pos % bits_per_word(); }

    /// @brief Returns the number of unused bits in the high word for NBITS.
    /// @tparam NBITS Number of bits in the bitset.
    /// @return Number of unused bits in the high word.
    template <size_t NBITS>
    static constexpr tWord hiword_nr_bits() noexcept { return static_cast<tWord>(NWORDS * bits_per_word() - NBITS); }

    /// @brief Returns a reference to the word containing the specified bit position.
    /// @param pos Bit position.
    /// @return Reference to the word.
    tWord &ref_word(size_t pos) noexcept { return aWords[which_word(pos)]; }

    /// @brief Returns a const reference to the word containing the specified bit position.
    /// @param pos Bit position.
    /// @return Const reference to the word.
    const tWord &ref_word(size_t pos) const noexcept { return aWords[which_word(pos)]; }

    /// @brief Returns the value of the word containing the specified bit position.
    /// @param pos Bit position.
    /// @return Value of the word.
    tWord get_word(size_t pos) const noexcept { return aWords[which_word(pos)]; }

    /// @brief Returns a reference to the highest word in the bitset.
    /// @return Reference to the highest word.
    tWord &hiword() noexcept { return aWords[NWORDS - 1]; }

    /// @brief Returns a const reference to the highest word in the bitset.
    /// @return Const reference to the highest word.
    const tWord &hiword() const noexcept { return aWords[NWORDS - 1]; }

    /// @brief Returns true if all bits are set to true.
    /// @tparam NBITS Number of bits in the bitset.
    /// @return true if all bits are set, false otherwise.
    template <size_t NBITS>
    bool all() const noexcept
    {
      for (size_t i = 0; i < NWORDS - 1; i++)
      {
        // use aWords instead of refWord() because the refWord parameter is bit position
        // (and not word position).
        if (aWords[i] != bits::all_one<tWord>())
        {
          return false;
        }
      }
      return (hiword() == (bits::all_one<tWord>() >> hiword_nr_bits<NBITS>()));
    }
    /// @brief Returns true if at least one bit is set to true.
    /// @return true if any bit is set, false otherwise.
    bool any() const noexcept
    {
      for (size_t i = 0; i < NWORDS; i++)
      {
        // use aWords instead of refWord() because the refWord parameter is bit position
        // (and not word position).
        if (aWords[i] != bits::all_zero<tWord>())
        {
          return true;
        }
      }
      return false;
    }
    /// @brief Returns true if all bits are set to false.
    /// @return true if all bits are false, false otherwise.
    bool none() const noexcept { return !any(); }

    /// @brief Sets all bits to true.
    /// @tparam NBITS Number of bits in the bitset.
    template <size_t NBITS>
    void set() noexcept
    {
      for (size_t i = 0; i < NWORDS - 1; i++)
      {
        // use aWords instead of refWord() because the refWord parameter is bit position
        // (and not word position).
        aWords[i] = bits::all_one<tWord>();
      }
      hiword() |= bits::all_one<tWord>() >> hiword_nr_bits<NBITS>();
    }
    /// @brief Sets the bit at the specified position to the given value.
    /// @param pos Bit position.
    /// @param value Value to set (true = 1, false = 0). Default is true.
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
    /// @brief Clears all bits (sets all bits to false).
    void reset() noexcept
    {
      for (size_t i = 0; i < NWORDS; i++)
      {
        // use aWords instead of refWord() because the refWord parameter is bit position
        // (and not word position).
        aWords[i] = bits::all_zero<tWord>();
      }
    }
    /// @brief Clears the bit at the specified position (sets to false).
    /// @param pos Bit position.
    void reset(size_t pos) noexcept
    {
      if (pos < NWORDS * bits_per_word())
      {
        ref_word(pos) &= ~bits::bit_mask<tWord>(which_bit(pos));
      }
    }

    /// @brief Finds the position of the first set bit.
    /// @param notfound Value to return if no bit is set.
    /// @return Position of the first set bit, or notfound if none are set.
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
    /// @brief Finds the position of the next set bit after prevpos.
    /// @param prevpos Previous bit position.
    /// @param notfound Value to return if no next set bit is found.
    /// @return Position of the next set bit, or notfound if none are found.
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
            w = aWords[wordpos];
            pos = bits::first(w, notfound);
          }
        }
      }
      return pos;
    }
  };

  // --------------------------------------------------------------------
  /// @brief Base implementation of a bitset for the special case where the number of words is zero.
  ///
  /// This template class provides a minimal interface for a bitset with zero storage words.
  /// It is used as a helper for template specialization and provides no actual storage or bit manipulation.
  /// All member functions are implemented as no-ops or return default values.
  ///
  /// @tparam W Type of the underlying word (unused in this specialization).
  /// @tparam kNrWords Number of words in the bitset (should be zero for this specialization).
  // --------------------------------------------------------------------
  template <typename W>
  struct bitset_base<W, 0U>
  {
    /// @brief Number of words in the bitset (always 0 for this specialization).
    static constexpr size_t kNrWords = 0;

    using tWord = W;
    using This = bitset_base<W, kNrWords>;

    tWord w;

    /// @brief Default constructor. Initializes the word to zero.
    constexpr bitset_base() noexcept : w() {}
    /// @brief Constructor that initializes the word to a given value.
    /// @param v Initial value for the word.
    constexpr bitset_base(tWord v) noexcept : w{v} {}

    /// @brief Returns the number of bits in a char (always 8).
    static constexpr size_t bits_per_char() noexcept { return 8U; }
    /// @brief Returns the number of bits in the word type.
    static constexpr size_t bits_per_word() noexcept { return bits_per_char() * sizeof(tWord); }
    /// @brief Returns the bit index within the word for a given position.
    /// @param pos Bit position.
    static constexpr size_t which_bit(size_t pos) noexcept { return pos; }
    /// @brief Returns the number of unused bits in the high word for NBITS.
    /// @tparam NBITS Number of bits in the bitset.
    template <size_t NBITS>
    static constexpr tWord hiword_nr_bits() noexcept { return static_cast<tWord>(0); }

    /// @brief Returns a reference to the word (same as w).
    /// @param pos Bit position (ignored).
    constexpr tWord &ref_word(size_t pos) noexcept
    {
      (void)pos;
      return w;
    }
    /// @brief Returns the value of the word (same as w).
    /// @param pos Bit position (ignored).
    constexpr tWord get_word(size_t pos) const noexcept
    {
      (void)pos;
      return static_cast<tWord>(0);
    }
    /// @brief Returns a reference to the high word (same as w).
    tWord &hiword() noexcept { return w; }
    /// @brief Returns the value of the high word (same as w).
    constexpr tWord hiword() const noexcept { return static_cast<tWord>(0); }

    /// @brief Returns true if all bits are set (always true for NWORDS=0).
    /// @tparam NBITS Number of bits in the bitset (ignored).
    /// @return Always true.
    template <size_t NBITS>
    bool all() const noexcept { return true; }

    /// @brief Returns true if any bit is set (always false for NWORDS=0).
    /// @return Always false.
    bool any() const noexcept { return false; }

    /// @brief Returns true if no bits are set (always false for NWORDS=0).
    /// @return Always false.
    bool none() const noexcept { return false; }

    /// @brief Sets all bits to true (no-op for NWORDS=0).
    /// @tparam NBITS Number of bits in the bitset (ignored).
    template <size_t NBITS>
    void set() noexcept {}

    /// @brief Sets the bit at position pos to value (no-op for NWORDS=0).
    /// @param pos Bit position (ignored).
    /// @param value Value to set (ignored).
    void set(size_t pos, bool value = true) noexcept
    {
      (void)pos;
      (void)value;
    }

    /// @brief Clears all bits (no-op for NWORDS=0).
    void reset() noexcept {}

    /// @brief Clears the bit at position pos (no-op for NWORDS=0).
    /// @param pos Bit position (ignored).
    void reset(size_t pos) noexcept { (void)pos; }

    /// @brief Finds the first set bit (always returns notfound for NWORDS=0).
    /// @param notfound Value to return if no bit is found.
    /// @return Always returns notfound.
    size_t find_first(size_t notfound) const { return notfound; }

    /// @brief Finds the next set bit after prevpos (always returns notfound for NWORDS=0).
    /// @param prevpos Previous bit position (ignored).
    /// @param notfound Value to return if no bit is found.
    /// @return Always returns notfound.
    size_t find_next(size_t prevpos, size_t notfound) const { return notfound; }
  };

  // --------------------------------------------------------------------
  /// @brief Partial specialization of bitset_base for the case where NWORDS == 1.
  ///
  /// This specialization optimizes storage and operations for a single word bitset.
  /// All bits are stored in a single word, and bit manipulation functions are implemented accordingly.
  ///
  /// @tparam W Underlying word type (e.g., uint8_t, uint16_t, uint32_t).
  // --------------------------------------------------------------------
  template <typename W>
  struct bitset_base<W, 1U>
  {
    /// @brief Number of words in the bitset (always 1 for this specialization).
    static constexpr size_t kNrWords = 1;

    /// @brief Underlying word type used for storage.
    using tWord = W;

    /// @brief Type alias for this bitset_base specialization.
    using This = bitset_base<W, kNrWords>;

    /// @brief Storage for the single word bitset.
    tWord w;
    
    /// @brief Default constructor. Initializes the word to zero.
    constexpr bitset_base() noexcept : w() {}
    /// @brief Constructor that initializes the word to a given value.
    /// @param v Initial value for the word.
    constexpr bitset_base(tWord v) noexcept : w{v} {}

    /// @brief Returns the number of bits in a char (always 8).
    static constexpr size_t bits_per_char() noexcept { return 8U; }
    /// @brief Returns the number of bits in the word type.
    static constexpr size_t bits_per_word() noexcept { return bits_per_char() * sizeof(tWord); }
    /// @brief Returns the bit index within the word for a given position.
    /// @param pos Bit position.
    static constexpr size_t which_bit(size_t pos) noexcept { return pos; }
    /// @brief Returns the number of unused bits in the high word for NBITS.
    /// @tparam NBITS Number of bits in the bitset.
    template <size_t NBITS>
    static constexpr tWord hiword_nr_bits() noexcept { return static_cast<tWord>(bits_per_word() - NBITS); }

    /// @brief Returns a reference to the word (same as w).
    /// @param pos Bit position.
    constexpr tWord &ref_word(size_t pos) noexcept
    {
      (void)pos;
      return w;
    }
    /// @brief Returns the value of the word (same as w).
    /// @param pos Bit position.
    constexpr tWord get_word(size_t pos) const noexcept
    {
      (void)pos;
      return w;
    }
    /// @brief Returns a reference to the high word (same as w).
    tWord &hiword() noexcept { return w; }
    /// @brief Returns the value of the high word (same as w).
    constexpr tWord hiword() const noexcept { return w; }

    /// @brief Returns true if all bits are set to true.
    /// @tparam NBITS Number of bits in the bitset.
    /// @return true if all bits are set, false otherwise.
    template <size_t NBITS>
    bool all() const noexcept { return hiword() == (bits::all_one<tWord>() >> hiword_nr_bits<NBITS>()); }
    /// @brief Returns true if at least one bit is set to true.
    /// @return true if any bit is set, false otherwise.
    bool any() const noexcept { return w != bits::all_zero<tWord>(); }
    /// @brief Returns true if all bits are set to false.
    /// @return true if all bits are false, false otherwise.
    bool none() const noexcept { return !any(); }

    /// @brief Sets all bits to true.
    /// @tparam NBITS Number of bits in the bitset.
    template <size_t NBITS>
    void set() noexcept
    {
      hiword() = bits::all_one<tWord>() >> hiword_nr_bits<NBITS>();
    }
    /// @brief Sets the bit at position pos to value.
    /// @param pos Bit position.
    /// @param value Value to set.
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

    /// @brief Clears all bits (sets all bits to false).
    void reset() noexcept { w = bits::all_zero<tWord>(); }
    /// @brief Clears the bit at the specified position (sets to false).
    /// @param pos Bit position.
    void reset(size_t pos) noexcept
    {
      if (pos < bits_per_word())
      {
        w &= ~bits::bit_mask<tWord>(which_bit(pos));
      }
    }

    /// @brief Finds the position of the first set bit.
    /// @param notfound Value to return if no bit is set.
    /// @return Position of the first set bit, or notfound if none are set.
    size_t find_first(size_t notfound) const { return bits::first(w, notfound); }
    /// @brief Finds the position of the next set bit after prevpos.
    /// @param prevpos Previous bit position.
    /// @param notfound Value to return if no next set bit is found.
    /// @return Position of the next set bit, or notfound if none are found.
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
  /// @brief A fixed-size sequence of bits, similar to std::bitset.
  ///
  /// This class template provides a bitset of NBITS bits, stored in words of type W.
  /// It supports bit access, bitwise queries, and bit manipulation operations.
  ///
  /// @tparam W     The underlying word type (e.g., uint8_t, uint16_t, uint32_t).
  /// @tparam NBITS The number of bits in the bitset.
  ///
  /// Example usage:
  /// @code
  /// util::bitset<uint8_t, 16> bs;
  /// bs.set(3);
  /// if (bs.test(3)) { ... }
  /// bs.reset();
  /// @endcode
  // --------------------------------------------------------------------
  template <typename W, size_t NBITS>
  class bitset : private bitset_base<W, bits::nr_words<W>(NBITS)>
  {
  public:
    /// Base type for internal storage.
    using Base = bitset_base<W, bits::nr_words<W>(NBITS)>;
    /// Underlying word type.
    using tWord = typename Base::tWord;
    /// This bitset type.
    using This = bitset<W, NBITS>;

    /// Number of bits in the bitset.
    static constexpr size_t kNrBits = NBITS;

  public:
    /// @brief Default constructor. Constructs a bitset with all bits set to zero.
    constexpr bitset() : Base() {}

    /// @brief Constructs a bitset, initializing the first (rightmost, least significant) bits
    ///       to the corresponding bit values of val.
    /// @param val Initial value for the first word.
    constexpr bitset(tWord val) : Base(val) {}

    /// @brief Accesses the bit at position pos.
    /// @param pos Bit position to access.
    /// @return true if the bit is set, false otherwise.
    constexpr bool operator[](size_t pos) const { return (Base::get_word(pos) & bits::bit_mask<tWord>(Base::which_bit(pos))) != static_cast<tWord>(0); }

    /// @brief Returns the value of the bit at the position pos.
    /// @param pos Bit position to test.
    /// @return true if the bit is set, false otherwise.
    bool test(size_t pos) const { return this->operator[](pos); }

    /// @brief Checks if all bits are set to true.
    /// @return true if all bits are set, false otherwise.
    bool all() const noexcept { return Base::template all<NBITS>(); }

    /// @brief Checks if any bits are set to true.
    /// @return true if any bit is set, false otherwise.
    bool any() const noexcept { return Base::any(); }

    /// @brief Checks if none of the bits are set to true.
    /// @return true if all bits are zero, false otherwise.
    bool none() const noexcept { return Base::none(); }

    /// @brief Sets all bits to true.
    /// @return Reference to this bitset.
    This &set() noexcept
    {
      Base::template set<NBITS>();
      return *this;
    }

    /// @brief Sets the bit at position pos to the specified value.
    /// @param pos   Bit position to set.
    /// @param value Value to set (true = 1, false = 0). Default is true.
    /// @return Reference to this bitset.
    This &set(size_t pos, bool value = true)
    {
      Base::set(pos, value);
      return *this;
    }

    /// @brief Sets all bits to false.
    /// @return Reference to this bitset.
    This &reset() noexcept
    {
      Base::reset();
      return *this;
    }

    /// @brief Sets the bit at position pos to false.
    /// @param pos Bit position to reset.
    /// @return Reference to this bitset.
    This &reset(size_t pos)
    {
      Base::reset(pos);
      return *this;
    }
  };
} // namespace util

#endif // BITSET_H_
