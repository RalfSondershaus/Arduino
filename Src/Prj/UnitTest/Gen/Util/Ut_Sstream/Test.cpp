/**
 * @file test.cpp
 *
 * @author Ralf Sondershaus
 *
 * Google Test for Gen/Util/Sstream.h
 */

#include <stdint.h>
#include <gtest/gtest.h>
#include <Util/Sstream.h>
#include <sstream>

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Construct_empty)
{
  using istringstream_t = util::basic_istringstream<16, char>;
  using string_t = istringstream_t::string_type;

  istringstream_t stream;

  EXPECT_EQ(stream.str().compare(string_t{ "" }), 0);
  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });

  std::istringstream ss;
  EXPECT_EQ(ss.tellg(), 0);
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Construct_with_string)
{
  using istringstream_t = util::basic_istringstream<16, char>;
  using string_t = istringstream_t::string_type;

  istringstream_t stream("Hallo");

  EXPECT_EQ(stream.str().compare(string_t{ "Hallo" }), 0);
  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });

  std::istringstream ss("Hallo");
  EXPECT_EQ(ss.tellg(), 0);
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
  using string_t = istringstream_t::string_type;

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

  std::istringstream ss("abcde");
  EXPECT_EQ(ss.tellg(), util::streampos{ 0 });
  EXPECT_EQ(std::istringstream::traits_type::to_char_type(ss.get()), 'a');
  EXPECT_EQ(ss.tellg(), util::streampos{ 1 });
  EXPECT_EQ(std::istringstream::traits_type::to_char_type(ss.get()), 'b');
  EXPECT_EQ(ss.tellg(), util::streampos{ 2 });
  EXPECT_EQ(std::istringstream::traits_type::to_char_type(ss.get()), 'c');
  EXPECT_EQ(ss.tellg(), util::streampos{ 3 });
  EXPECT_EQ(std::istringstream::traits_type::to_char_type(ss.get()), 'd');
  EXPECT_EQ(ss.tellg(), util::streampos{ 4 });
  EXPECT_EQ(std::istringstream::traits_type::to_char_type(ss.get()), 'e');
  EXPECT_EQ(ss.tellg(), util::streampos{ 5 });
  EXPECT_EQ(ss.get(), -1);
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Unformatted_input_get_char)
{
  using istringstream_t = util::basic_istringstream<64, char>;
  using traits_t = istringstream_t::traits_type;
  using string_t = istringstream_t::string_type;

  istringstream_t stream("abcde");
  char c;

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream.get(c);
  EXPECT_EQ(c, 'a');
  EXPECT_EQ(stream.tellg(), util::streampos{ 1 });
  EXPECT_EQ(stream.gcount(), util::streampos{ 1 });
  stream.get(c);
  EXPECT_EQ(c, 'b');
  EXPECT_EQ(stream.tellg(), util::streampos{ 2 });
  EXPECT_EQ(stream.gcount(), util::streampos{ 1 });
  stream.get(c);
  EXPECT_EQ(c, 'c');
  EXPECT_EQ(stream.tellg(), util::streampos{ 3 });
  EXPECT_EQ(stream.gcount(), util::streampos{ 1 });
  stream.get(c);
  EXPECT_EQ(c, 'd');
  EXPECT_EQ(stream.tellg(), util::streampos{ 4 });
  EXPECT_EQ(stream.gcount(), util::streampos{ 1 });
  stream.get(c);
  EXPECT_EQ(c, 'e');
  EXPECT_EQ(stream.tellg(), util::streampos{ 5 });
  EXPECT_EQ(stream.gcount(), util::streampos{ 1 });
  stream.get(c);
  EXPECT_EQ(c, 'e');
  EXPECT_EQ(stream.tellg(), util::streampos{ -1 });
  EXPECT_EQ(stream.gcount(), util::streampos{ 0 });
  EXPECT_EQ(stream.get(), -1);

  std::istringstream ss("abcde");
  EXPECT_EQ(ss.tellg(), util::streampos{ 0 });
  ss.get(c);
  EXPECT_EQ(c, 'a');
  EXPECT_EQ(ss.tellg(), util::streampos{ 1 });
  EXPECT_EQ(ss.gcount(), util::streampos{ 1 });
  ss.get(c);
  EXPECT_EQ(c, 'b');
  EXPECT_EQ(ss.tellg(), util::streampos{ 2 });
  EXPECT_EQ(ss.gcount(), util::streampos{ 1 });
  ss.get(c);
  EXPECT_EQ(c, 'c');
  EXPECT_EQ(ss.tellg(), util::streampos{ 3 });
  EXPECT_EQ(ss.gcount(), util::streampos{ 1 });
  ss.get(c);
  EXPECT_EQ(c, 'd');
  EXPECT_EQ(ss.tellg(), util::streampos{ 4 });
  EXPECT_EQ(ss.gcount(), util::streampos{ 1 });
  ss.get(c);
  EXPECT_EQ(c, 'e');
  EXPECT_EQ(ss.tellg(), util::streampos{ 5 });
  EXPECT_EQ(ss.gcount(), util::streampos{ 1 });
  ss.get(c);
  EXPECT_EQ(c, 'e');
  EXPECT_EQ(ss.tellg(), util::streampos{ -1 });
  EXPECT_EQ(ss.gcount(), util::streampos{ 0 });
  EXPECT_EQ(ss.get(), -1);
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Unformatted_input_get_char_pointer)
{
  using istringstream_t = util::basic_istringstream<64, char>;
  using traits_t = istringstream_t::traits_type;
  using string_t = istringstream_t::string_type;
  
  istringstream_t stream("abcde");
  constexpr size_t kNrChars = 4; // kNrChars-1 characters are copied (last array element is terminating 0)
  char s[kNrChars];

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream.get(s, kNrChars);
  EXPECT_EQ(string_t{s}.compare(string_t{"abc"}), 0);
  EXPECT_EQ(stream.gcount(), util::streampos{ 3 });

  std::istringstream ss("abcde");
  EXPECT_EQ(ss.tellg(), util::streampos{ 0 });
  ss.get(s, kNrChars);
  EXPECT_EQ(std::string{ s }, std::string{ "abc" });
  EXPECT_EQ(ss.gcount(), util::streampos{ 3 });
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Unformatted_input_get_char_pointer_available_too_small)
{
  using istringstream_t = util::basic_istringstream<64, char>;
  using traits_t = istringstream_t::traits_type;
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
  EXPECT_EQ(stream.gcount(), 5);

  std::istringstream ss("abcde");
  EXPECT_EQ(ss.tellg(), util::streampos{ 0 });
  ss.get(s, kNrChars);
  EXPECT_EQ(std::string{ s }, std::string{ "abcde" });
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), false);
  EXPECT_EQ(ss.good(), false);
  EXPECT_EQ(!ss, false);
  EXPECT_EQ((bool)ss, true);
  EXPECT_EQ(ss.gcount(), 5);
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Unformatted_input_get_char_pointer_empty_stream)
{
  using istringstream_t = util::basic_istringstream<64, char>;
  using traits_t = istringstream_t::traits_type;
  using string_t = istringstream_t::string_type;

  istringstream_t stream("");
  constexpr size_t kNrChars = 10;
  char s[kNrChars];

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream.get(s, kNrChars);
  EXPECT_EQ(string_t{ s }.compare(string_t{ "" }), 0);
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), true);
  EXPECT_EQ(stream.gcount(), 0);

  std::istringstream ss("");
  EXPECT_EQ(ss.tellg(), 0);
  ss.get(s, kNrChars);
  EXPECT_EQ(std::string{ s }, std::string{ "" });
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), true);
  EXPECT_EQ(ss.gcount(), 0);
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Unformatted_input_get_char_pointer_get_buffer_smaller_than_read_request)
{
  using istringstream_t = util::basic_istringstream<8, char>;
  using traits_t = istringstream_t::traits_type;
  using string_t = istringstream_t::string_type;

  istringstream_t stream("12345678");
  constexpr size_t kNrChars = 10;
  char s[kNrChars];

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream.get(s, kNrChars);
  EXPECT_EQ(string_t{ s }.compare(string_t{ "12345678" }), 0);
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), false);
  EXPECT_EQ(stream.gcount(), 8);
}

// -------------------------------------------------------------------------
TEST(Ut_Sstream, Formatted_input_uint16)
{
  using istringstream_t = util::basic_istringstream<16, char>;
  using traits_t = istringstream_t::traits_type;
  using string_t = istringstream_t::string_type;

  istringstream_t stream("12345");
  uint16 un;

  EXPECT_EQ(stream.tellg(), util::streampos{ 0 });
  stream >> un;
  EXPECT_EQ(un, 12345);
  EXPECT_EQ(stream.eof(), true);
  EXPECT_EQ(stream.fail(), false);
  EXPECT_EQ(stream.gcount(), 5);

  std::istringstream ss("12345");
  EXPECT_EQ(ss.tellg(), 0);
  ss >> un;
  EXPECT_EQ(un, 12345);
  EXPECT_EQ(ss.gcount(), 5);
  EXPECT_EQ(ss.eof(), true);
  EXPECT_EQ(ss.fail(), false);
}
