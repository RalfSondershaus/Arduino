/**
 * @file test.cpp
 *
 * @author Ralf Sondershaus
 *
 * Google Test for Gen/Util/String.h
 */

#include <Util/String.h>
#include <gtest/gtest.h>
#include <stdint.h>
#include <array>
#include <algorithm>

std::streampos h;

// explicit template instantiation to verify that all functions are compiling
template util::basic_string<16, char>;

TEST(Ut_String, Construct_empty)
{
  util::basic_string<16, char> str;
  EXPECT_EQ(str.empty(), true);
  str.append("", 0);
  EXPECT_EQ(str.empty(), true);
}

TEST(Ut_String, Construct_from_const_pointer)
{
  util::basic_string<16, char> str("hello");
  EXPECT_EQ(str.empty(), false);
  EXPECT_EQ(str.compare("hello"), 0);
  str.append("", 0);
  EXPECT_EQ(str.empty(), false);
}

TEST(Ut_String, Construct_from_cost_pointer_empty)
{
  util::basic_string<16, char> str("");
  EXPECT_EQ(str.empty(), true);
  EXPECT_EQ(str.compare(""), 0);
}

TEST(Ut_String, Construct_from_char)
{
  util::basic_string<16, char> str('A');
  EXPECT_EQ(str.empty(), false);
  EXPECT_EQ(str.compare("A"), 0);
}

TEST(Ut_String, Assign_operator_string)
{
  util::basic_string<16, char> strA('A');
  util::basic_string<16, char> strB('B');
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.length(), 1U);
  EXPECT_EQ(strA.compare("A"), 0);
  EXPECT_EQ(strB.empty(), false);
  EXPECT_EQ(strB.compare("B"), 0);
  strA = strB;
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.length(), 1U);
  EXPECT_EQ(strA.compare("B"), 0);
  EXPECT_EQ(strB.empty(), false);
  EXPECT_EQ(strB.compare("B"), 0);
}

TEST(Ut_String, Assign_operator_string_longer)
{
  util::basic_string<16, char> strA('A');
  util::basic_string<32, char> strB('B');
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.length(), 1U);
  EXPECT_EQ(strA.compare("A"), 0);
  EXPECT_EQ(strB.empty(), false);
  EXPECT_EQ(strB.compare("B"), 0);
  strA = strB;
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.length(), 1U);
  EXPECT_EQ(strA.compare("B"), 0);
  EXPECT_EQ(strB.empty(), false);
  EXPECT_EQ(strB.compare("B"), 0);
}

TEST(Ut_String, Assign_operator_string_shorter)
{
  util::basic_string<16, char> strA('A');
  util::basic_string<8, char> strB('B');
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.length(), 1U);
  EXPECT_EQ(strA.compare("A"), 0);
  EXPECT_EQ(strB.empty(), false);
  EXPECT_EQ(strB.compare("B"), 0);
  strA = strB;
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.length(), 1U);
  EXPECT_EQ(strA.compare("B"), 0);
  EXPECT_EQ(strB.empty(), false);
  EXPECT_EQ(strB.compare("B"), 0);
}

TEST(Ut_String, Assign_operator_string_4_string_8)
{
  util::basic_string<4, char> strA("A");
  util::basic_string<8, char> strB("BCDEFGHI");
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.length(), 1U);
  EXPECT_EQ(strA.compare("A"), 0);
  EXPECT_EQ(strB.empty(), false);
  EXPECT_EQ(strB.length(), 8U);
  EXPECT_EQ(strB.compare("BCDEFGHI"), 0);
  strA = strB;
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.length(), 4U);
  EXPECT_EQ(strA.compare("BCDE"), 0);
  EXPECT_EQ(strB.empty(), false);
  EXPECT_EQ(strB.length(), 8U);
  EXPECT_EQ(strB.compare("BCDEFGHI"), 0);
}

TEST(Ut_String, Assign_operator_string_8_string_4)
{
  util::basic_string<8, char> strA("ABCDEFGH");
  util::basic_string<4, char> strB("WXYZ");
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.length(), 8U);
  EXPECT_EQ(strA.compare("ABCDEFGH"), 0);
  EXPECT_EQ(strB.empty(), false);
  EXPECT_EQ(strB.length(), 4U);
  EXPECT_EQ(strB.compare("WXYZ"), 0);
  strA = strB;
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.length(), 4U);
  EXPECT_EQ(strA.compare("WXYZ"), 0);
  EXPECT_EQ(strB.empty(), false);
  EXPECT_EQ(strB.length(), 4U);
  EXPECT_EQ(strB.compare("WXYZ"), 0);
}

TEST(Ut_String, Assign_operator_string_string_empty)
{
  util::basic_string<8, char> strA("ABCDEFGH");
  util::basic_string<4, char> strB("");
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.length(), 8U);
  EXPECT_EQ(strA.compare("ABCDEFGH"), 0);
  EXPECT_EQ(strB.empty(), true);
  EXPECT_EQ(strB.length(), 0U);
  EXPECT_EQ(strB.compare(""), 0);
  strA = strB;
  EXPECT_EQ(strA.empty(), true);
  EXPECT_EQ(strA.length(), 0U);
  EXPECT_EQ(strA.compare(""), 0);
  EXPECT_EQ(strB.empty(), true);
  EXPECT_EQ(strB.length(), 0U);
  EXPECT_EQ(strB.compare(""), 0);
}

TEST(Ut_String, Assign_operator_string_empty_string)
{
  util::basic_string<4, char> strA("");
  util::basic_string<8, char> strB("ABCDEFGH");
  EXPECT_EQ(strA.empty(), true);
  EXPECT_EQ(strA.length(), 0U);
  EXPECT_EQ(strA.compare(""), 0);
  EXPECT_EQ(strB.empty(), false);
  EXPECT_EQ(strB.length(), 8U);
  EXPECT_EQ(strB.compare("ABCDEFGH"), 0);
  strA = strB;
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.length(), 4U);
  EXPECT_EQ(strA.compare("ABCD"), 0);
  EXPECT_EQ(strB.empty(), false);
  EXPECT_EQ(strB.length(), 8U);
  EXPECT_EQ(strB.compare("ABCDEFGH"), 0);
}

TEST(Ut_String, Assign_operator_const_pointer)
{
  util::basic_string<16, char> strA('A');
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.compare("A"), 0);
  strA = "B";
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.compare("B"), 0);
}

TEST(Ut_String, Append_const_pointer)
{
  util::basic_string<16, char> str;

  EXPECT_EQ(str.empty(), true);

  str.append("hello");
  EXPECT_EQ(str.empty(), false);
  EXPECT_EQ(str.compare("hello"), 0);
}

TEST(Ut_String, Append_string)
{
  util::basic_string<16, char> strA;
  util::basic_string<16, char> strB("hello");

  EXPECT_EQ(strA.empty(), true);

  strA.append(strB);
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.compare("hello"), 0);
}

TEST(Ut_String, Append_sub_string)
{
  util::basic_string<16, char> strA;
  util::basic_string<8, char> strB("hello");

  EXPECT_EQ(strA.empty(), true);

  strA.append(strB, 0U, 3U);
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.compare("hel"), 0);

  strA = "";
  EXPECT_EQ(strA.empty(), true);
  strA.append(strB, 3U, 3U);
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.compare("lo"), 0);
  EXPECT_EQ(strA.size(), 2U);

  strA = "123456789ABC";
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.size(), 12U);
  strA.append(strB, 0U, 5U);
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.size(), 16U);
  EXPECT_EQ(strA.find("hell"), 12U);
}

TEST(Ut_String, Append_sub_const_pointer)
{
  util::basic_string<16, char> strA;

  EXPECT_EQ(strA.empty(), true);

  strA.append("hello", 3U);
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.compare("hel"), 0);
  EXPECT_EQ(strA.size(), 3U);

  strA = "123456789ABC";
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.size(), 12U);
  strA.append("hello", 5U);
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.size(), 16U);
  EXPECT_EQ(strA.find("hell"), 12U);
}

TEST(Ut_String, Find_const_pointer_pos_0)
{
  util::basic_string<16, char> str("hallihallo");

  EXPECT_EQ(str.empty(), false);
  EXPECT_EQ(str.find("hallo"), 5U);
}

TEST(Ut_String, Find_string_pos_0)
{
  util::basic_string<16, char> strA("hallihallo");
  util::basic_string<16, char> strB("hallo");

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB), 5U);
}

TEST(Ut_String, Find_string_pos_0_negative)
{
  using string = typename util::basic_string<16, char>;
  string strA("hallihall");
  string strB("hallo");

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB), string::npos);
}

TEST(Ut_String, Find_const_pointer_pos_n)
{
  util::basic_string<16, char> strA("hallihallo");

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find("hall", 1), 5U);
}

TEST(Ut_String, Find_string_pos_n)
{
  util::basic_string<16, char> strA("hallihallo");
  util::basic_string<16, char> strB("hall");

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB, 1), 5U);
}

TEST(Ut_String, Find_string_pos_n_negative)
{
  using string = typename util::basic_string<16, char>;
  string strA("hallihallo");
  string strB("hall");

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB, strA.length() + 1), string::npos);
}

// find without restart
TEST(Ut_String, Find_string_abcd_bc)
{
  using string = typename util::basic_string<16, char>;
  string strA("abcd");
  string strB("bc");
  const util::size_t nPos = 0;

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB, nPos), 1U);
}

// find with restart
TEST(Ut_String, Find_string_ababcd_bc)
{
  using string = typename util::basic_string<16, char>;
  string strA("ababcd");
  string strB("bc");
  const util::size_t nPos = 0;

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB, nPos), 3U);
}

// find with deep restart
TEST(Ut_String, Find_string_abbdbbcd_bc)
{
  using string = typename util::basic_string<16, char>;
  string strA("abbdbbcd");
  string strB("bc");
  const util::size_t nPos = 0;

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB, nPos), 5U);
}

// find empty string
TEST(Ut_String, Find_string_abcd_empty)
{
  using string = typename util::basic_string<16, char>;
  string strA("abcd");
  string strB("");
  const util::size_t nPos = 0;

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB, nPos), 0U);
}

// find without restart
TEST(Ut_String, Find_const_pointer_abcd_bc)
{
  using string = typename util::basic_string<16, char>;
  string strA("abcd");
  const char strB[] = "bc";
  const util::size_t nPos = 0;

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB, nPos), 1U);
}

// find with restart
TEST(Ut_String, Find_const_pointer_ababcd_bc)
{
  using string = typename util::basic_string<16, char>;
  string strA("ababcd");
  const char strB[] = "bc";
  const util::size_t nPos = 0;

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB, nPos), 3U);
}

// find with deep restart
TEST(Ut_String, Find_const_pointer_abbdbbcd_bc)
{
  using string = typename util::basic_string<16, char>;
  string strA("abbdbbcd");
  const char strB[] = "bc";
  const util::size_t nPos = 0;

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB, nPos), 5U);
}

// find empty string
TEST(Ut_String, Find_const_pointer_abcd_empty)
{
  using string = typename util::basic_string<16, char>;
  string strA("abcd");
  const char strB[] = "";
  const util::size_t nPos = 0;

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB, nPos), 0U);
}

// find character
TEST(Ut_String, Find_character)
{
  using string = typename util::basic_string<16, char>;
  const string strA("abcdABCD");
  const util::size_t npos = string::npos;
  struct Step
  {
  public:
    char c;
    util::size_t expPos;
  };
  const std::array<Step, 6> steps = { {
    { 'a', 0 },
    { 'b', 1 },
    { 'e', npos },
    { 'A', 4 },
    { 'B', 5 },
    { 'E', npos }
  } };

  std::for_each(steps.begin(), steps.end(), [&](const Step& v) { EXPECT_EQ(strA.find(v.c), v.expPos); });
}

TEST(Ut_String, Compare_with_const_pointer)
{
  util::basic_string<16, char> str("hello");
  EXPECT_EQ(str.empty(), false);
  EXPECT_EQ(str.compare("hello"), 0);
  EXPECT_EQ(str.compare("hel"), 1);
  EXPECT_EQ(str.compare("helloo"), -1);
  str.append("", 0);
  EXPECT_EQ(str.empty(), false);
}

TEST(Ut_String, Compare_with_string)
{
  util::basic_string<16, char> strA("hello");
  util::basic_string<16, char> strB("hello");
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.compare(strB), 0);
  strB = "hel";
  EXPECT_EQ(strA.compare(strB), 1);
  strB = "helloo";
  EXPECT_EQ(strA.compare(strB), -1);
}

TEST(Ut_String, stoi_base10)
{
  typedef struct
  {
    util::basic_string<16, char> str;
    int n;
  } tStep;
  static const tStep aSteps[] =
  {
    { "0", 0 },
    { "  0", 0 },
    { "1", 1 },
    { "-1", -1 },
    { "-32768", INT16_MIN },
    { "32767", INT16_MAX },
    { "-2147483648", INT32_MIN }, // for 32 bit architectures
    { "2147483647", INT32_MAX },   // for 32 bit architectures
    { "word with 1", 0 },
  };
  int i, n;

  for (i = 0; i < sizeof(aSteps) / sizeof(tStep); i++)
  {
    n = util::stoi(aSteps[i].str);
    EXPECT_EQ(n, aSteps[i].n);
  }
}

TEST(Ut_String, stoi_base16)
{
  typedef struct
  {
    util::basic_string<16, char> str;
    long n;
  } tStep;
  static const tStep aSteps[] =
  {
    { "0", 0 },
    { "  0", 0 },
    { "1", 1 },
    { "  1", 1 },
    { "-1", -1 },
    { "-8000", INT16_MIN },
    { "7FFF", INT16_MAX },
    { "-80000000", INT32_MIN }, // for 32 bit architectures
    { "7FFFFFFF", INT32_MAX },   // for 32 bit architectures
    { "word with 1", 0 },
  };
  int i, n;

  for (i = 0; i < sizeof(aSteps) / sizeof(tStep); i++)
  {
    n = util::stoi(aSteps[i].str, nullptr, 16);
    EXPECT_EQ(n, aSteps[i].n);
    EXPECT_EQ(errno, 0);
  }
}