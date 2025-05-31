/**
 * @file test.cpp
 *
 * @author Ralf Sondershaus
 *
 * Google Test for Gen/Util/Sstream.h
 */

#include <stdint.h>

#include <unity_adapt.h>
#include <Util/Sstream.h>

#define CFG_OFF    0
#define CFG_ON     1

// CFG_OFF = don't run tests with std::; typically on the target (such as Arduino Mega)
// CFG_ON = run tests with std::; typically on the host (such as Windows)
#ifdef ARDUINO
#define CFG_TEST_WITH_STD    CFG_OFF
#else
#define CFG_TEST_WITH_STD    CFG_ON
#endif

#if CFG_TEST_WITH_STD == CFG_ON
#include <sstream>
#include <iostream>
#include <iomanip>
#endif


#if CFG_TEST_WITH_STD == CFG_ON
template<> void EXPECT_EQ<std::streampos,std::streampos>(std::streampos actual, std::streampos expected) { EXPECT_EQ<std::streamoff,std::streamoff>(expected, actual); }
template<> void EXPECT_EQ<std::string,std::string>(std::string actual, std::string expected) { TEST_ASSERT_EQUAL_STRING(expected.c_str(), actual.c_str()); }
#endif

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Construct_empty)
{
  using istringstream_t = util::basic_istringstream<16, char>;
  using string_t = istringstream_t::string_type;

  istringstream_t stream;
  
  EXPECT_EQ(stream.str().compare(string_t{ "" }), 0);
  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss;
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
#endif
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Construct_with_string)
{
  using istringstream_t = util::basic_istringstream<16, char>;
  using string_t = istringstream_t::string_type;

  istringstream_t stream("Hallo");

  EXPECT_EQ(stream.str().compare(string_t{ "Hallo" }), 0);
  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss("Hallo");
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
#endif
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Construct_with_string_that_is_too_long)
{
  using istringstream_t = util::basic_istringstream<8, char>;
  using string_t = istringstream_t::string_type;

  istringstream_t stream("1234567890");

  EXPECT_EQ(stream.str().compare(string_t{ "12345678" }), 0);
  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Unformatted_input_get_int_type_void)
{
  using istringstream_t = util::basic_istringstream<64, char>;
  using traits_t = istringstream_t::traits_type;

  istringstream_t stream("abcde");

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  EXPECT_EQ(traits_t::to_char_type(stream.get()), 'a');
  EXPECT_EQ(stream.tellg(), util::streampos{ 1 });
  EXPECT_EQ(traits_t::to_char_type(stream.get()), 'b');
  EXPECT_EQ(stream.tellg(), util::streampos{ 2 });
  EXPECT_EQ(traits_t::to_char_type(stream.get()), 'c');
  EXPECT_EQ(stream.tellg(), util::streampos{ 3 });
  EXPECT_EQ(traits_t::to_char_type(stream.get()), 'd');
  EXPECT_EQ(stream.tellg(), util::streampos{ 4 });
  EXPECT_EQ(traits_t::to_char_type(stream.get()), 'e');
  EXPECT_EQ(stream.tellg(), util::streampos{ 5 });
  EXPECT_EQ(stream.get(), -1);

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss("abcde");
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  EXPECT_EQ(std::istringstream::traits_type::to_char_type(ss.get()), 'a');
  EXPECT_EQ(ss.tellg(), std::streampos{ 1 });
  EXPECT_EQ(std::istringstream::traits_type::to_char_type(ss.get()), 'b');
  EXPECT_EQ(ss.tellg(), std::streampos{ 2 });
  EXPECT_EQ(std::istringstream::traits_type::to_char_type(ss.get()), 'c');
  EXPECT_EQ(ss.tellg(), std::streampos{ 3 });
  EXPECT_EQ(std::istringstream::traits_type::to_char_type(ss.get()), 'd');
  EXPECT_EQ(ss.tellg(), std::streampos{ 4 });
  EXPECT_EQ(std::istringstream::traits_type::to_char_type(ss.get()), 'e');
  EXPECT_EQ(ss.tellg(), std::streampos{ 5 });
  EXPECT_EQ(ss.get(), -1);
#endif
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Unformatted_input_get_char)
{
  using istringstream_t = util::basic_istringstream<64, char>;

  istringstream_t stream("abcde");
  char c;

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream.get(c);
  EXPECT_EQ(c, 'a');
  EXPECT_EQ(stream.tellg(), util::streampos{ 1 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 1 });
  stream.get(c);
  EXPECT_EQ(c, 'b');
  EXPECT_EQ(stream.tellg(), util::streampos{ 2 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 1 });
  stream.get(c);
  EXPECT_EQ(c, 'c');
  EXPECT_EQ(stream.tellg(), util::streampos{ 3 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 1 });
  stream.get(c);
  EXPECT_EQ(c, 'd');
  EXPECT_EQ(stream.tellg(), util::streampos{ 4 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 1 });
  stream.get(c);
  EXPECT_EQ(c, 'e');
  EXPECT_EQ(stream.tellg(), util::streampos{ 5 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 1 });
  stream.get(c);
  EXPECT_EQ(c, 'e');
  EXPECT_EQ(stream.tellg(), util::streampos{ -1 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.get(), -1);

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss("abcde");
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss.get(c);
  EXPECT_EQ(c, 'a');
  EXPECT_EQ(ss.tellg(), std::streampos{ 1 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 1 });
  ss.get(c);
  EXPECT_EQ(c, 'b');
  EXPECT_EQ(ss.tellg(), std::streampos{ 2 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 1 });
  ss.get(c);
  EXPECT_EQ(c, 'c');
  EXPECT_EQ(ss.tellg(), std::streampos{ 3 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 1 });
  ss.get(c);
  EXPECT_EQ(c, 'd');
  EXPECT_EQ(ss.tellg(), std::streampos{ 4 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 1 });
  ss.get(c);
  EXPECT_EQ(c, 'e');
  EXPECT_EQ(ss.tellg(), std::streampos{ 5 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 1 });
  ss.get(c);
  EXPECT_EQ(c, 'e');
  EXPECT_EQ(ss.tellg(), std::streampos{ -1 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.get(), -1);
#endif
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Unformatted_input_get_char_pointer)
{
  using istringstream_t = util::basic_istringstream<64, char>;
  using string_t = istringstream_t::string_type;
  
  istringstream_t stream("abcde");
  constexpr size_t kNrChars = 4; // kNrChars-1 characters are copied (last array element is terminating 0)
  char s[kNrChars];

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream.get(s, kNrChars);
  EXPECT_EQ(string_t{s}.compare(string_t{"abc"}), 0);
  EXPECT_EQ(stream.gcount(), util::streamsize{ 3 });

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss("abcde");
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss.get(s, kNrChars);
  EXPECT_EQ(std::string{ s }, std::string{ "abc" });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 3 });
#endif
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Unformatted_input_get_char_pointer_available_too_small)
{
  using istringstream_t = util::basic_istringstream<64, char>;
  using string_t = istringstream_t::string_type;

  istringstream_t stream("abcde");
  constexpr size_t kNrChars = 10; 
  char s[kNrChars];

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream.get(s, kNrChars);
  EXPECT_EQ(string_t{ s }.compare(string_t{ "abcde" }), 0);
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), false);
  EXPECT_EQ(stream.good(), false);
  EXPECT_EQ(!stream, false);
  EXPECT_EQ((bool)stream, true);
  EXPECT_EQ(stream.gcount(), util::streamsize{ 5 });

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss("abcde");
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss.get(s, kNrChars);
  EXPECT_EQ(std::string{ s }, std::string{ "abcde" });
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), false);
  EXPECT_EQ(ss.good(), false);
  EXPECT_EQ(!ss, false);
  EXPECT_EQ((bool)ss, true);
  EXPECT_EQ(ss.gcount(), std::streamsize{ 5 });
#endif
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Unformatted_input_get_char_pointer_empty_stream)
{
  using istringstream_t = util::basic_istringstream<64, char>;
  using string_t = istringstream_t::string_type;

  istringstream_t stream("");
  constexpr size_t kNrChars = 10;
  char s[kNrChars];

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream.get(s, kNrChars);
  EXPECT_EQ(string_t{ s }.compare(string_t{ "" }), 0);
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), true);
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss("");
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss.get(s, kNrChars);
  EXPECT_EQ(std::string{ s }, std::string{ "" });
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), true);
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
#endif
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Unformatted_input_get_char_pointer_get_buffer_smaller_than_read_request)
{
  using istringstream_t = util::basic_istringstream<8, char>;
  using string_t = istringstream_t::string_type;

  istringstream_t stream("12345678");
  constexpr size_t kNrChars = 10;
  char s[kNrChars];

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream.get(s, kNrChars);
  EXPECT_EQ(string_t{ s }.compare(string_t{ "12345678" }), 0);
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), false);
  EXPECT_EQ(stream.gcount(), util::streamsize{ 8 });
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Unformatted_input_getline_char_pointer_delimiter_spaces)
{
  using istringstream_t = util::basic_istringstream<64, char>;
  using string_t = istringstream_t::string_type;
  
  istringstream_t stream("SET CAL SIGNAL 0 ASPECT 1 ASPECT 11000");
  constexpr size_t kNrChars = 64; // kNrChars-1 characters are copied (last array element is terminating 0)
  constexpr char delimSpace = ' ';
  char s[kNrChars];
  uint16 un;

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  // SET
  stream.getline(s, kNrChars, delimSpace);
  EXPECT_EQ(string_t{s}.compare(string_t{"SET"}), 0);
  EXPECT_EQ(stream.gcount(), util::streamsize{ 4 });
  // CAL
  stream.getline(s, kNrChars, delimSpace);
  EXPECT_EQ(string_t{s}.compare(string_t{"CAL"}), 0);
  EXPECT_EQ(stream.gcount(), util::streamsize{ 4 });
  // SIGNAL 0 <space>
  stream.getline(s, kNrChars, delimSpace);
  EXPECT_EQ(string_t{s}.compare(string_t{"SIGNAL"}), 0);
  EXPECT_EQ(stream.gcount(), util::streamsize{ 7 });
  stream >> un;
  EXPECT_EQ(un, uint16{ 0U });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 7 });
  EXPECT_EQ(stream.eof(), false);
  EXPECT_EQ(stream.fail(), false);
  stream.getline(s, kNrChars, delimSpace);
  EXPECT_EQ(string_t{s}.compare(string_t{""}), 0);
  EXPECT_EQ(stream.gcount(), util::streamsize{ 1 });
  // ASPECT 1 <space>
  stream.getline(s, kNrChars, delimSpace);
  EXPECT_EQ(string_t{s}.compare(string_t{"ASPECT"}), 0);
  EXPECT_EQ(stream.gcount(), util::streamsize{ 7 });
  stream >> un;
  EXPECT_EQ(un, uint16{ 1U });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 7 });
  EXPECT_EQ(stream.eof(), false);
  EXPECT_EQ(stream.fail(), false);
  stream.getline(s, kNrChars, delimSpace);
  EXPECT_EQ(string_t{s}.compare(string_t{""}), 0);
  EXPECT_EQ(stream.gcount(), util::streamsize{ 1 });
  // ASPECT 11000
  stream.getline(s, kNrChars, delimSpace);
  EXPECT_EQ(string_t{s}.compare(string_t{"ASPECT"}), 0);
  EXPECT_EQ(stream.gcount(), util::streamsize{ 7 });
  stream.getline(s, kNrChars, delimSpace);
  EXPECT_EQ(string_t{s}.compare(string_t{"11000"}), 0);
  EXPECT_EQ(stream.gcount(), util::streamsize{ 5 });
  EXPECT_EQ(stream.eof(), true);

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss("SET CAL SIGNAL 0 ASPECT 1 ASPECT 11000");
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  // SET
  ss.getline(s, kNrChars, delimSpace);
  EXPECT_EQ(std::string{ s }, std::string{ "SET" });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 4 });
  // CAL
  ss.getline(s, kNrChars, delimSpace);
  EXPECT_EQ(std::string{s}.compare(std::string{"CAL"}), 0);
  EXPECT_EQ(ss.gcount(), std::streamsize{ 4 });
  // SIGNAL 0 <space>
  ss.getline(s, kNrChars, delimSpace);
  EXPECT_EQ(std::string{s}.compare(std::string{"SIGNAL"}), 0);
  EXPECT_EQ(ss.gcount(), std::streamsize{ 7 });
  ss >> un;
  EXPECT_EQ(un, uint16{ 0U });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 7 });
  EXPECT_EQ(ss.eof(), false);
  EXPECT_EQ(ss.fail(), false);
  ss.getline(s, kNrChars, delimSpace);
  EXPECT_EQ(std::string{s}.compare(std::string{""}), 0);
  EXPECT_EQ(ss.gcount(), std::streamsize{ 1 });
  // ASPECT 1 <space>
  ss.getline(s, kNrChars, delimSpace);
  EXPECT_EQ(std::string{s}.compare(std::string{"ASPECT"}), 0);
  EXPECT_EQ(ss.gcount(), std::streamsize{ 7 });
  ss >> un;
  EXPECT_EQ(un, uint16{ 1U });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 7 });
  EXPECT_EQ(ss.eof(), false);
  EXPECT_EQ(ss.fail(), false);
  ss.getline(s, kNrChars, delimSpace);
  EXPECT_EQ(std::string{s}.compare(std::string{""}), 0);
  EXPECT_EQ(ss.gcount(), std::streamsize{ 1 });
  // ASPECT 11000
  ss.getline(s, kNrChars, delimSpace);
  EXPECT_EQ(std::string{s}.compare(std::string{"ASPECT"}), 0);
  EXPECT_EQ(ss.gcount(), std::streamsize{ 7 });
  ss.getline(s, kNrChars, delimSpace);
  EXPECT_EQ(std::string{s}.compare(std::string{"11000"}), 0);
  EXPECT_EQ(ss.gcount(), std::streamsize{ 5 });
  EXPECT_EQ(ss.eof(), true);
#endif
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Formatted_input_uint16)
{
  using istringstream_t = util::basic_istringstream<16, char>;

  istringstream_t stream("12345");
  uint16 un;

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream >> un;
  EXPECT_EQ(un, uint16{ 12345 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), false);

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss("12345");
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss >> un;
  EXPECT_EQ(un, uint16{ 12345 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), false);
#endif
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Formatted_input_uint16_hex)
{
  using istringstream_t = util::basic_istringstream<16, char>;

  istringstream_t stream("0xA000");
  uint16 un;

  stream.unsetf(util::ios_base::dec);
  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream >> un;
  EXPECT_EQ(un, uint16{ 0xA000 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), false);

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss("0xA000");
  ss.unsetf(std::ios_base::dec);
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss >> un;
  EXPECT_EQ(un, uint16{ 0xA000 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), false);
#endif
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Formatted_input_uint16_0)
{
  using istringstream_t = util::basic_istringstream<16, char>;

  istringstream_t stream("0");
  uint16 un;

  stream.unsetf(util::ios_base::dec);
  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream >> un;
  EXPECT_EQ(un, uint16{ 0 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), false);

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss("0");
  ss.unsetf(std::ios_base::dec);
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss >> un;
  EXPECT_EQ(un, uint16{ 0 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), false);
#endif
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Formatted_input_uint16_0_space)
{
  using istringstream_t = util::basic_istringstream<16, char>;

  istringstream_t stream("0 ");
  uint16 un;

  stream.unsetf(util::ios_base::dec);
  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream >> un;
  EXPECT_EQ(un, uint16{ 0 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), false);
  EXPECT_EQ(stream.fail(), false);

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss("0 ");
  ss.unsetf(std::ios_base::dec);
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss >> un;
  EXPECT_EQ(un, uint16{ 0 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), false);
  EXPECT_EQ(ss.fail(), false);
#endif
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Formatted_input_uint16_2x)
{
  using istringstream_t = util::basic_istringstream<16, char>;

  istringstream_t stream("12345 678");
  uint16 un;

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream >> un;
  EXPECT_EQ(un, uint16{ 12345 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), false);
  EXPECT_EQ(stream.fail(), false);
  stream >> un;
  EXPECT_EQ(un, uint16{ 678 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), false);

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss("12345 678");
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss >> un;
  EXPECT_EQ(un, uint16{ 12345 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), false);
  EXPECT_EQ(ss.fail(), false);
  ss >> un;
  EXPECT_EQ(un, uint16{ 678 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), false);
#endif
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Formatted_input_uint16_2x_char)
{
  using istringstream_t = util::basic_istringstream<16, char>;

  istringstream_t stream("12345 a 678");
  uint16 un;

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream >> un;
  EXPECT_EQ(un, uint16{ 12345 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), false);
  EXPECT_EQ(stream.fail(), false);
  stream >> un;
  EXPECT_EQ(un, uint16{ 0 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), false);
  EXPECT_EQ(stream.fail(), true);
  stream >> un;
  EXPECT_EQ(un, uint16{ 0 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), false);
  EXPECT_EQ(stream.fail(), true);

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss("12345 a 678");
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss >> un;
  EXPECT_EQ(un, uint16{ 12345 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), false);
  EXPECT_EQ(ss.fail(), false);
  ss >> un;
  EXPECT_EQ(un, uint16{ 0 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), false);
  EXPECT_EQ(ss.fail(), true);
  ss >> un;
  EXPECT_EQ(un, uint16{ 0 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), false);
  EXPECT_EQ(ss.fail(), true);
#endif
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Formatted_input_sint16_pos)
{
  using istringstream_t = util::basic_istringstream<16, char>;

  istringstream_t stream("12345");
  sint16 n;

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream >> n;
  EXPECT_EQ(n, sint16{ 12345 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), false);

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss("12345");
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss >> n;
  EXPECT_EQ(n, sint16{ 12345 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), false);
#endif
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Formatted_input_sint16_0)
{
  using istringstream_t = util::basic_istringstream<16, char>;

  istringstream_t stream("0");
  sint16 n;

  stream.unsetf(util::ios_base::dec);
  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream >> n;
  EXPECT_EQ(n, sint16{ 0 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), false);

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss("0");
  ss.unsetf(std::ios_base::dec);
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss >> n;
  EXPECT_EQ(n, sint16{ 0 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), false);
#endif
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Formatted_input_sint16_0_space)
{
  using istringstream_t = util::basic_istringstream<16, char>;

  istringstream_t stream("0 ");
  sint16 n;

  stream.unsetf(util::ios_base::dec);
  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream >> n;
  EXPECT_EQ(n, sint16{ 0 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), false);
  EXPECT_EQ(stream.fail(), false);

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss("0 ");
  ss.unsetf(std::ios_base::dec);
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss >> n;
  EXPECT_EQ(n, sint16{ 0 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), false);
  EXPECT_EQ(ss.fail(), false);
#endif
}


// -------------------------------------------------------------------------
TEST(Ut_Sstream, Formatted_input_sint16_neg)
{
  using istringstream_t = util::basic_istringstream<16, char>;

  istringstream_t stream("-12345");
  sint16 n;

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream >> n;
  EXPECT_EQ(n, sint16{ -12345 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), false);

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss("-12345");
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss >> n;
  EXPECT_EQ(n, sint16{ -12345 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), false);
#endif
}

// -------------------------------------------------------------------------
// Extract a sint16 from a stream (which is a short).
// 
// basic_istream& operator>>( short& value ):
//    Extracts a long value lval by calling std::num_get::get(). After that:
//    If lval < std::numeric_limits<short>::min(), sets failbit and stores std::numeric_limits<short>::min() to val.
//    Otherwise, if std::numeric_limits<short>::max() < lval, sets failbit and stores std::numeric_limits<short>::max() to val.
//    Otherwise, stores static_cast<short>(lval) to val.
//
// 0x0A00 = 2,560: expected value is 2,560. Expected that failbit is not set.
TEST(Ut_Sstream, Formatted_input_sint16_pos_hex)
{
  using istringstream_t = util::basic_istringstream<16, char>;

  istringstream_t stream("0x0A00");
  sint16 n;

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream.unsetf(util::ios_base::dec);
  stream >> n;
  EXPECT_EQ(n, sint16{ 2560 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), false);

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss("0x0A00");
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss.unsetf(std::ios_base::dec);
  ss >> n;
  EXPECT_EQ(n, sint16{ 2560 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), false);
#endif
}

// -------------------------------------------------------------------------
// Extract a sint16 from a stream (which is a short).
// 
// basic_istream& operator>>( short& value ):
//    Extracts a long value lval by calling std::num_get::get(). After that:
//    If lval < std::numeric_limits<short>::min(), sets failbit and stores std::numeric_limits<short>::min() to val.
//    Otherwise, if std::numeric_limits<short>::max() < lval, sets failbit and stores std::numeric_limits<short>::max() to val.
//    Otherwise, stores static_cast<short>(lval) to val.
//
// 0xA000 = 40,960 is greater than 32,767 (sint16_max): expected value is 32,767. Expected that failbit is set.
TEST(Ut_Sstream, Formatted_input_sint16_pos_hex_fail)
{
  using istringstream_t = util::basic_istringstream<16, char>;

  istringstream_t stream("0xA000");
  sint16 n;

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream.unsetf(util::ios_base::dec);
  stream >> n;
  EXPECT_EQ(n, sint16{ 32767 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), true);

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss("0xA000");
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss.unsetf(std::ios_base::dec);
  ss >> n;
  EXPECT_EQ(n, sint16{ 32767 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), true);
#endif
}

// -------------------------------------------------------------------------
// Extract a sint16 from a stream (which is a short).
// 
// basic_istream& operator>>( short& value ):
//    Extracts a long value lval by calling std::num_get::get(). After that:
//    If lval < std::numeric_limits<short>::min(), sets failbit and stores std::numeric_limits<short>::min() to val.
//    Otherwise, if std::numeric_limits<short>::max() < lval, sets failbit and stores std::numeric_limits<short>::max() to val.
//    Otherwise, stores static_cast<short>(lval) to val.
//
// -0x0A00 = -2,560: expected value is -2,560. Expected that failbit is not set.
TEST(Ut_Sstream, Formatted_input_sint16_neg_hex)
{
  using istringstream_t = util::basic_istringstream<16, char>;

  istringstream_t stream("-0x0A00");
  sint16 n;

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream.unsetf(util::ios_base::dec);
  stream >> n;
  EXPECT_EQ(n, sint16{ -2560 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), false);

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss("-0x0A00");
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss.unsetf(std::ios_base::dec);
  ss >> n;
  EXPECT_EQ(n, sint16{ -2560 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), false);
#endif
}

// -------------------------------------------------------------------------
// Extract a sint16 from a stream (which is a short).
// 
// basic_istream& operator>>( short& value ):
//    Extracts a long value lval by calling std::num_get::get(). After that:
//    If lval < std::numeric_limits<short>::min(), sets failbit and stores std::numeric_limits<short>::min() to val.
//    Otherwise, if std::numeric_limits<short>::max() < lval, sets failbit and stores std::numeric_limits<short>::max() to val.
//    Otherwise, stores static_cast<short>(lval) to val.
//
// -0xA000 = -40,960 is less than -32,768 (sint16_min): expected value is -32,768. Expected that failbit is set.
TEST(Ut_Sstream, Formatted_input_sint16_neg_hex_fail)
{
  using istringstream_t = util::basic_istringstream<16, char>;

  istringstream_t stream("-0xA000");
  sint16 n;

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream.unsetf(util::ios_base::dec);
  stream >> n;
  EXPECT_EQ(n, sint16{ -32768 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), true);

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss("-0xA000");
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss.unsetf(std::ios_base::dec);
  ss >> n;
  EXPECT_EQ(n, sint16{ -32768 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), true);
#endif
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Formatted_input_sint16_pos_big)
{
  using istringstream_t = util::basic_istringstream<16, char>;

  istringstream_t stream("32767 32768");
  sint16 n;

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream >> n;
  EXPECT_EQ(n, sint16{ 32767 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), false);
  EXPECT_EQ(stream.fail(), false);
  stream >> n;
  EXPECT_EQ(n, sint16{ 32767 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), true);

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss("32767 32768");
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss >> n;
  EXPECT_EQ(n, sint16{ 32767 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), false);
  EXPECT_EQ(ss.fail(), false);
  ss >> n;
  EXPECT_EQ(n, sint16{ 32767 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), true);
#endif
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Formatted_input_sint16_neg_big)
{
  using istringstream_t = util::basic_istringstream<16, char>;

  istringstream_t stream("-32768 -32769");
  sint16 n;

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream >> n;
  EXPECT_EQ(n, sint16{ -32768 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), false);
  EXPECT_EQ(stream.fail(), false);
  stream >> n;
  EXPECT_EQ(n, sint16{ -32768 });
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), true);

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss("-32768 -32769");
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss >> n;
  EXPECT_EQ(n, sint16{ -32768 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), false);
  EXPECT_EQ(ss.fail(), false);
  ss >> n;
  EXPECT_EQ(n, sint16{ -32768 });
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), true);
#endif
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Formatted_input_char)
{
  using istringstream_t = util::basic_istringstream<16, char>;

  istringstream_t stream(" ab");
  char ch;

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream >> ch;
  EXPECT_EQ(ch, 'a');
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), false);
  EXPECT_EQ(stream.fail(), false);
  stream >> ch;
  EXPECT_EQ(ch, 'b');
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), false);
  EXPECT_EQ(stream.fail(), false);
  stream >> ch;
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), true);

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss(" ab");
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss >> ch;
  EXPECT_EQ(ch, 'a');
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), false);
  EXPECT_EQ(ss.fail(), false);
  ss >> ch;
  EXPECT_EQ(ch, 'b');
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), false);
  EXPECT_EQ(ss.fail(), false);
  ss >> ch;
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), true);
#endif
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Formatted_input_char_spaces)
{
  using istringstream_t = util::basic_istringstream<16, char>;

  istringstream_t stream(" a  b  ");
  char ch;

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream >> ch;
  EXPECT_EQ(ch, 'a');
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), false);
  EXPECT_EQ(stream.fail(), false);
  stream >> ch;
  EXPECT_EQ(ch, 'b');
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), false);
  EXPECT_EQ(stream.fail(), false);
  stream >> ch;
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), true);

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss(" a  b  ");
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss >> ch;
  EXPECT_EQ(ch, 'a');
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), false);
  EXPECT_EQ(ss.fail(), false);
  ss >> ch;
  EXPECT_EQ(ch, 'b');
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), false);
  EXPECT_EQ(ss.fail(), false);
  ss >> ch;
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), true);
#endif
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Formatted_input_char_array)
{
  using istringstream_t = util::basic_istringstream<16, char>;

  istringstream_t stream(" ab");
  char arr[4];

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream >> arr;
  EXPECT_EQ(strcmp(arr, "ab"), 0);
  //EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), false);
  stream >> arr;
  //EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), true);

#if CFG_TEST_WITH_STD == CFG_ON
  std::cout << sizeof(util::streamsize) << std::endl;
  std::istringstream ss(" ab");
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss >> arr;
  EXPECT_EQ(strcmp(arr, "ab"), 0);
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), false);
  ss >> arr;
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), true);
#endif
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Formatted_input_char_array_spaces)
{
  using istringstream_t = util::basic_istringstream<16, char>;

  istringstream_t stream(" ab  cd  ");
  char arr[4];

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream >> arr;
  EXPECT_EQ(strcmp(arr, "ab"), 0);
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), false);
  EXPECT_EQ(stream.fail(), false);
  stream >> arr;
  EXPECT_EQ(strcmp(arr, "cd"), 0);
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), false);
  EXPECT_EQ(stream.fail(), false);
  stream >> arr;
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), true);

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss(" ab  cd  ");
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss >> arr;
  EXPECT_EQ(strcmp(arr, "ab"), 0);
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), false);
  EXPECT_EQ(ss.fail(), false);
  ss >> arr;
  EXPECT_EQ(strcmp(arr, "cd"), 0);
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), false);
  EXPECT_EQ(ss.fail(), false);
  ss >> arr;
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), true);
#endif
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Formatted_input_char_array_spaces_width)
{
  using istringstream_t = util::basic_istringstream<16, char>;

  istringstream_t stream(" abcd  ef  ");
  char arr[4];

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream >> util::setw(3) >> arr;
  EXPECT_EQ(strcmp(arr, "ab"), 0);
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), false);
  EXPECT_EQ(stream.fail(), false);
  EXPECT_EQ(stream.width(), util::streamsize{ 0 });
  stream >> util::setw(3) >> arr;
  EXPECT_EQ(strcmp(arr, "cd"), 0);
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), false);
  EXPECT_EQ(stream.fail(), false);
  stream >> arr;
  EXPECT_EQ(strcmp(arr, "ef"), 0);
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), false);
  EXPECT_EQ(stream.fail(), false);
  stream >> arr;
  EXPECT_EQ(stream.gcount(), util::streamsize{ 0 });
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), true);

#if CFG_TEST_WITH_STD == CFG_ON
  std::istringstream ss(" abcd  ef  ");
  EXPECT_EQ(ss.tellg(), std::streampos{ 0 });
  ss >> std::setw(3) >> arr;
  EXPECT_EQ(strcmp(arr, "ab"), 0);
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), false);
  EXPECT_EQ(ss.fail(), false);
  ss >> std::setw(3) >> arr;
  EXPECT_EQ(strcmp(arr, "cd"), 0);
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), false);
  EXPECT_EQ(ss.fail(), false);
  ss >> arr;
  EXPECT_EQ(strcmp(arr, "ef"), 0);
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), false);
  EXPECT_EQ(ss.fail(), false);
  ss >> arr;
  EXPECT_EQ(ss.gcount(), std::streamsize{ 0 });
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), true);
#endif
}

void setUp(void)
{
}

void tearDown(void)
{
}

void test_setup(void)
{
}

bool test_loop(void)
{
  UNITY_BEGIN();

  RUN_TEST(Construct_empty);
  RUN_TEST(Construct_with_string);
  RUN_TEST(Construct_with_string_that_is_too_long);
  RUN_TEST(Unformatted_input_get_int_type_void);
  RUN_TEST(Unformatted_input_get_char);
  RUN_TEST(Unformatted_input_get_char_pointer);
  RUN_TEST(Unformatted_input_get_char_pointer_available_too_small);
  RUN_TEST(Unformatted_input_get_char_pointer_empty_stream);
  RUN_TEST(Unformatted_input_get_char_pointer_get_buffer_smaller_than_read_request);
  RUN_TEST(Unformatted_input_getline_char_pointer_delimiter_spaces);
  RUN_TEST(Formatted_input_uint16);
  RUN_TEST(Formatted_input_uint16_hex);
  RUN_TEST(Formatted_input_uint16_0);
  RUN_TEST(Formatted_input_uint16_0_space);
  RUN_TEST(Formatted_input_uint16_2x);
  RUN_TEST(Formatted_input_uint16_2x_char);
  RUN_TEST(Formatted_input_sint16_pos);
  RUN_TEST(Formatted_input_sint16_neg);
  RUN_TEST(Formatted_input_sint16_0);
  RUN_TEST(Formatted_input_sint16_0_space);
  RUN_TEST(Formatted_input_sint16_pos_hex);
  RUN_TEST(Formatted_input_sint16_pos_hex_fail);
  RUN_TEST(Formatted_input_sint16_neg_hex);
  RUN_TEST(Formatted_input_sint16_neg_hex_fail);
  RUN_TEST(Formatted_input_sint16_pos_big);
  RUN_TEST(Formatted_input_sint16_neg_big);
  RUN_TEST(Formatted_input_char);
  RUN_TEST(Formatted_input_char_spaces);
  RUN_TEST(Formatted_input_char_array);
  RUN_TEST(Formatted_input_char_array_spaces);
  RUN_TEST(Formatted_input_char_array_spaces_width);
 
  (void) UNITY_END();

  // Return false to stop program execution (relevant on Windows)
  return false;
}