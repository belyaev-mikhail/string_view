#include <sstream>

#include <gtest/gtest.h>
#include "string_view.hpp"

namespace {
    using namespace essentials;

    TEST(string_view, sanity_check) {
        std::string s0 = "hello";
        std::string s1 = "world";
        ASSERT_NE(string_view(s0), string_view(s1));
    }

    TEST(string_view, simple) {
        std::string s = "Hello world";
        string_view vs = s;

        ASSERT_EQ(s.substr(2, 5), vs.substr(2, 5));
        ASSERT_EQ(11, vs.size());
        ASSERT_EQ('e', vs[1]);
    }

    TEST(string_view, empty) {
        ASSERT_EQ(string_view(), string_view(""));
        ASSERT_EQ(string_view(), std::string());
        ASSERT_EQ(string_view(), string_view("Hello").substr(4, 0));
        ASSERT_TRUE(string_view("hello").substr(3, 0).empty());

        ASSERT_THROW(string_view().at(1), std::out_of_range);
    }

    TEST(string_view, comparisons) {
        ASSERT_TRUE(string_view("") < string_view(" "));
        ASSERT_TRUE(string_view("hel") < string_view("hello"));
        ASSERT_TRUE(string_view("abc") < string_view("abd"));
        ASSERT_TRUE(string_view("abc") < string_view("abcd"));
        ASSERT_TRUE(string_view("abcdefgh") < string_view("abd"));

#define TEST_CASE(A, B) {    \
            std::string sA = A;    \
            std::string sB = B;    \
            string_view vsA = A;     \
            string_view vsB = B;     \
            ASSERT_EQ(sA < sB, vsA < vsB);      \
            ASSERT_EQ(sA <= sB, vsA <= vsB);    \
            ASSERT_EQ(sA > sB, vsA > vsB);      \
            ASSERT_EQ(sA >= sB, vsA >= vsB);    \
            ASSERT_EQ(sA == sB, vsA == vsB);    \
            ASSERT_EQ(sA != sB, vsA != vsB);    \
        }

        TEST_CASE("", "");
        TEST_CASE("A", "B");
        TEST_CASE("hello", "world");
        TEST_CASE("hello", "hello");
        TEST_CASE("hello", "hello!");
        TEST_CASE("  ", "   ");
#undef TEST_CASE
    }

    TEST(string_view, std_string_equivalence) {
#define TEST_CASE(...) { \
            std::string s = "Hello world"; \
            string_view vs = "Hello world$$$"; \
            vs.remove_suffix(3); \
            ASSERT_EQ(s.__VA_ARGS__, vs.__VA_ARGS__); \
        }

        TEST_CASE(size());

        TEST_CASE(substr(3));
        TEST_CASE(substr(0));
        TEST_CASE(substr(s.size()));

        ASSERT_THROW(string_view("aaa").substr(4), std::out_of_range);
        ASSERT_THROW(std::string("aaa").substr(4), std::out_of_range);

        TEST_CASE(substr(2, 2));
        TEST_CASE(substr(2, 0));
        TEST_CASE(substr(2, 40));

        TEST_CASE(at(4))
        TEST_CASE(at(0))
        TEST_CASE(at(s.size() - 1))
        ASSERT_THROW(string_view("aaa").at(4), std::out_of_range);
        ASSERT_THROW(std::string("aaa").at(4), std::out_of_range);

        TEST_CASE(find(""));
        TEST_CASE(find("", 8));
        TEST_CASE(find("el"));
        TEST_CASE(find("He"));
        TEST_CASE(find("orld"));
        TEST_CASE(find("zz"));
        TEST_CASE(find("llop"));
        TEST_CASE(find("o", 5));

        TEST_CASE(rfind(""));
        TEST_CASE(rfind("", 8));
        TEST_CASE(rfind("el"));
        TEST_CASE(rfind("He"));
        TEST_CASE(rfind("orld"));
        TEST_CASE(rfind("zz"));
        TEST_CASE(rfind("llop"));
        TEST_CASE(rfind("o", 5));

        TEST_CASE(find_first_of("", 8));
        TEST_CASE(find_first_of(""));
        TEST_CASE(find_first_of("el"));
        TEST_CASE(find_first_of("He"));
        TEST_CASE(find_first_of("orld"));
        TEST_CASE(find_first_of("zz"));
        TEST_CASE(find_first_of("llop"));
        TEST_CASE(find_first_of("o", 5));
        TEST_CASE(find_first_of("old", 4));

        TEST_CASE(find_last_of("", 8));
        TEST_CASE(find_last_of(""));
        TEST_CASE(find_last_of("el"));
        TEST_CASE(find_last_of("He"));
        TEST_CASE(find_last_of("orld"));
        TEST_CASE(find_last_of("zz"));
        TEST_CASE(find_last_of("llop"));
        TEST_CASE(find_last_of("o", 5));
        TEST_CASE(find_last_of("old", 4));

        TEST_CASE(find_first_not_of("", 8));
        TEST_CASE(find_first_not_of(""));
        TEST_CASE(find_first_not_of("el"));
        TEST_CASE(find_first_not_of("He"));
        TEST_CASE(find_first_not_of("orld"));
        TEST_CASE(find_first_not_of("zz"));
        TEST_CASE(find_first_not_of("llop"));
        TEST_CASE(find_first_not_of("o", 5));
        TEST_CASE(find_first_not_of("zz", 4));
        TEST_CASE(find_first_not_of("zz", 11));

        TEST_CASE(find_last_not_of("", 8));
        TEST_CASE(find_last_not_of(""));
        TEST_CASE(find_last_not_of("el"));
        TEST_CASE(find_last_not_of("He"));
        TEST_CASE(find_last_not_of("orld"));
        TEST_CASE(find_last_not_of("zz"));
        TEST_CASE(find_last_not_of("llop"));
        TEST_CASE(find_last_not_of("o", 5));
        TEST_CASE(find_last_not_of("zz", 4));
        TEST_CASE(find_last_not_of("zz", 11));
#undef TEST_CASE
    }

    TEST(string_view, io) {
#define TEST_CASE(S, W, F, L) {    \
            std::stringstream ost; \
            std::stringstream ostv; \
            std::string s = S; \
            string_view vs = S; \
            ost.width(W); \
            ostv.width(W); \
            if(L) ost << std::left; \
            if(L) ostv << std::left; \
            ost.fill(F); \
            ostv.fill(F); \
            ost << s; \
            ostv << vs; \
            ASSERT_EQ(ost.str(), ostv.str()); \
            ASSERT_EQ(ost.width(), ostv.width()); \
        }

        TEST_CASE("", 5, '8', true);
        TEST_CASE("", 5, '8', false);
        TEST_CASE("hello world", 50, '8', true);
        TEST_CASE("hello world", 50, '8', false);
        TEST_CASE("so wow", 6, '8', true);
        TEST_CASE("so wow", 6, '8', false);
        TEST_CASE("so wow", 4, '8', true);
        TEST_CASE("so wow", 4, '8', false);
        TEST_CASE("so wow", 0, '8', true);
        TEST_CASE("so wow", 0, '8', false);
        TEST_CASE("so wow", 1, '8', true);
        TEST_CASE("so wow", 1, '8', false);

#undef TEST_CASE
    }

    TEST(string_view, iterators) {
#define TEST_CASE(S) {    \
            string_view vs = S; \
            std::string s(vs.begin(), vs.end()); \
            ASSERT_EQ(vs, s); \
        }

        TEST_CASE("");
        TEST_CASE("1");
        TEST_CASE("hello world");

#undef TEST_CASE
#define TEST_CASE(S, REV) {    \
            string_view vs = S; \
            std::string s(vs.rbegin(), vs.rend()); \
            ASSERT_EQ(std::string(REV), s); \
        }

        TEST_CASE("", "");
        TEST_CASE("1", "1");
        TEST_CASE("hello world", "dlrow olleh");

#undef TEST_CASE
    }

    TEST(string_view, trim) {
#define TEST_CASE(S, T) {    \
            string_view vs = S; \
            vs.remove_prefix(T); \
            ASSERT_EQ(vs, std::string(S).substr(T)); \
        } \
        {    \
            string_view vs = S; \
            vs.remove_suffix(T); \
            ASSERT_EQ(vs, std::string(S).substr(0, sizeof(S) - T - 1)); \
        }

        TEST_CASE("", 0);
        TEST_CASE("1", 0);
        TEST_CASE("hello world", 0);
        TEST_CASE("hello world", 1);
        TEST_CASE("hello world", 5);
        TEST_CASE("hello world", 11);

#undef TEST_CASE
    }

    TEST(string_view, copy) {
#define TEST_CASE(S, C) { \
            char arr[256]; \
            string_view vs = S; \
            ASSERT_EQ(vs.size(), sizeof(S) - 1); \
            auto cc = vs.copy(arr, C); \
            ASSERT_EQ(cc, std::min(size_t(C), vs.size())); \
            arr[cc] = 0; \
            ASSERT_EQ(vs.substr(0, cc), string_view(arr)); \
        }

        TEST_CASE("", 0);
        TEST_CASE("1", 0);
        TEST_CASE("hello world", 0);
        TEST_CASE("hello world", 1);
        TEST_CASE("hello world", 5);
        TEST_CASE("hello world", 11);
        TEST_CASE("hello world", 15);

#undef TEST_CASE
    }

}