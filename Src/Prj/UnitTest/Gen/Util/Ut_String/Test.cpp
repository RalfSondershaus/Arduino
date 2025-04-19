/**
 * @file Test.cpp
 *
 * @author Ralf Sondershaus
 *
 * Unity Test for Gen/Util/String.h
 */

#include <unity_adapt.h>

#include <Util/Array.h>
#include <Util/String.h>
#include <stdint.h>

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
template<> void EXPECT_EQ<std::streampos>(std::streampos actual, std::streampos expected) { EXPECT_EQ<std::streamoff>(expected, actual); }
template<> void EXPECT_EQ<std::string>(std::string actual, std::string expected) { TEST_ASSERT_EQUAL_STRING(expected.c_str(), actual.c_str()); }
#endif

// -------------------------------------------------------------------------
void Ut_String_Construct_empty(void)
{
  util::basic_string<16, char> str;
  EXPECT_EQ(str.empty(), true);
  str.append("", 0);
  EXPECT_EQ(str.empty(), true);
}

// -------------------------------------------------------------------------
void Ut_String_Construct_from_const_pointer(void)
{
  util::basic_string<16, char> str("hello");
  EXPECT_EQ(str.empty(), false);
  EXPECT_EQ(str.compare("hello"), 0);
  str.append("", 0);
  EXPECT_EQ(str.empty(), false);
}

// -------------------------------------------------------------------------
void Ut_String_Construct_from_cost_pointer_empty(void)
{
  util::basic_string<16, char> str("");
  EXPECT_EQ(str.empty(), true);
  EXPECT_EQ(str.compare(""), 0);
}

// -------------------------------------------------------------------------
void Ut_String_Construct_from_char(void)
{
  util::basic_string<16, char> str('A');
  EXPECT_EQ(str.empty(), false);
  EXPECT_EQ(str.compare("A"), 0);
}

// -------------------------------------------------------------------------
void Ut_String_Assign_operator_string(void)
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

// -------------------------------------------------------------------------
void Ut_String_Assign_operator_string_longer(void)
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

// -------------------------------------------------------------------------
void Ut_String_Assign_operator_string_shorter(void)
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

// -------------------------------------------------------------------------
void Ut_String_Assign_operator_string_4_string_8(void)
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

// -------------------------------------------------------------------------
void Ut_String_Assign_operator_string_8_string_4(void)
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

// -------------------------------------------------------------------------
void Ut_String_Assign_operator_string_string_empty(void)
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

// -------------------------------------------------------------------------
void Ut_String_Assign_operator_string_empty_string(void)
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

// -------------------------------------------------------------------------
void Ut_String_Assign_operator_const_pointer(void)
{
  util::basic_string<16, char> strA('A');
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.compare("A"), 0);
  strA = "B";
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.compare("B"), 0);
}

// -------------------------------------------------------------------------
void Ut_String_Append_const_pointer(void)
{
  util::basic_string<16, char> str;

  EXPECT_EQ(str.empty(), true);

  str.append("hello");
  EXPECT_EQ(str.empty(), false);
  EXPECT_EQ(str.compare("hello"), 0);
}

// -------------------------------------------------------------------------
void Ut_String_Append_const_pointer_with_clear(void)
{
  util::basic_string<16, char> str;

  EXPECT_EQ(str.empty(), true);

  str.append("hello");
  EXPECT_EQ(str.empty(), false);
  EXPECT_EQ(str.compare("hello"), 0);
  str.clear();
  EXPECT_EQ(str.empty(), true);
  EXPECT_EQ(str.size(), static_cast<util::size_t>(0));
  EXPECT_EQ(str.max_size(), static_cast<util::size_t>(16));
  str.append("abc");
  EXPECT_EQ(str.empty(), false);
  EXPECT_EQ(str.compare("abc"), 0);
}

// -------------------------------------------------------------------------
void Ut_String_Append_string(void)
{
  util::basic_string<16, char> strA;
  util::basic_string<16, char> strB("hello");

  EXPECT_EQ(strA.empty(), true);

  strA.append(strB);
  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.compare("hello"), 0);
}

// -------------------------------------------------------------------------
void Ut_String_Append_sub_string(void)
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

// -------------------------------------------------------------------------
void Ut_String_Append_sub_const_pointer(void)
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

// -------------------------------------------------------------------------
void Ut_String_operator_plus_equal_char_with_clear(void)
{
  util::basic_string<16, char> str;

  EXPECT_EQ(str.empty(), true);

  str += 'a';
  EXPECT_EQ(str.empty(), false);
  EXPECT_EQ(str.compare("a"), 0);
  str += 'b';
  EXPECT_EQ(str.empty(), false);
  EXPECT_EQ(str.compare("ab"), 0);
  str.clear();
  EXPECT_EQ(str.empty(), true);
  str += 'c';
  EXPECT_EQ(str.empty(), false);
  EXPECT_EQ(str.compare("c"), 0);
  str += 'd';
  EXPECT_EQ(str.empty(), false);
  EXPECT_EQ(str.compare("cd"), 0);
}

// -------------------------------------------------------------------------
void Ut_String_Find_const_pointer_pos_0(void)
{
  util::basic_string<16, char> str("hallihallo");

  EXPECT_EQ(str.empty(), false);
  EXPECT_EQ(str.find("hallo"), 5U);
}

// -------------------------------------------------------------------------
void Ut_String_Find_string_pos_0(void)
{
  util::basic_string<16, char> strA("hallihallo");
  util::basic_string<16, char> strB("hallo");

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB), 5U);
}

// -------------------------------------------------------------------------
void Ut_String_Find_string_pos_0_negative(void)
{
  using string = typename util::basic_string<16, char>;
  string strA("hallihall");
  string strB("hallo");

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB), string::npos);
}

// -------------------------------------------------------------------------
void Ut_String_Find_const_pointer_pos_n(void)
{
  util::basic_string<16, char> strA("hallihallo");

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find("hall", 1), 5U);
}

// -------------------------------------------------------------------------
void Ut_String_Find_string_pos_n(void)
{
  util::basic_string<16, char> strA("hallihallo");
  util::basic_string<16, char> strB("hall");

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB, 1), 5U);
}

// -------------------------------------------------------------------------
void Ut_String_Find_string_pos_n_negative(void)
{
  using string = typename util::basic_string<16, char>;
  string strA("hallihallo");
  string strB("hall");

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB, strA.length() + 1), string::npos);
}

// -------------------------------------------------------------------------
// find without restart
void Ut_String_Find_string_abcd_bc(void)
{
  using string = typename util::basic_string<16, char>;
  string strA("abcd");
  string strB("bc");
  const util::size_t nPos = 0;

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB, nPos), 1U);
}

// -------------------------------------------------------------------------
// find with restart
void Ut_String_Find_string_ababcd_bc(void)
{
  using string = typename util::basic_string<16, char>;
  string strA("ababcd");
  string strB("bc");
  const util::size_t nPos = 0;

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB, nPos), 3U);
}

// -------------------------------------------------------------------------
// find with deep restart
void Ut_String_Find_string_abbdbbcd_bc(void)
{
  using string = typename util::basic_string<16, char>;
  string strA("abbdbbcd");
  string strB("bc");
  const util::size_t nPos = 0;

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB, nPos), 5U);
}

// -------------------------------------------------------------------------
// find empty string
void Ut_String_Find_string_abcd_empty(void)
{
  using string = typename util::basic_string<16, char>;
  string strA("abcd");
  string strB("");
  const util::size_t nPos = 0;

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB, nPos), 0U);
}

// -------------------------------------------------------------------------
// find without restart
void Ut_String_Find_const_pointer_abcd_bc(void)
{
  using string = typename util::basic_string<16, char>;
  string strA("abcd");
  const char strB[] = "bc";
  const util::size_t nPos = 0;

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB, nPos), 1U);
}

// -------------------------------------------------------------------------
// find with restart
void Ut_String_Find_const_pointer_ababcd_bc(void)
{
  using string = typename util::basic_string<16, char>;
  string strA("ababcd");
  const char strB[] = "bc";
  const util::size_t nPos = 0;

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB, nPos), 3U);
}

// -------------------------------------------------------------------------
// find with deep restart
void Ut_String_Find_const_pointer_abbdbbcd_bc(void)
{
  using string = typename util::basic_string<16, char>;
  string strA("abbdbbcd");
  const char strB[] = "bc";
  const util::size_t nPos = 0;

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB, nPos), 5U);
}

// -------------------------------------------------------------------------
// find empty string
void Ut_String_Find_const_pointer_abcd_empty(void)
{
  using string = typename util::basic_string<16, char>;
  string strA("abcd");
  const char strB[] = "";
  const util::size_t nPos = 0;

  EXPECT_EQ(strA.empty(), false);
  EXPECT_EQ(strA.find(strB, nPos), 0U);
}

// -------------------------------------------------------------------------
// find character
void Ut_String_Find_character(void)
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
  const util::array<Step, 6> steps = { {
    { 'a', 0 },
    { 'b', 1 },
    { 'e', npos },
    { 'A', 4 },
    { 'B', 5 },
    { 'E', npos }
  } };

  for (auto it = steps.begin(); it != steps.end(); it++) 
  { 
    EXPECT_EQ(strA.find(it->c), it->expPos); 
  };
}

// -------------------------------------------------------------------------
void Ut_String_Compare_with_const_pointer(void)
{
  util::basic_string<16, char> str("hello");
  EXPECT_EQ(str.empty(), false);
  EXPECT_EQ(str.compare("hello"), 0);
  EXPECT_EQ(str.compare("hel"), 1);
  EXPECT_EQ(str.compare("helloo"), -1);
  str.append("", 0);
  EXPECT_EQ(str.empty(), false);
}

// -------------------------------------------------------------------------
void Ut_String_Compare_with_string(void)
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

// -------------------------------------------------------------------------
void Ut_String_stoi_base0(void)
{
  typedef struct
  {
    util::basic_string<16, char> str;
    int n;
    size_t pos;
  } tStep;
  static const tStep aSteps[] =
  {
    { "0", 0, 1U },
    { "  0", 0, 3U },
    { "1", 1, 1U },
    { "-1", -1, 2U },
    { "-32768", INT16_MIN, 6U },
    { "32767", INT16_MAX, 5U },
    { "-2147483648", INT32_MIN, 11U }, // for 32 bit architectures
    { "2147483647", INT32_MAX, 10U },   // for 32 bit architectures
    { "word with 1", 0, 0U },
    { "-0x8000", INT16_MIN, 7U },
    { "0x7FFF", INT16_MAX, 6U },
    { "-0x00008000", INT16_MIN, 11U },
    { "0x00007FFF", INT16_MAX, 10U },
    { "-010", -8, 4U }, // octal base
    { "-015", -(8+5), 4U }, // octal base
    { "010", 8, 3U }, // octal base
    { "00", 0, 2U }, // octal base
    { "017", 8+7, 3U }, // octal base
  };
  size_t i;
  int n;
  size_t pos;

  for (i = 0; i < sizeof(aSteps) / sizeof(tStep); i++)
  {
    n = util::stoi<int>(aSteps[i].str, &pos, 0);
    EXPECT_EQ(n, aSteps[i].n);
    EXPECT_EQ(pos, aSteps[i].pos);
  }
}

// -------------------------------------------------------------------------
void Ut_String_stoi_base10(void)
{
  typedef struct
  {
    util::basic_string<16, char> str;
    int n;
    size_t pos;
  } tStep;
  static const tStep aSteps[] =
  {
    { "0", 0, 1U },
    { "  0", 0, 3U },
    { "1", 1, 1U },
    { "-1", -1, 2U },
    { "-32768", INT16_MIN, 6U },
    { "32767", INT16_MAX, 5U },
    { "-2147483648", INT32_MIN, 11U}, // for 32 bit architectures
    { "2147483647", INT32_MAX, 10U },   // for 32 bit architectures
    { "-21474836480", 0, 0U }, // out of range for 32 bit architectures
    { "21474836470", 0, 0U },  // out of range for 32 bit architectures
    { "word with 1", 0, 0U }
  };
  size_t i;
  int n;
  size_t pos;

  for (i = 0; i < sizeof(aSteps) / sizeof(tStep); i++)
  {
    n = util::stoi<int>(aSteps[i].str, &pos);
    EXPECT_EQ(n, aSteps[i].n);
    EXPECT_EQ(pos, aSteps[i].pos);
  }
}

// -------------------------------------------------------------------------
void Ut_String_stoi_base16(void)
{
  typedef struct
  {
    util::basic_string<16, char> str;
    int n;
    size_t pos;
  } tStep;
  static const tStep aSteps[] =
  {
    { "0", 0, 1U },
    { "  0", 0, 3U },
    { "1", 1, 1U },
    { "  1", 1, 3U },
    { "-1", -1, 2U },
    { "-8000", INT16_MIN, 5U },
    { "7FFF", INT16_MAX, 4U },
    { "-0x8000", INT16_MIN, 7U },
    { "0x7FFF", INT16_MAX, 6U },
    { "-80000000", INT32_MIN, 9U }, // for 32 bit architectures
    { "7FFFFFFF", INT32_MAX, 8U },  // for 32 bit architectures
    { "-800000000", 0, 0U },        // out of range for 32 bit architectures
    { "7FFFFFFFF", 0, 0U },         // out of range for 32 bit architectures
    { "word with 1", 0, 0U }
  };
  int n;
  size_t pos, i;

  for (i = 0; i < sizeof(aSteps) / sizeof(tStep); i++)
  {
    n = util::stoi<int> (aSteps[i].str, &pos, 16);
    EXPECT_EQ(n, aSteps[i].n);
    EXPECT_EQ(pos, aSteps[i].pos);
  }
}

// -------------------------------------------------------------------------
void Ut_String_stoui_uint32_base16(void)
{
  using basetype = uint32;
  typedef struct
  {
    util::basic_string<16, char> str;
    basetype n;
    size_t pos;
  } tStep;
  static const tStep aSteps[] =
  {
    { "0", 0U, 1U },
    { "  0", 0U, 3U },
    { "1", 1U, 1U },
    { "  1", 1U, 3U },
    { "-1", 0U, 0U },
    { "-8000", 0U, 0U },
    { "FFFF", UINT16_MAX, 4U },
    { "-80000000", 0U, 0U },
    { "FFFFFFFF", UINT32_MAX, 8U },
    { "word with 1", 0U, 0U }
  };
  basetype n;
  size_t pos, i;

  for (i = 0; i < sizeof(aSteps) / sizeof(tStep); i++)
  {
    n = util::stoui<basetype>(aSteps[i].str, &pos, 16);
    EXPECT_EQ(n, aSteps[i].n);
    EXPECT_EQ(pos, aSteps[i].pos);
  }
}

// -------------------------------------------------------------------------
void Ut_String_stoui_uint16_base16(void)
{
  using basetype = uint16;
  typedef struct
  {
    util::basic_string<16, char> str;
    basetype n;
    size_t pos;
  } tStep;
  static const tStep aSteps[] =
  {
    { "0", 0U, 1U },
    { "  0", 0U, 3U },
    { "1", 1U, 1U },
    { "  1", 1U, 3U },
    { "-1", 0U, 0U },
    { "-8000", 0U, 0U },
    { "FF", UINT8_MAX, 2U },
    { "FFFF", UINT16_MAX, 4U },
    { "-80000000", 0U, 0U },
    { "FFFFFFFF", 0U, 0U },
    { "word with 1", 0U, 0U }
  };
  basetype n;
  size_t pos, i;

  for (i = 0; i < sizeof(aSteps) / sizeof(tStep); i++)
  {
    n = util::stoui<basetype>(aSteps[i].str, &pos, 16);
    EXPECT_EQ(n, aSteps[i].n);
    EXPECT_EQ(pos, aSteps[i].pos);
  }
}

// -------------------------------------------------------------------------
void Ut_String_stoui_uint8_base16(void)
{
  using basetype = uint8;
  typedef struct
  {
    util::basic_string<16, char> str;
    basetype n;
    size_t pos;
  } tStep;
  static const tStep aSteps[] =
  {
    { "0", 0U, 1U },
    { "  0", 0U, 3U },
    { "1", 1U, 1U },
    { "  1", 1U, 3U },
    { "-1", 0U, 0U },
    { "-8000", 0U, 0U },
    { "FF", UINT8_MAX, 2U },
    { "FFFF", 0U, 0U },
    { "-80000000", 0U, 0U },
    { "FFFFFFFF", 0U, 0U },
    { "word with 1", 0U, 0U }
  };
  basetype n;
  size_t pos, i;

  for (i = 0; i < sizeof(aSteps) / sizeof(tStep); i++)
  {
    n = util::stoui<basetype>(aSteps[i].str, &pos, 16);
    EXPECT_EQ(n, aSteps[i].n);
    EXPECT_EQ(pos, aSteps[i].pos);
  }
}

// -------------------------------------------------------------------------
void Ut_String_stoui_uint32_base10(void)
{
  using basetype = uint32;
  typedef struct
  {
    util::basic_string<16, char> str;
    basetype n;
    size_t pos;
  } tStep;
  static const tStep aSteps[] =
  {
    { "0", 0U, 1U },
    { "  0", 0U, 3U },
    { "1", 1U, 1U },
    { "  1", 1U, 3U },
    { "-1", 0U, 0U },
    { "-8000", 0U, 0U },
    { "255", UINT8_MAX, 3U },
    { "-128", 0U, 0U },
    { "65535", UINT16_MAX, 5U },
    { "-32768", 0U, 0U },
    { "-2147483648", 0U, 0U },
    { "4294967295", UINT32_MAX, 10U },
    { "word with 1", 0U, 0U }
  };
  basetype n;
  size_t pos, i;

  for (i = 0; i < sizeof(aSteps) / sizeof(tStep); i++)
  {
    n = util::stoui<basetype>(aSteps[i].str, &pos, 10);
    EXPECT_EQ(n, aSteps[i].n);
    EXPECT_EQ(pos, aSteps[i].pos);
  }
}

// -------------------------------------------------------------------------
void Ut_String_stoui_uint16_base10(void)
{
  using basetype = uint16;
  typedef struct
  {
    util::basic_string<16, char> str;
    basetype n;
    size_t pos;
  } tStep;
  static const tStep aSteps[] =
  {
    { "0", 0U, 1U },
    { "  0", 0U, 3U },
    { "1", 1U, 1U },
    { "  1", 1U, 3U },
    { "-1", 0U, 0U },
    { "-8000", 0U, 0U },
    { "255", UINT8_MAX, 3U },
    { "-128", 0U, 0U },
    { "65535", UINT16_MAX, 5U },
    { "-32768", 0U, 0U },
    { "-2147483648", 0U, 0U },
    { "4294967295", 0U, 0U },
    { "word with 1", 0U, 0U }
  };
  basetype n;
  size_t pos, i;

  for (i = 0; i < sizeof(aSteps) / sizeof(tStep); i++)
  {
    n = util::stoui<basetype>(aSteps[i].str, &pos, 10);
    EXPECT_EQ(n, aSteps[i].n);
    EXPECT_EQ(pos, aSteps[i].pos);
  }
}

// -------------------------------------------------------------------------
void Ut_String_stoui_uint8_base10(void)
{
  using basetype = uint8;
  typedef struct
  {
    util::basic_string<16, char> str;
    basetype n;
    size_t pos;
  } tStep;
  static const tStep aSteps[] =
  {
    { "0", 0U, 1U },
    { "  0", 0U, 3U },
    { "1", 1U, 1U },
    { "  1", 1U, 3U },
    { "-1", 0U, 0U },
    { "-8000", 0U, 0U },
    { "255", UINT8_MAX, 3U },
    { "-128", 0U, 0U },
    { "65535", 0U, 0U },
    { "-32768", 0U, 0U },
    { "-2147483648", 0U, 0U },
    { "4294967295", 0U, 0U },
    { "word with 1", 0U, 0U }
  };
  basetype n;
  size_t pos, i;

  for (i = 0; i < sizeof(aSteps) / sizeof(tStep); i++)
  {
    n = util::stoui<basetype>(aSteps[i].str, &pos, 10);
    EXPECT_EQ(n, aSteps[i].n);
    EXPECT_EQ(pos, aSteps[i].pos);
  }
}

// -------------------------------------------------------------------------
void Ut_String_stoui_uint32_base0(void)
{
  using basetype = uint32;
  typedef struct
  {
    util::basic_string<16, char> str;
    basetype n;
    size_t pos;
  } tStep;
  static const tStep aSteps[] =
  {
    { "0", 0U, 1U },
    { "  0", 0U, 3U },
    { "1", 1U, 1U },
    { "  1", 1U, 3U },
    { "-1", 0U, 0U },
    { "-8000", 0U, 0U },
    { "255", UINT8_MAX, 3U },
    { "65535", UINT16_MAX, 5U },
    { "4294967295", UINT32_MAX, 10U },
    { "0xFF", UINT8_MAX, 4U },
    { "0xFFFF", UINT16_MAX, 6U },
    { "0xFFFFFFFF", UINT32_MAX, 10U },
    { "0x00", 0U, 4U },
    { "00",0U, 2U },
    { "0377", UINT8_MAX, 4U },
    { "0177777", UINT16_MAX, 7U },
    { "037777777777", UINT32_MAX, 12U },
  };
  basetype n;
  size_t pos, i;

  for (i = 0; i < sizeof(aSteps) / sizeof(tStep); i++)
  {
    n = util::stoui<basetype>(aSteps[i].str, &pos, 0);
    EXPECT_EQ(n, aSteps[i].n);
    EXPECT_EQ(pos, aSteps[i].pos);
  }
}

// -------------------------------------------------------------------------
void Ut_String_stoui_uint16_base0(void)
{
  using basetype = uint16;
  typedef struct
  {
    util::basic_string<16, char> str;
    basetype n;
    size_t pos;
  } tStep;
  static const tStep aSteps[] =
  {
    { "0", 0U, 1U },
    { "  0", 0U, 3U },
    { "1", 1U, 1U },
    { "  1", 1U, 3U },
    { "-1", 0U, 0U },
    { "-8000", 0U, 0U },
    { "255", UINT8_MAX, 3U },
    { "65535", UINT16_MAX, 5U },
    { "4294967295", 0U, 0U },
    { "0xFF", UINT8_MAX, 4U },
    { "0xFFFF", UINT16_MAX, 6U },
    { "0xFFFFFFFF", 0U, 0U },
    { "0x00", 0U, 4U },
    { "00",0U, 2U },
    { "0377", UINT8_MAX, 4U },
    { "0177777", UINT16_MAX, 7U },
    { "037777777777", 0U, 0U },
  };
  basetype n;
  size_t pos, i;

  for (i = 0; i < sizeof(aSteps) / sizeof(tStep); i++)
  {
    n = util::stoui<basetype>(aSteps[i].str, &pos, 0);
    EXPECT_EQ(n, aSteps[i].n);
    EXPECT_EQ(pos, aSteps[i].pos);
  }
}

// -------------------------------------------------------------------------
void Ut_String_stoui_uint8_base0(void)
{
  using basetype = uint8;
  typedef struct
  {
    util::basic_string<16, char> str;
    basetype n;
    size_t pos;
  } tStep;
  static const tStep aSteps[] =
  {
    { "0", 0U, 1U },
    { "  0", 0U, 3U },
    { "1", 1U, 1U },
    { "  1", 1U, 3U },
    { "-1", 0U, 0U },
    { "-8000", 0U, 0U },
    { "255", UINT8_MAX, 3U },
    { "65535", 0U, 0U },
    { "4294967295", 0U, 0U },
    { "0xFF", UINT8_MAX, 4U },
    { "0xFFFF", 0U, 0U },
    { "0xFFFFFFFF", 0U, 0U },
    { "0x00", 0U, 4U },
    { "00",0U, 2U },
    { "0377", UINT8_MAX, 4U },
    { "0177777", 0U, 0U },
    { "037777777777", 0U, 0U },
  };
  basetype n;
  size_t pos, i;

  for (i = 0; i < sizeof(aSteps) / sizeof(tStep); i++)
  {
    n = util::stoui<basetype>(aSteps[i].str, &pos, 0);
    EXPECT_EQ(n, aSteps[i].n);
    EXPECT_EQ(pos, aSteps[i].pos);
  }
}

// -------------------------------------------------------------------------
void Ut_String_stoui_uint8_base2(void)
{
  using basetype = uint8;
  typedef struct
  {
    util::basic_string<16, char> str;
    basetype n;
    size_t pos;
  } tStep;
  static const tStep aSteps[] =
  {
    {         "0",        0U, 1U },
    { "        0",        0U, 9U },
    { "        1",        1U, 9U },
    { "       11",        3U, 9U },
    { "      011",        3U, 9U },
    { "      111",        7U, 9U },
    { " 11111111", UINT8_MAX, 9U },
    { "011111111", UINT8_MAX, 9U },
    { "111111111",        0U, 0U },
    { "012111111",        1U, 2U },
  };
  basetype n;
  size_t pos, i;

  for (i = 0; i < sizeof(aSteps) / sizeof(tStep); i++)
  {
    n = util::stoui<basetype>(aSteps[i].str, &pos, 2);
    EXPECT_EQ(n, aSteps[i].n);
    EXPECT_EQ(pos, aSteps[i].pos);
  }
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

  RUN_TEST(Ut_String_Construct_empty);
  RUN_TEST(Ut_String_Construct_from_const_pointer);
  RUN_TEST(Ut_String_Construct_from_cost_pointer_empty);
  RUN_TEST(Ut_String_Construct_from_char);
  RUN_TEST(Ut_String_Assign_operator_string);
  RUN_TEST(Ut_String_Assign_operator_string_longer);
  RUN_TEST(Ut_String_Assign_operator_string_shorter);
  RUN_TEST(Ut_String_Assign_operator_string_4_string_8);
  RUN_TEST(Ut_String_Assign_operator_string_8_string_4);
  RUN_TEST(Ut_String_Assign_operator_string_string_empty);
  RUN_TEST(Ut_String_Assign_operator_string_empty_string);
  RUN_TEST(Ut_String_Assign_operator_const_pointer);
  RUN_TEST(Ut_String_Append_const_pointer);
  RUN_TEST(Ut_String_Append_const_pointer_with_clear);
  RUN_TEST(Ut_String_Append_string);
  RUN_TEST(Ut_String_Append_sub_string);
  RUN_TEST(Ut_String_Append_sub_const_pointer);
  RUN_TEST(Ut_String_operator_plus_equal_char_with_clear);
  RUN_TEST(Ut_String_Find_const_pointer_pos_0);
  RUN_TEST(Ut_String_Find_string_pos_0);
  RUN_TEST(Ut_String_Find_string_pos_0_negative);
  RUN_TEST(Ut_String_Find_const_pointer_pos_n);
  RUN_TEST(Ut_String_Find_string_pos_n);
  RUN_TEST(Ut_String_Find_string_pos_n_negative);
  RUN_TEST(Ut_String_Find_string_abcd_bc);
  RUN_TEST(Ut_String_Find_string_ababcd_bc);
  RUN_TEST(Ut_String_Find_string_abbdbbcd_bc);
  RUN_TEST(Ut_String_Find_string_abcd_empty);
  RUN_TEST(Ut_String_Find_const_pointer_abcd_bc);
  RUN_TEST(Ut_String_Find_const_pointer_ababcd_bc);
  RUN_TEST(Ut_String_Find_const_pointer_abbdbbcd_bc);
  RUN_TEST(Ut_String_Find_const_pointer_abcd_empty);
  RUN_TEST(Ut_String_Find_character);
  RUN_TEST(Ut_String_Compare_with_const_pointer);
  RUN_TEST(Ut_String_Compare_with_string);
  RUN_TEST(Ut_String_stoi_base0);
  RUN_TEST(Ut_String_stoi_base10);
  RUN_TEST(Ut_String_stoi_base16);
  RUN_TEST(Ut_String_stoui_uint32_base16);
  RUN_TEST(Ut_String_stoui_uint16_base16);
  RUN_TEST(Ut_String_stoui_uint8_base16);
  RUN_TEST(Ut_String_stoui_uint32_base10);
  RUN_TEST(Ut_String_stoui_uint16_base10);
  RUN_TEST(Ut_String_stoui_uint8_base10);
  RUN_TEST(Ut_String_stoui_uint32_base0);
  RUN_TEST(Ut_String_stoui_uint16_base0);
  RUN_TEST(Ut_String_stoui_uint8_base0);
  RUN_TEST(Ut_String_stoui_uint8_base2);

  (void) UNITY_END();

   // Return false to stop program execution (relevant on Windows)
  return false;
}
