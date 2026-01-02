/**
 * @file test.cpp
 *
 * @author Ralf Sondershaus
 *
 * Unity Test for Gen/Util/String_view.h
 */

#include <Unity_adapt.h>

#include <Util/String_view.h>
#include <stdint.h>

#define CFG_OFF 0
#define CFG_ON 1

// CFG_OFF = don't run tests with std::; typically on the target (such as Arduino Mega)
// CFG_ON = run tests with std::; typically on the host (such as Windows)
#ifdef ARDUINO
#define CFG_TEST_WITH_STD CFG_OFF
#else
#define CFG_TEST_WITH_STD CFG_ON
#endif

#if CFG_TEST_WITH_STD == CFG_ON
// Requires C++17
#include <string_view>
#include <iostream>
#include <array>
#include <algorithm>
#endif

// -------------------------------------------------------------------------
void Ut_String_view_Construct_empty(void)
{
    util::basic_string_view<char> sv;
    EXPECT_EQ(sv.empty(), true);
}

// -------------------------------------------------------------------------
void Ut_String_view_Construct_from_char_array_null_terminated(void)
{
    const char szArray[] = "abcd";
    util::basic_string_view<char> sv(szArray);
    EXPECT_EQ(sv.empty(), false);
    EXPECT_EQ(sv.size(), 4U);
    EXPECT_EQ(sv.length(), 4U);
    EXPECT_EQ(sv.front(), 'a');
    EXPECT_EQ(sv.back(), 'd');
}

// -------------------------------------------------------------------------
void Ut_String_view_Construct_from_char_array_with_count(void)
{
    const char szArray[] = "abcdefgh";
    util::basic_string_view<char> sv(szArray, 4);
    EXPECT_EQ(sv.empty(), false);
    EXPECT_EQ(sv.size(), 4U);
    EXPECT_EQ(sv.length(), 4U);
    EXPECT_EQ(sv.front(), 'a');
    EXPECT_EQ(sv.back(), 'd');
}

// -------------------------------------------------------------------------
void Ut_String_view_assignment1(void)
{
    const char szArray1[] = "abcdefgh";
    const char szArray2[] = "ijklm";
    util::basic_string_view<char> sv1(szArray1);
    util::basic_string_view<char> sv2(szArray2);
    EXPECT_EQ(sv1.empty(), false);
    EXPECT_EQ(sv2.empty(), false);
    EXPECT_EQ(sv1.size(), 8U);
    EXPECT_EQ(sv2.size(), 5U);
    EXPECT_EQ(sv1.length(), sv1.size());
    EXPECT_EQ(sv2.length(), sv2.size());
    sv1 = sv2;
    EXPECT_EQ(sv1.empty(), false);
    EXPECT_EQ(sv2.empty(), false);
    EXPECT_EQ(sv1.size(), 5U);
    EXPECT_EQ(sv2.size(), sv1.size());
    EXPECT_EQ(sv1.length(), sv1.size());
    EXPECT_EQ(sv2.length(), sv2.size());
    EXPECT_EQ(sv1.front(), 'i');
    EXPECT_EQ(sv1.back(), 'm');
    EXPECT_EQ(sv2.front(), 'i');
    EXPECT_EQ(sv2.back(), 'm');
}

// -------------------------------------------------------------------------
void Ut_String_view_assignment2(void)
{
    const char szArray1[] = "abcdefgh";
    const char szArray2[] = "ijklm";
    const char szArray3[] = "nopq";
    util::basic_string_view<char> sv1(szArray1);
    util::basic_string_view<char> sv2(szArray2);
    util::basic_string_view<char> sv3(szArray3);
    EXPECT_EQ(sv1.empty(), false);
    EXPECT_EQ(sv2.empty(), false);
    EXPECT_EQ(sv3.empty(), false);
    EXPECT_EQ(sv1.size(), 8U);
    EXPECT_EQ(sv2.size(), 5U);
    EXPECT_EQ(sv3.size(), 4U);
    EXPECT_EQ(sv1.length(), sv1.size());
    EXPECT_EQ(sv2.length(), sv2.size());
    EXPECT_EQ(sv3.length(), sv3.size());
    sv1 = sv2 = sv3;
    EXPECT_EQ(sv1.empty(), false);
    EXPECT_EQ(sv2.empty(), false);
    EXPECT_EQ(sv3.empty(), false);
    EXPECT_EQ(sv3.size(), 4U);
    EXPECT_EQ(sv1.size(), sv3.size());
    EXPECT_EQ(sv2.size(), sv3.size());
    EXPECT_EQ(sv1.front(), 'n');
    EXPECT_EQ(sv1.back(), 'q');
    EXPECT_EQ(sv2.front(), 'n');
    EXPECT_EQ(sv2.back(), 'q');
}

// -------------------------------------------------------------------------
void Ut_String_view_operator_at(void)
{
    const char szArray[] = "abcdefgh";
    util::basic_string_view<char> sv(szArray);
    EXPECT_EQ(sv.empty(), false);
    EXPECT_EQ(sv.size(), 8U);
    EXPECT_EQ(sv.length(), sv.size());
    EXPECT_EQ(sv[0], 'a');
    EXPECT_EQ(sv[1], 'b');
    EXPECT_EQ(sv[2], 'c');
    EXPECT_EQ(sv[3], 'd');
    EXPECT_EQ(sv[4], 'e');
    EXPECT_EQ(sv[5], 'f');
    EXPECT_EQ(sv[6], 'g');
    EXPECT_EQ(sv[7], 'h');
}

// -------------------------------------------------------------------------
void Ut_String_view_remove_prefix(void)
{
    const char szArray[] = "abcdefgh";
    util::basic_string_view<char> sv(szArray);
    EXPECT_EQ(sv.empty(), false);
    EXPECT_EQ(sv.size(), 8U);
    EXPECT_EQ(sv.length(), sv.size());
    sv.remove_prefix(4U);
    EXPECT_EQ(sv.size(), 4U);
    EXPECT_EQ(sv[0], 'e');
    EXPECT_EQ(sv[1], 'f');
    EXPECT_EQ(sv[2], 'g');
    EXPECT_EQ(sv[3], 'h');
}

// -------------------------------------------------------------------------
void Ut_String_view_remove_prefix_more_than_exist(void)
{
    const char szArray[] = "abcdefgh";
    util::basic_string_view<char> sv(szArray);
    EXPECT_EQ(sv.empty(), false);
    EXPECT_EQ(sv.size(), 8U);
    EXPECT_EQ(sv.length(), sv.size());
    sv.remove_prefix(10U);
    EXPECT_EQ(sv.size(), 0U);
    EXPECT_EQ(sv.empty(), true);
}

// -------------------------------------------------------------------------
void Ut_String_view_remove_suffix(void)
{
    const char szArray[] = "abcdefgh";
    util::basic_string_view<char> sv(szArray);
    EXPECT_EQ(sv.empty(), false);
    EXPECT_EQ(sv.size(), 8U);
    EXPECT_EQ(sv.length(), sv.size());
    sv.remove_suffix(4U);
    EXPECT_EQ(sv.size(), 4U);
    EXPECT_EQ(sv[0], 'a');
    EXPECT_EQ(sv[1], 'b');
    EXPECT_EQ(sv[2], 'c');
    EXPECT_EQ(sv[3], 'd');
}

// -------------------------------------------------------------------------
void Ut_String_view_remove_suffix_more_than_exist(void)
{
    const char szArray[] = "abcdefgh";
    util::basic_string_view<char> sv(szArray);
    EXPECT_EQ(sv.empty(), false);
    EXPECT_EQ(sv.size(), 8U);
    EXPECT_EQ(sv.length(), sv.size());
    sv.remove_suffix(10U);
    EXPECT_EQ(sv.size(), 0U);
    EXPECT_EQ(sv.empty(), true);
}

// -------------------------------------------------------------------------
void Ut_String_view_copy_pos0(void)
{
    const char szArray[] = "abcd";
    char dest[10];
    size_t res;
    util::basic_string_view<char> sv(szArray);
    EXPECT_EQ(sv.empty(), false);
    EXPECT_EQ(sv.size(), 4U);
    res = sv.copy(dest, sv.size(), 0U);
    EXPECT_EQ(res, 4U);
    EXPECT_EQ(sv.size(), 4U);
    EXPECT_EQ(sv.empty(), false);
    EXPECT_EQ(dest[0], 'a');
    EXPECT_EQ(dest[1], 'b');
    EXPECT_EQ(dest[2], 'c');
    EXPECT_EQ(dest[3], 'd');
}

// -------------------------------------------------------------------------
void Ut_String_view_copy_pos4_size_too_large(void)
{
    const char szArray[] = "abcdefgh";
    char dest[10];
    size_t res;
    util::basic_string_view<char> sv(szArray);
    EXPECT_EQ(sv.empty(), false);
    EXPECT_EQ(sv.size(), 8U);
    res = sv.copy(dest, sv.size(), 4U);
    EXPECT_EQ(res, sv.size() - 4U);
    EXPECT_EQ(sv.size(), 8U);
    EXPECT_EQ(sv.empty(), false);
    EXPECT_EQ(dest[0], 'e');
    EXPECT_EQ(dest[1], 'f');
    EXPECT_EQ(dest[2], 'g');
    EXPECT_EQ(dest[3], 'h');
}

// -------------------------------------------------------------------------
void Ut_String_view_copy_pos_too_large(void)
{
    const char szArray[] = "abcd";
    char dest[10];
    size_t res;
    util::basic_string_view<char> sv(szArray);
    EXPECT_EQ(sv.empty(), false);
    EXPECT_EQ(sv.size(), 4U);
    res = sv.copy(dest, sv.size(), sv.size() + 1);
    EXPECT_EQ(res, 0U);
    EXPECT_EQ(sv.size(), 4U);
    EXPECT_EQ(sv.empty(), false);
}

// -------------------------------------------------------------------------
void Ut_String_view_substr_pos0(void)
{
    const char szArray[] = "abcdefgh";
    util::basic_string_view<char> sv(szArray);
    EXPECT_EQ(sv.empty(), false);
    EXPECT_EQ(sv.size(), 8U);
    util::basic_string_view<char> sub = sv.substr(0, sv.size());
    EXPECT_EQ(sv.size(), 8U);
    EXPECT_EQ(sub.size(), sv.size());
    EXPECT_EQ(sv.compare(sub), 0);
}

// -------------------------------------------------------------------------
void Ut_String_view_compare_equal_0(void)
{
    const char szArray[] = "abcdefgh";
    util::basic_string_view<char> sv(szArray);
    EXPECT_EQ(sv.empty(), false);
    EXPECT_EQ(sv.size(), 8U);
    int res = sv.compare(util::basic_string_view<char>("abcdefgh"));
    EXPECT_EQ(res, 0);
    EXPECT_EQ(sv.size(), 8U);
}

// -------------------------------------------------------------------------
void Ut_String_view_compare_equal_0_example(void)
{
    util::basic_string_view<char> sv1("apple");
    util::basic_string_view<char> sv2("banana");
    util::basic_string_view<char> sv3("apple");
    util::basic_string_view<char> sv4("app");
    int result1 = sv1.compare(sv2); // Returns negative value (sv1 < sv2)
    int result2 = sv1.compare(sv3); // Returns 0 (sv1 == sv3)
    int result3 = sv2.compare(sv1); // Returns positive value (sv2 > sv1)
    int result4 = sv1.compare(sv4); // Returns positive value (sv1 > sv4, same prefix but sv1 is longer)
    EXPECT_EQ(result1 < 0, true);
    EXPECT_EQ(result2, 0);
    EXPECT_EQ(result3 > 0, true);
    EXPECT_EQ(result4 > 0, true);
}

// -------------------------------------------------------------------------
void Ut_String_view_compare_equal_minus1(void)
{
    const char szArray[] = "abcdefg";
    util::basic_string_view<char> sv(szArray);
    EXPECT_EQ(sv.empty(), false);
    EXPECT_EQ(sv.size(), 7U);
    int res = sv.compare(util::basic_string_view<char>("abcdefgh"));
    EXPECT_EQ(res, -1);
    EXPECT_EQ(sv.size(), 7U);
}

// -------------------------------------------------------------------------
void Ut_String_view_compare_equal_plus1(void)
{
    const char szArray[] = "abcdefgh";
    util::basic_string_view<char> sv(szArray);
    EXPECT_EQ(sv.empty(), false);
    EXPECT_EQ(sv.size(), 8U);
    int res = sv.compare(util::basic_string_view<char>("abcdefg"));
    EXPECT_EQ(res, 1);
    EXPECT_EQ(sv.size(), 8U);
}

// -------------------------------------------------------------------------
void Ut_String_view_compare_not_equal_one_letter_less(void)
{
    const char szArray[] = "a";
    int res;
    util::basic_string_view<char> sv(szArray);
    EXPECT_EQ(sv.empty(), false);
    EXPECT_EQ(sv.size(), 1U);
    res = sv.compare(util::basic_string_view<char>("b"));
    EXPECT_EQ(res, -1);
    EXPECT_EQ(sv.size(), 1U);
#if CFG_TEST_WITH_STD == CFG_ON
    std::string_view std_sv(szArray);
    EXPECT_EQ(std_sv.empty(), false);
    EXPECT_EQ(std_sv.size(), 1U);
    res = std_sv.compare(std::string_view("b"));
    EXPECT_EQ(res, -1);
    EXPECT_EQ(std_sv.size(), 1U);
#endif
}

// -------------------------------------------------------------------------
void Ut_String_view_compare_not_equal_8_letters_less(void)
{
    const char szArray[] = "abcdefgh";
    int res;
    util::basic_string_view<char> sv(szArray);
    EXPECT_EQ(sv.empty(), false);
    EXPECT_EQ(sv.size(), 8U);
    res = sv.compare(util::basic_string_view<char>("abcdefgi"));
    EXPECT_EQ(res, -1);
    EXPECT_EQ(sv.size(), 8U);
#if CFG_TEST_WITH_STD == CFG_ON
    std::string_view std_sv(szArray);
    EXPECT_EQ(std_sv.empty(), false);
    EXPECT_EQ(std_sv.size(), 8U);
    res = std_sv.compare(std::string_view("abcdefgi"));
    EXPECT_EQ(res, -1);
    EXPECT_EQ(std_sv.size(), 8U);
#endif
}

// -------------------------------------------------------------------------
void Ut_String_view_compare_not_equal_one_letter_greater(void)
{
    const char szArray[] = "b";
    int res;
    util::basic_string_view<char> sv(szArray);
    EXPECT_EQ(sv.empty(), false);
    EXPECT_EQ(sv.size(), 1U);
    res = sv.compare(util::basic_string_view<char>("a"));
    EXPECT_EQ(res, 1);
    EXPECT_EQ(sv.size(), 1U);
#if CFG_TEST_WITH_STD == CFG_ON
    std::string_view std_sv(szArray);
    EXPECT_EQ(std_sv.empty(), false);
    EXPECT_EQ(std_sv.size(), 1U);
    res = std_sv.compare(std::string_view("a"));
    EXPECT_EQ(res, 1);
    EXPECT_EQ(std_sv.size(), 1U);
#endif
}

// -------------------------------------------------------------------------
void Ut_String_view_compare_not_equal_8_letters_greater(void)
{
    const char szArray[] = "abcdefgh";
    int res;
    util::basic_string_view<char> sv(szArray);
    EXPECT_EQ(sv.empty(), false);
    EXPECT_EQ(sv.size(), 8U);
    res = sv.compare(util::basic_string_view<char>("abcdefgg"));
    EXPECT_EQ(res, 1);
    EXPECT_EQ(sv.size(), 8U);
#if CFG_TEST_WITH_STD == CFG_ON
    std::string_view std_sv(szArray);
    EXPECT_EQ(std_sv.empty(), false);
    EXPECT_EQ(std_sv.size(), 8U);
    res = std_sv.compare(std::string_view("abcdefgg"));
    EXPECT_EQ(res, 1);
    EXPECT_EQ(std_sv.size(), 8U);
#endif
}

// -------------------------------------------------------------------------
void Ut_String_view_compare_pos_cnt_sv(void)
{
    struct tStep
    {
        const char *szCmp1;
        const char *szCmp2;
        size_t pos1;
        size_t count1;
        int expectedRes;
    };

    struct tStep aSteps[] =
        {
            // szCmp1   , szCmp2    , pos1, count1, expectedRes
            {"abcdefgh", "abcdefgh", 0, 8, 0},
            {"abcdefgh", "bcdefgh", 1, 7, 0},
            {"abcdefgh", "bcdefgh", 1, 8, 0},
            {"abcdefgh", "abcdefgh", 1, 8, 1},
            {"abcdefgh", "def", 3, 3, 0},
            {"abcdefgh", "cdef", 3, 3, 1},
        };

    for (size_t i = 0; i < sizeof(aSteps) / (sizeof(struct tStep)); i++)
    {
        util::basic_string_view<char> sv1(aSteps[i].szCmp1);
        util::basic_string_view<char> sv2(aSteps[i].szCmp2);
        const size_t pos1 = aSteps[i].pos1;
        const size_t count1 = aSteps[i].count1;
        EXPECT_EQ(sv1.empty(), false);
        int res = sv1.compare(pos1, count1, sv2);
        EXPECT_EQ(res, aSteps[i].expectedRes);
#if CFG_TEST_WITH_STD == CFG_ON
        std::string_view std_sv1(aSteps[i].szCmp1);
        std::string_view std_sv2(aSteps[i].szCmp2);
        EXPECT_EQ(std_sv1.empty(), false);
        res = std_sv1.compare(pos1, count1, std_sv2);
        EXPECT_EQ(res, aSteps[i].expectedRes);
#endif
    }
}

// -------------------------------------------------------------------------
void Ut_String_view_compare_pos_cnt_sv_pos_cnt(void)
{
    struct tStep
    {
        const char *szCmp1;
        const char *szCmp2;
        size_t pos1;
        size_t count1;
        size_t pos2;
        size_t count2;
        int expectedRes;
    };

    struct tStep aSteps[] =
        {
            // szCmp1   , szCmp2    , pos1, count1, pos2, count2, expectedRes
            {"abcdefgh", "abcdefgh", 0, 8, 0, 8, 0},
            {"abcdefgh", "bcdefgh", 1, 7, 0, 7, 0},
            {"abcdefgh", "bcdefgh", 1, 8, 0, 7, 0},
            {"abcdefgh", "abcdefgh", 1, 8, 0, 8, 1},
            {"abcdefgh", "def", 3, 3, 0, 3, 0},
            {"def", "abcdefgh", 0, 3, 3, 3, 0},
            {"def", "abcdefgh", 0, 3, 3, 4, -1},
            {"def", "abcdefgh", 0, 3, 2, 3, 1},
            {"def", "abcdefgh", 0, 3, 4, 3, -1},
        };

    for (size_t i = 0; i < sizeof(aSteps) / (sizeof(struct tStep)); i++)
    {
        util::basic_string_view<char> sv1(aSteps[i].szCmp1);
        util::basic_string_view<char> sv2(aSteps[i].szCmp2);
        const size_t pos1 = aSteps[i].pos1;
        const size_t count1 = aSteps[i].count1;
        const size_t pos2 = aSteps[i].pos2;
        const size_t count2 = aSteps[i].count2;
        EXPECT_EQ(sv1.empty(), false);
        int res = sv1.compare(pos1, count1, sv2, pos2, count2);
        EXPECT_EQ(res, aSteps[i].expectedRes);
#if CFG_TEST_WITH_STD == CFG_ON
        std::string_view std_sv1(aSteps[i].szCmp1);
        std::string_view std_sv2(aSteps[i].szCmp2);
        EXPECT_EQ(std_sv1.empty(), false);
        res = std_sv1.compare(pos1, count1, std_sv2, pos2, count2);
        EXPECT_EQ(res, aSteps[i].expectedRes);
#endif
    }
}

// -------------------------------------------------------------------------
void Ut_String_view_starts_with_sv(void)
{
    struct tStep
    {
        const char *szCmp1;
        const char *szCmp2;
        bool expectedRes;
    };

    struct tStep aSteps[] =
        {
            // szCmp1   , szCmp2    , expectedRes
            {"abcdefgh", "a", true},
            {"abcdefgh", "ab", true},
            {"abcdefgh", "abc", true},
            {"abcdefgh", "abcd", true},
            {"abcdefgh", "abcde", true},
            {"abcdefgh", "abcdef", true},
            {"abcdefgh", "abcdefg", true},
            {"abcdefgh", "abcdefgh", true},
            {"abcdefgh", "abcdefghe", false},
            {"abcdefgh", "b", false},
            {"abcdefgh", "babc", false},
            {"abcdefgh", "abce", false},
        };

    for (size_t i = 0; i < sizeof(aSteps) / (sizeof(struct tStep)); i++)
    {
        util::basic_string_view<char> sv1(aSteps[i].szCmp1);
        util::basic_string_view<char> sv2(aSteps[i].szCmp2);
        bool res = sv1.starts_with(sv2); // C++20
        EXPECT_EQ(res, aSteps[i].expectedRes);
    }
}

// -------------------------------------------------------------------------
void Ut_String_view_starts_with_ch(void)
{
    struct tStep
    {
        const char *szCmp1;
        char ch;
        bool expectedRes;
    };

    struct tStep aSteps[] =
        {
            // szCmp1   , szCmp2    , expectedRes
            {"abcdefgh", 'a', true},
            {"abcdefgh", 'b', false},
            {"bcdefgh", 'b', true},
            {"bcdefgh", 'a', false},
            {"bcdefgh", 'c', false},
            {"bcdefgh", 'd', false},
            {"bcdefgh", 'e', false},
            {"bcdefgh", 'f', false},
            {"bcdefgh", 'g', false},
            {"bcdefgh", 'h', false},
            {"bcdefgh", 'i', false},
        };

    for (size_t i = 0; i < sizeof(aSteps) / (sizeof(struct tStep)); i++)
    {
        util::basic_string_view<char> sv1(aSteps[i].szCmp1);
        const char ch = aSteps[i].ch;
        bool res = sv1.starts_with(ch); // C++20
        EXPECT_EQ(res, aSteps[i].expectedRes);
    }
}

// -------------------------------------------------------------------------
void Ut_String_view_ends_with_sv(void)
{
    struct tStep
    {
        const char *szCmp1;
        const char *szCmp2;
        bool expectedRes;
    };

    struct tStep aSteps[] =
        {
            // szCmp1   , szCmp2    , expectedRes
            {"abcdefgh", "h", true},
            {"abcdefgh", "gh", true},
            {"abcdefgh", "fgh", true},
            {"abcdefgh", "efgh", true},
            {"abcdefgh", "defgh", true},
            {"abcdefgh", "cdefgh", true},
            {"abcdefgh", "bcdefgh", true},
            {"abcdefgh", "abcdefgh", true},
            {"abcdefgh", "abcdefghe", false},
            {"abcdefgh", "g", false},
            {"abcdefgh", "egh", false},
            {"abcdefgh", "fg", false},
        };

    for (size_t i = 0; i < sizeof(aSteps) / (sizeof(struct tStep)); i++)
    {
        util::basic_string_view<char> sv1(aSteps[i].szCmp1);
        util::basic_string_view<char> sv2(aSteps[i].szCmp2);
        bool res = sv1.ends_with(sv2); // C++20
        EXPECT_EQ(res, aSteps[i].expectedRes);
    }
}

// -------------------------------------------------------------------------
void Ut_String_view_ends_with_ch(void)
{
    struct tStep
    {
        const char *szCmp1;
        char ch;
        bool expectedRes;
    };

    struct tStep aSteps[] =
        {
            // szCmp1   , szCmp2    , expectedRes
            {"abcdefgh", 'h', true},
            {"abcdefgh", 'g', false},
            {"bcdefgh", 'h', true},
            {"bcdefgh", 'g', false},
            {"bcdefgh", 'f', false},
            {"bcdefgh", 'e', false},
            {"bcdefgh", 'd', false},
            {"bcdefgh", 'c', false},
            {"bcdefgh", 'b', false},
            {"bcdefgh", 'a', false},
            {"bcdefgh", 'i', false},
        };

    for (size_t i = 0; i < sizeof(aSteps) / (sizeof(struct tStep)); i++)
    {
        util::basic_string_view<char> sv1(aSteps[i].szCmp1);
        const char ch = aSteps[i].ch;
        bool res = sv1.ends_with(ch); // C++20
        EXPECT_EQ(res, aSteps[i].expectedRes);
    }
}

// -------------------------------------------------------------------------
void Ut_String_view_find_sv_pos(void)
{
    struct tStep
    {
        const char *szCmp1;
        const char *szCmp2;
        size_t pos1;
        size_t expectedRes;
    };

    struct tStep aSteps[] =
        {
            // szCmp1   , szCmp2    , pos1, expectedRes
            {"abcdefgh", "h", 0, 7},
            {"abcdefgh", "gh", 0, 6},
            {"abcdefgh", "fgh", 0, 5},
            {"abcdefgh", "efgh", 0, 4},
            {"abcdefgh", "defgh", 0, 3},
            {"abcdefgh", "cdefgh", 0, 2},
            {"abcdefgh", "bcdefgh", 0, 1},
            {"abcdefgh", "abcdefgh", 0, 0},
            {"abcdefgh", "abcdefghe", 0, static_cast<size_t>(-1)},
            {"abcdefgh", "g", 0, 6},
            {"abcdefgh", "egh", 0, static_cast<size_t>(-1)},
            {"abcdefgh", "fg", 0, 5},
            {"abcdefgh", "fg", 1, 5},
            {"abcdefgh", "fg", 2, 5},
            {"abcdefgh", "fg", 3, 5},
            {"abcdefgh", "fg", 4, 5},
            {"abcdefgh", "fg", 5, 5},
            {"abcdefgh", "fg", 6, static_cast<size_t>(-1)},
            {"abcdefgh", "fg", 7, static_cast<size_t>(-1)},
            {"abcdefgh", "fg", 8, static_cast<size_t>(-1)},
            {"abcdefgh", "fg", 9, static_cast<size_t>(-1)},
            {"abcdefghabcdefgh", "fg", 0, 5},
            {"abcdefghabcdefgh", "fg", 1, 5},
            {"abcdefghabcdefgh", "fg", 2, 5},
            {"abcdefghabcdefgh", "fg", 3, 5},
            {"abcdefghabcdefgh", "fg", 4, 5},
            {"abcdefghabcdefgh", "fg", 5, 5},
            {"abcdefghabcdefgh", "fg", 6, 13},
            {"abcdefghabcdefgh", "fg", 7, 13},
            {"abcdefghabcdefgh", "fg", 13, 13},
            {"abcdefghabcdefgh", "fg", 14, static_cast<size_t>(-1)},
        };

    for (size_t i = 0; i < sizeof(aSteps) / (sizeof(struct tStep)); i++)
    {
        util::basic_string_view<char> sv1(aSteps[i].szCmp1);
        util::basic_string_view<char> sv2(aSteps[i].szCmp2);
        const size_t pos1 = aSteps[i].pos1;
        size_t res = sv1.find(sv2, pos1);
        EXPECT_EQ(res, aSteps[i].expectedRes);
    }
}

// -------------------------------------------------------------------------
void Ut_String_view_find_first_of_sv_pos(void)
{
    struct tStep
    {
        const char *szCmp1;
        const char *szCmp2;
        size_t pos1;
        size_t expectedRes;
    };

    struct tStep aSteps[] =
        {
            // szCmp1   , szCmp2    , pos1, expectedRes
            {"abcdefgh", "h", 0, 7},
            {"abcdefgh", "gh", 0, 6},
            {"abcdefgh", "fgh", 0, 5},
            {"abcdefgh", "efgh", 0, 4},
            {"abcdefgh", "defgh", 0, 3},
            {"abcdefgh", "cdefgh", 0, 2},
            {"abcdefgh", "bcdefgh", 0, 1},
            {"abcdefgh", "abcdefgh", 0, 0},
            {"abcdefgh", "abcdefghe", 0, 0},
            {"abcdefgh", "ijk", 0, static_cast<size_t>(-1)},
            {"abcdefghabcdefgh", "fg", 1, 5},
            {"abcdefghabcdefgh", "fg", 2, 5},
            {"abcdefghabcdefgh", "fg", 3, 5},
            {"abcdefghabcdefgh", "fg", 4, 5},
            {"abcdefghabcdefgh", "fg", 5, 5},
            {"abcdefghabcdefgh", "fg", 6, 6},
            {"abcdefghabcdefgh", "fg", 7, 13},
            {"abcdefghabcdefgh", "fg", 13, 13},
            {"abcdefghabcdefgh", "fg", 14, 14},
            {"abcdefghabcdefgh", "fg", 15, static_cast<size_t>(-1)},
        };

    for (size_t i = 0; i < sizeof(aSteps) / (sizeof(struct tStep)); i++)
    {
        util::basic_string_view<char> sv1(aSteps[i].szCmp1);
        util::basic_string_view<char> sv2(aSteps[i].szCmp2);
        const size_t pos1 = aSteps[i].pos1;
        size_t res = sv1.find_first_of(sv2, pos1);
        EXPECT_EQ(res, aSteps[i].expectedRes);
    }
}

// -------------------------------------------------------------------------
void Ut_String_view_find_first_of_ch_pos(void)
{
    struct tStep
    {
        const char *szCmp1;
        char ch;
        size_t pos1;
        size_t expectedRes;
    };

    struct tStep aSteps[] =
        {
            // szCmp1   , ch    , pos1, expectedRes
            {"abcdefgh", 'h', 0, 7},
            {"abcdefgh", 'i', 0, static_cast<size_t>(-1)},
            {"abcdefghabcdefgh", 'f', 1, 5},
            {"abcdefghabcdefgh", 'f', 2, 5},
            {"abcdefghabcdefgh", 'f', 3, 5},
            {"abcdefghabcdefgh", 'f', 4, 5},
            {"abcdefghabcdefgh", 'f', 5, 5},
            {"abcdefghabcdefgh", 'f', 6, 13},
            {"abcdefghabcdefgh", 'f', 7, 13},
            {"abcdefghabcdefgh", 'f', 13, 13},
            {"abcdefghabcdefgh", 'f', 14, static_cast<size_t>(-1)},
            {"abcdefghabcdefgh", 'f', 15, static_cast<size_t>(-1)},
        };

    for (size_t i = 0; i < sizeof(aSteps) / (sizeof(struct tStep)); i++)
    {
        util::basic_string_view<char> sv1(aSteps[i].szCmp1);
        const char ch = aSteps[i].ch;
        const size_t pos1 = aSteps[i].pos1;
        size_t res = sv1.find_first_of(ch, pos1);
        EXPECT_EQ(res, aSteps[i].expectedRes);
    }
}

// -------------------------------------------------------------------------
void Ut_String_view_find_first_not_of_sv_pos(void)
{
    struct tStep
    {
        const char *szCmp1;
        const char *szCmp2;
        size_t pos1;
        size_t expectedRes;
    };

    struct tStep aSteps[] =
        {
            // szCmp1   , szCmp2    , pos1, expectedRes
            {"abcdefgh", "h", 0, 0},
            {"abcdefgh", "gh", 0, 0},
            {"abcdefgh", "fgh", 0, 0},
            {"abcdefgh", "efgh", 0, 0},
            {"abcdefgh", "defgh", 0, 0},
            {"abcdefgh", "cdefgh", 0, 0},
            {"abcdefgh", "bcdefgh", 0, 0},
            {"abcdefgh", "ab", 0, 2},
            {"abcdefgh", "abcdefgh", 0, static_cast<size_t>(-1)},
            {"abcdefgh", "abcdefghe", 0, static_cast<size_t>(-1)},
            {"abcdefgh", "ijk", 0, 0},
            {"abcdefghabcdefgh", "fg", 1, 1},
            {"abcdefghabcdefgh", "fg", 2, 2},
            {"abcdefghabcdefgh", "fg", 3, 3},
            {"abcdefghabcdefgh", "fg", 4, 4},
            {"abcdefghabcdefgh", "fg", 5, 7},
            {"abcdefghabcdefgh", "fg", 6, 7},
            {"abcdefghabcdefgh", "fg", 7, 7},
            {"abcdefghabcdefgh", "fg", 13, 15},
            {"abcdefghabcdefgh", "fg", 14, 15},
            {"abcdefghabcdefgh", "fg", 15, 15},
        };

    for (size_t i = 0; i < sizeof(aSteps) / (sizeof(struct tStep)); i++)
    {
        util::basic_string_view<char> sv1(aSteps[i].szCmp1);
        util::basic_string_view<char> sv2(aSteps[i].szCmp2);
        const size_t pos1 = aSteps[i].pos1;
        size_t res = sv1.find_first_not_of(sv2, pos1);
        EXPECT_EQ(res, aSteps[i].expectedRes);
#if CFG_TEST_WITH_STD == CFG_ON
        std::string_view std_sv1(aSteps[i].szCmp1);
        std::string_view std_sv2(aSteps[i].szCmp2);
        res = std_sv1.find_first_not_of(std_sv2, pos1);
        EXPECT_EQ(res, aSteps[i].expectedRes);
#endif
    }
}

// -------------------------------------------------------------------------
void Ut_String_view_find_first_not_of_ch_pos(void)
{
    struct tStep
    {
        const char *szCmp1;
        char ch;
        size_t pos1;
        size_t expectedRes;
    };

    struct tStep aSteps[] =
        {
            // szCmp1   , ch    , pos1, expectedRes
            {"abcdefgh", 'h', 0, 0},
            {"abcdefgh", 'i', 0, 0},
            {"abcdefghabcdefgh", 'f', 1, 1},
            {"abcdefghabcdefgh", 'f', 2, 2},
            {"abcdefghabcdefgh", 'f', 3, 3},
            {"abcdefghabcdefgh", 'f', 4, 4},
            {"abcdefghabcdefgh", 'f', 5, 6},
            {"abcdefghabcdefgh", 'f', 6, 6},
            {"abcdefghabcdefgh", 'f', 7, 7},
            {"abcdefghabcdefgh", 'f', 13, 14},
            {"abcdefghabcdefgh", 'f', 14, 14},
            {"abcdefghabcdefgh", 'f', 15, 15},
            {"        abcdefgh", ' ', 0, 8},
        };

    for (size_t i = 0; i < sizeof(aSteps) / (sizeof(struct tStep)); i++)
    {
        util::basic_string_view<char> sv1(aSteps[i].szCmp1);
        const char ch = aSteps[i].ch;
        const size_t pos1 = aSteps[i].pos1;
        size_t res = sv1.find_first_not_of(ch, pos1);
        EXPECT_EQ(res, aSteps[i].expectedRes);
#if CFG_TEST_WITH_STD == CFG_ON
        std::string_view std_sv1(aSteps[i].szCmp1);
        res = std_sv1.find_first_not_of(ch, pos1);
        EXPECT_EQ(res, aSteps[i].expectedRes);
#endif
    }
}

// -------------------------------------------------------------------------
void setUp(void)
{
}

// -------------------------------------------------------------------------
void tearDown(void)
{
}

// -------------------------------------------------------------------------
void test_setup(void)
{
}

// -------------------------------------------------------------------------
bool test_loop(void)
{
    UNITY_BEGIN();

    RUN_TEST(Ut_String_view_Construct_empty);
    RUN_TEST(Ut_String_view_Construct_from_char_array_null_terminated);
    RUN_TEST(Ut_String_view_Construct_from_char_array_with_count);
    RUN_TEST(Ut_String_view_assignment1);
    RUN_TEST(Ut_String_view_assignment2);
    RUN_TEST(Ut_String_view_operator_at);
    RUN_TEST(Ut_String_view_remove_prefix);
    RUN_TEST(Ut_String_view_remove_prefix_more_than_exist);
    RUN_TEST(Ut_String_view_remove_suffix);
    RUN_TEST(Ut_String_view_remove_suffix_more_than_exist);
    RUN_TEST(Ut_String_view_copy_pos0);
    RUN_TEST(Ut_String_view_copy_pos4_size_too_large);
    RUN_TEST(Ut_String_view_copy_pos_too_large);
    RUN_TEST(Ut_String_view_substr_pos0);
    RUN_TEST(Ut_String_view_compare_equal_0);
    RUN_TEST(Ut_String_view_compare_equal_0_example);
    RUN_TEST(Ut_String_view_compare_equal_minus1);
    RUN_TEST(Ut_String_view_compare_equal_plus1);
    RUN_TEST(Ut_String_view_compare_not_equal_one_letter_less);
    RUN_TEST(Ut_String_view_compare_not_equal_8_letters_less);
    RUN_TEST(Ut_String_view_compare_not_equal_one_letter_greater);
    RUN_TEST(Ut_String_view_compare_not_equal_8_letters_greater);
    RUN_TEST(Ut_String_view_compare_pos_cnt_sv);
    RUN_TEST(Ut_String_view_compare_pos_cnt_sv_pos_cnt);
    RUN_TEST(Ut_String_view_starts_with_sv);
    RUN_TEST(Ut_String_view_starts_with_ch);
    RUN_TEST(Ut_String_view_ends_with_sv);
    RUN_TEST(Ut_String_view_ends_with_ch);
    RUN_TEST(Ut_String_view_find_sv_pos);
    RUN_TEST(Ut_String_view_find_first_of_sv_pos);
    RUN_TEST(Ut_String_view_find_first_of_ch_pos);
    RUN_TEST(Ut_String_view_find_first_not_of_sv_pos);
    RUN_TEST(Ut_String_view_find_first_not_of_ch_pos);

    return UNITY_END();
}
