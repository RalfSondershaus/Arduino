/**
 * @file String.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Defines util::basic_istream
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

#ifndef UTIL_ISTREAM_H
#define UTIL_ISTREAM_H

#include <Std_Types.h>
#include <Platform_Limits.h>
#include <Util/Locale_Fwd.h>    // util::num_get
#include <Util/Basic_ios.h>     // util::basic_ios, util::ios_base
#include <Util/Iterator.h>      // util::char_traits
#include <Util/Locale_facets.h> // util::num_get
#include <Util/StreamBuf.h>     // util::basic_streambuf

namespace util
{
    // ---------------------------------------------------
    /// Provides support for high-level input operations on character streams.
    ///
    /// Default Traits type is defined in Ios_Fwd.h
    // ---------------------------------------------------
    template <class CharT, class Traits>
    class basic_istream : public basic_ios<CharT, Traits>
    {
    public:
        using char_type = CharT;
        using traits_type = Traits;
        using int_type = typename Traits::int_type;
        using pos_type = typename Traits::pos_type;
        using off_type = typename Traits::off_type;
        using basic_streambuf_type = basic_streambuf<char_type, traits_type>;
        using iter_type = istreambuf_iterator<char_type, traits_type>;
        using num_get_type = util::num_get<char_type, iter_type>;
        using ctype_type = util::ctype<char_type>;
        using basic_ios_type = basic_ios<char_type, traits_type>;

    protected:
        /// Input streams are not copyable
        basic_istream(const basic_istream &rhs) = delete;
        basic_istream &operator=(const basic_istream &rhs) = delete;

        /// Number of extracted characters
        streamsize my_gcount;

    public:
        /// construct
        explicit basic_istream(basic_streambuf_type *sb) : my_gcount(0)
        {
            basic_ios_type::init(sb);
        }

        /// Do not define (virtual) destructors
        /// - Nothing to be deleted since dynamic memory allocation is not used
        ///   and objects are destructed at shut down only (-> never).
        /// - When using virtual destructors, AVR GCC throws 'undefined reference
        ///   to `operator delete(void*, unsigned int)'.

        /// nested class. The constructor prepares the formatted input functions and the
        /// unformatted input functions.
        class sentry
        {
        public:
            basic_istream &mystr;
            bool myok;

            explicit sentry(basic_istream &str, bool noskip = false) : mystr(str), myok(str.ipfx(noskip)) {}
            sentry(const sentry &s) = delete;
            sentry &operator=(const sentry &s) = delete;
            explicit operator bool() const { return myok; }
        };

        /// check if stream is good() and skip white spaces. Returns good().
        /// White spaces are detected by util::use_facet<ctype_type>::isspace
        /// (which might not be fully support locales yet).
        bool ipfx(bool noskip = false)
        {
            if (!this->good())
            {
                this->setstate(ios_base::failbit);
            }
            else
            {
                if (!noskip)
                {
                    int_type nc = this->rdbuf()->sgetc();
                    // Store reference to locale to suppress GCC 13.3 compiler error 
                    // "error: possibly dangling reference to a temporary [-Werror=dangling-reference]".
                    const locale &loc = this->getloc();
                    const ctype_type &facet = util::use_facet<ctype_type>(loc);

                    while ((!traits_type::eq_int_type(traits_type::eof(), nc)) && (facet.isspace(traits_type::to_char_type(nc))))
                    {
                        nc = this->rdbuf()->snextc();
                    }

                    if (traits_type::eq_int_type(traits_type::eof(), nc))
                    {
                        this->setstate(ios_base::eofbit | ios_base::failbit);
                    }
                }
            }
            return this->good();
        }

        // Formatted input

        basic_istream &operator>>(sint16 &value)
        {
            const sentry ok(*this);
            long val;
            ios_base::iostate err = ios_base::goodbit;
            if (ok)
            {
                // Store reference to locale to suppress GCC 13.3 compiler error 
                // "error: possibly dangling reference to a temporary [-Werror=dangling-reference]".
                const locale &loc = this->getloc();
                const num_get_type &facet = util::use_facet<num_get_type>(loc);
                facet.gets(*this, 0, *this, err, val);
                if (val > platform::numeric_limits<sint16>::max_())
                {
                    err |= ios_base::failbit;
                    value = platform::numeric_limits<sint16>::max_();
                }
                else if (val < platform::numeric_limits<sint16>::min_())
                {
                    err |= ios_base::failbit;
                    value = platform::numeric_limits<sint16>::min_();
                }
                else
                {
                    value = static_cast<sint16>(val);
                }
            }

            this->setstate(err);

            return *this;
        }

        basic_istream &operator>>(uint16 &value)
        {
            const sentry ok(*this);
            unsigned long val;
            ios_base::iostate err = ios_base::goodbit;
            if (ok)
            {
                // Store reference to locale to suppress GCC 13.3 compiler error 
                // "error: possibly dangling reference to a temporary [-Werror=dangling-reference]".
                const locale &loc = this->getloc();
                const num_get_type &facet = util::use_facet<num_get_type>(loc);
                facet.getu(*this, 0, *this, err, val);
                if (val > platform::numeric_limits<uint16>::max_())
                {
                    err |= ios_base::failbit;
                    value = platform::numeric_limits<uint16>::max_();
                }
                else
                {
                    value = static_cast<uint16>(val);
                }
            }

            this->setstate(err);

            return *this;
        }

        // Unformatted input

        /// Reads one character.
        /// If available, returns the character and advances the input sequence by one character.
        /// Otherwise, returns Traits::eof() and sets failbit and eofbit.
        int_type get()
        {
            const sentry ok(*this, true);
            int_type ret;
            my_gcount = 0;

            if (ok)
            {
                ret = this->rdbuf()->sgetc();
                if (traits_type::eq_int_type(ret, traits_type::eof()))
                {
                    this->setstate(ios_base::failbit | ios_base::eofbit);
                }
                else
                {
                    this->rdbuf()->sbumpc();
                    my_gcount = 1;
                }
            }
            else
            {
                ret = traits_type::eof();
                this->setstate(ios_base::failbit | ios_base::eofbit);
            }
            return ret;
        }

        /// Reads one character and stores it to ch if available.
        /// Otherwise, leaves ch unmodified and sets failbit and eofbit.
        basic_istream &get(char_type &ch)
        {
            int_type n = get();
            if (!traits_type::eq_int_type(n, traits_type::eof()))
            {
                ch = traits_type::to_char_type(n);
            }
            return *this;
        }

        /// Same as get(s, count, widen('\n')), that is, reads at most std::max(0, count - 1)
        /// characters and stores them into character string pointed to by s until '\n' is found.
        basic_istream &get(char_type *s, streamsize count)
        {
            // Store reference to locale to suppress GCC 13.3 compiler error 
            // "error: possibly dangling reference to a temporary [-Werror=dangling-reference]".
            const locale &loc = this->getloc();
            const ctype_type &facet = util::use_facet<ctype_type>(loc);
            return get(s, count, facet.widen('\n'));
        }

        /// Reads characters and stores them into the successive locations of the character
        /// array whose first element is pointed to by s. Characters are extracted and stored
        /// until any of the following occurs:
        /// - count is less than 1 or count - 1 characters have been stored.
        /// - end of file condition occurs in the input sequence (setstate(eofbit) is called).
        /// - the next available input character c equals delim, as determined by Traits::eq(c, delim).
        ///   This character is not extracted (unlike getline()).
        basic_istream &get(char_type *s, streamsize count, char_type delim)
        {
            const sentry ok(*this, true);
            ios_base::iostate state = ios_base::goodbit;
            my_gcount = 0;

            if (ok)
            {
                int_type n = this->rdbuf()->sgetc();
                while (count > static_cast<streamsize>(1))
                {
                    if (traits_type::eq_int_type(n, traits_type::eof()))
                    {
                        state |= ios_base::eofbit;
                        break;
                    }
                    else if (traits_type::eq(traits_type::to_char_type(n), delim))
                    {
                        break;
                    }
                    else
                    {
                        *s++ = traits_type::to_char_type(n);
                        my_gcount++;
                    }
                    n = this->rdbuf()->snextc();
                    count--;
                }
            }
            else
            {
                state |= ios_base::failbit | ios_base::eofbit;
            }

            // if no characters were extracted, sets failbit
            if (my_gcount == 0)
            {
                state |= ios_base::failbit;
            }

            this->setstate(state);
            *s = char_type(); // terminating 0
            return *this;
        }

        /// Same as getline(s, count, widen('\n')), that is, reads at most std::max(0, count - 1)
        /// characters and stores them into character string pointed to by s until '\n' is found.
        basic_istream &getline(char_type *s, streamsize count)
        {
            // Store reference to locale to suppress GCC 13.3 compiler error 
            // "error: possibly dangling reference to a temporary [-Werror=dangling-reference]".
            const locale &loc = this->getloc();
            const ctype_type &facet = util::use_facet<ctype_type>(loc);
            return get(s, count, facet.widen('\n'));
        }

        /// Reads characters and stores them into the successive locations of the character
        /// array whose first element is pointed to by s. Characters are extracted and stored
        /// until any of the following occurs:
        /// - end of file condition occurs in the input sequence.
        /// - the next available character c is the delimiter, as determined by Traits::eq(c, delim).
        ///   The delimiter is extracted (unlike basic_istream::get()) and counted towards gcount(), but is not stored.
        /// - count is non-positive, or count - 1 characters have been extracted (setstate(failbit) is called in this case).
        basic_istream &getline(char_type *s, streamsize count, char_type delim)
        {
            const sentry ok(*this, true);
            ios_base::iostate state = ios_base::goodbit;
            my_gcount = 0;

            if (ok)
            {
                int_type n = this->rdbuf()->sgetc();
                while (count > static_cast<streamsize>(1))
                {
                    if (traits_type::eq_int_type(n, traits_type::eof()))
                    {
                        state |= ios_base::eofbit;
                        break;
                    }
                    else if (traits_type::eq(traits_type::to_char_type(n), delim))
                    {
                        // The delimiter is extracted (unlike basic_istream::get()) and counted
                        // towards gcount(), but is not stored
                        my_gcount++;
                        n = this->rdbuf()->snextc();
                        break;
                    }
                    else
                    {
                        *s++ = traits_type::to_char_type(n);
                        my_gcount++;
                        n = this->rdbuf()->snextc();
                    }
                    count--;
                }
            }
            else
            {
                state |= ios_base::failbit | ios_base::eofbit;
            }

            // if no characters were extracted, sets failbit
            if (my_gcount == 0)
            {
                state |= ios_base::failbit;
            }

            this->setstate(state);
            *s = char_type(); // terminating 0
            return *this;
        }

        /// Returns the number of characters extracted by the last unformatted input operation,
        /// or the maximum representable value of std::streamsize if the number is not representable.
        util::streamsize gcount() const { return my_gcount; }

#if 0
    // not implemented yet

    /// reads the next character without extracting it
    int_type peek();

    /// unextracts a character. Makes the most recently extracted character available again.
    basic_istringstream& unget();

    /// Puts the character ch back to the input stream so the next extracted character will be ch
    basic_istringstream& putback(char_type ch);

    /// Extracts and discards characters from the input stream until and including delim.
    basic_istringstream& ignore(std::streamsize count = 1, int_type delim = Traits::eof());

    /// extracts blocks of characters
    basic_istringstream& read(char_type* s, util::streamsize count);

    /// extracts already available blocks of characters
    util::streamsize readsome(char_type* s, util::streamsize count);
#endif

        // Positioning

        /// Returns input position indicator of the current associated streambuf object.
        pos_type tellg()
        {
            pos_type p;
            const sentry ok(*this, true);
            if (ok)
            {
                p = this->rdbuf()->pubseekoff(0, ios_base::cur, ios_base::in);
            }
            else
            {
                p = static_cast<pos_type>(-1);
            }
            return p;
        }
#if 0
    /// Sets input position indicator of the current associated streambuf object.
    basic_istringstream& seekg(pos_type pos);

    /// Sets input position indicator of the current associated streambuf object.
    basic_istringstream& seekg(off_type off, std::ios_base::seekdir dir);
#endif
    };

    // --------------------------------------------------------------------------
    // Character input operations

    /// Behaves as a FormattedInputFunction.
    /// After constructing and checking the sentry object, which may skip leading whitespace,
    /// extracts a character and stores it to ch. If no character is available, sets failbit
    /// (in addition to eofbit that is set as required of a FormattedInputFunction).
    template <class CharT, class Traits>
    basic_istream<CharT, Traits> &operator>>(basic_istream<CharT, Traits> &st, CharT &ch)
    {
        using basic_istream_type = basic_istream<CharT, Traits>;
        using int_type = typename basic_istream_type::int_type;
        using traits_type = typename basic_istream_type::traits_type;

        const typename basic_istream_type::sentry ok(st);
        if (ok)
        {
            // Read one character and advance the input sequence by one character afterwards
            int_type n = st.rdbuf()->sbumpc();
            if (traits_type::eq_int_type(n, traits_type::eof()))
            {
                st.setstate(ios_base::eofbit);
            }
            else
            {
                ch = traits_type::to_char_type(n);
            }
        }
        else
        {
            st.setstate(ios_base::failbit | ios_base::eofbit);
        }

        return st;
    }
    // --------------------------------------------------------------------------
    template <class Traits>
    basic_istream<char, Traits> &operator>>(basic_istream<char, Traits> &st, signed char &ch)
    {
        return (st >> reinterpret_cast<char &>(ch));
    }
    // --------------------------------------------------------------------------
    template <class Traits>
    basic_istream<char, Traits> &operator>>(basic_istream<char, Traits> &st, unsigned char &ch)
    {
        return (st >> reinterpret_cast<char &>(ch));
    }

    // --------------------------------------------------------------------------
    /// Behaves as a FormattedInputFunction. After constructing and checking the sentry object,
    /// which may skip leading whitespace, extracts successive characters and stores them at
    /// successive locations of a character array whose first element is pointed to by s.
    /// The extraction stops if any of the following conditions is met:
    /// - A whitespace character (as determined by the ctype<CharT> facet) is found.
    ///   The whitespace character is not extracted.
    /// - End of file occurs in the input sequence (this also sets eofbit).
    /// In either case, an additional null character value CharT() is stored at the end of the
    /// output. If no characters were extracted, sets failbit (the null character is still written,
    /// to the first position in the output). Finally, calls st.width(0) to cancel the effects
    /// of std::setw, if any.
    template <class CharT, class Traits>
    basic_istream<CharT, Traits> &operator>>(basic_istream<CharT, Traits> &st, CharT *s)
    {
        using char_type = CharT;
        using basic_istream_type = basic_istream<CharT, Traits>;
        using int_type = typename basic_istream_type::int_type;
        using traits_type = typename basic_istream_type::traits_type;
        using ctype_type = util::ctype<char_type>;

        const typename basic_istream_type::sentry ok(st);

        ios_base::iostate state = ios_base::goodbit;
        util::streamsize gcount = 0;
        if (ok)
        {
            util::streamsize width = st.width();

            if (width <= 0)
            {
                width = platform::numeric_limits<util::streamsize>::max_();
            }

            int_type n = st.rdbuf()->sgetc();

            // Store reference to locale to suppress GCC 13.3 compiler error 
            // "error: possibly dangling reference to a temporary [-Werror=dangling-reference]".
            const locale &st_loc = st.getloc();
            const ctype_type &facet = util::use_facet<ctype_type>(st_loc);

            while (!traits_type::eq_int_type(n, traits_type::eof()) && !facet.isspace(traits_type::to_char_type(n)) && (gcount < width - 1))
            {
                *s++ = traits_type::to_char_type(n);
                gcount++;
                n = st.rdbuf()->snextc();
            }

            if (traits_type::eq_int_type(n, traits_type::eof()))
            {
                state |= ios_base::eofbit;
            }
        }
        else
        {
            state |= ios_base::failbit | ios_base::eofbit;
        }

        // if no characters were extracted, sets failbit
        if (gcount == 0)
        {
            state |= ios_base::failbit;
        }

        st.setstate(state);
        *s = char_type(); // terminating 0
        st.width(0);      // cancel the effects of std::setw
        return st;
    }
    template <class Traits>
    basic_istream<char, Traits> &operator>>(basic_istream<char, Traits> &st, signed char *s);
    template <class Traits>
    basic_istream<char, Traits> &operator>>(basic_istream<char, Traits> &st, unsigned char *s);

    // --------------------------------------------------------------------------
    // IO manipulators.
    struct Setw
    {
        int w;
    };

    inline Setw setw(int n) { return {n}; }

    /// Set width parameter of the stream.
    template <typename CharT, typename Traits>
    inline basic_istream<CharT, Traits> &operator>>(basic_istream<CharT, Traits> &st, Setw sw)
    {
        st.width(sw.w);
        return st;
    }

} // namespace util

#endif // UTIL_ISTREAM_H