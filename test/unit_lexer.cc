#include "detail/lexer.hpp"

#include <gtest/gtest.h>

// 测试 lexer 用的宏
#define TEST_LEXER_BASE(json, type)                                            \
    microlife::detail::lexer lex;                                              \
    microlife::detail::json_t::string_t str;                                   \
    str = json;                                                                \
    lex.init(str.begin(), str.end());                                          \
    EXPECT_EQ(microlife::detail::lexer::token_t::type, lex.scan());

#define TEST_LEXER_TYPE(json, type)                                            \
    do {                                                                       \
        TEST_LEXER_BASE(json, type)                                            \
    } while (0)

#define TEST_LEXER_ERROR(json) TEST_LEXER_TYPE(json, parse_error)

// test null
#define TEST_LEXER_NULL(json) TEST_LEXER_TYPE(json, literal_null)

TEST(lexer, null) {
    // null
    TEST_LEXER_NULL("null");
    TEST_LEXER_NULL(" null ");
    TEST_LEXER_NULL("\nnull\t");

    // error
    TEST_LEXER_ERROR(" nulll ");
    TEST_LEXER_ERROR(" nu ll ");
    TEST_LEXER_ERROR("n");
    TEST_LEXER_ERROR("nul");
    TEST_LEXER_ERROR("unull");
    TEST_LEXER_ERROR("nnull");
    TEST_LEXER_ERROR("nulll");
}

// test boolean
#define TEST_LEXER_TRUE(json) TEST_LEXER_TYPE(json, literal_true)
#define TEST_LEXER_FALSE(json) TEST_LEXER_TYPE(json, literal_false)

TEST(lexer, boolean) {
    // true
    TEST_LEXER_TRUE("true");
    TEST_LEXER_TRUE(" true ");
    TEST_LEXER_TRUE("\ntrue\t");

    // false
    TEST_LEXER_FALSE("false");
    TEST_LEXER_FALSE(" false ");
    TEST_LEXER_FALSE("\nfalse\t");

    // error
    TEST_LEXER_ERROR(" truee ");
    TEST_LEXER_ERROR(" tre ");
    TEST_LEXER_ERROR(" tru ");
    TEST_LEXER_ERROR(" rue ");
    TEST_LEXER_ERROR(" falsee ");
    TEST_LEXER_ERROR("faase");
    TEST_LEXER_ERROR(" fa ");
}

// test string
#define TEST_LEXER_STRING(value, json)                                         \
    do {                                                                       \
        TEST_LEXER_BASE(json, value_string)                                    \
        EXPECT_EQ(value, lex.get_string());                                    \
    } while (0)

TEST(lexer, string) {
    // string
    TEST_LEXER_STRING("hello", R"("hello")");
    TEST_LEXER_STRING("\" \\ / \b \f \n \r \t",
                      "\"\\\" \\\\ / \\b \\f \\n \\r \\t\""); // 与下面语句相同
    TEST_LEXER_STRING("\" \\ / \b \f \n \r \t", R"("\" \\ / \b \f \n \r \t")");
    TEST_LEXER_STRING("Hello\nWorld", R"("Hello\nWorld")");

    // 缺失右引号
    TEST_LEXER_ERROR("\"");
    TEST_LEXER_ERROR("\"abc");

    // 无效的转义字符
    TEST_LEXER_ERROR("\"\\v\"");
    TEST_LEXER_ERROR("\"\\'\"");
    TEST_LEXER_ERROR("\"\\0\"");
    TEST_LEXER_ERROR("\"\\x12\"");

    // 无效的字符
    TEST_LEXER_ERROR("\"\x01\"");
    TEST_LEXER_ERROR("\"\x1F\"");

    // utf-8
    TEST_LEXER_STRING(std::string("Hello") + '\0' + "World",
                      "\"Hello\\u0000World\"");
    TEST_LEXER_STRING("\x24", "\"\\u0024\"");         // Dollar sign U+0024
    TEST_LEXER_STRING("\xC2\xA2", "\"\\u00A2\"");     // Cents sign U+00A2
    TEST_LEXER_STRING("\xE2\x82\xAC", "\"\\u20AC\""); // Euro sign U+20AC
    TEST_LEXER_STRING("\xF0\x9D\x84\x9E",
                      "\"\\uD834\\uDD1E\""); // G clef sign U+1D11E
    TEST_LEXER_STRING("\xF0\x9D\x84\x9E",
                      "\"\\ud834\\udd1e\""); // G clef sign U+1D11E

    // 无效的 utf-8
    TEST_LEXER_ERROR("\"\\u\"");
    TEST_LEXER_ERROR("\"\\u0\"");
    TEST_LEXER_ERROR("\"\\u01\"");
    TEST_LEXER_ERROR("\"\\u012\"");
    TEST_LEXER_ERROR("\"\\u/000\"");
    TEST_LEXER_ERROR("\"\\uG000\"");
    TEST_LEXER_ERROR("\"\\u0/00\"");
    TEST_LEXER_ERROR("\"\\u0G00\"");
    TEST_LEXER_ERROR("\"\\u00/0\"");
    TEST_LEXER_ERROR("\"\\u00G0\"");
    TEST_LEXER_ERROR("\"\\u000/\"");
    TEST_LEXER_ERROR("\"\\u000G\"");
    TEST_LEXER_ERROR("\"\\u 123\"");
    TEST_LEXER_ERROR("\"\\uD800\\u0/00\"");

    // 无效的 utf-8 代理
    TEST_LEXER_ERROR("\"\\uD800\"");
    TEST_LEXER_ERROR("\"\\uDBFF\"");
    TEST_LEXER_ERROR("\"\\uD800\\\\\"");
    TEST_LEXER_ERROR("\"\\uD800\\uDBFF\"");
    TEST_LEXER_ERROR("\"\\uD800\\uE000\"");
}

// test number
#define TEST_LEXER_NUMBER(value, json)                                         \
    do {                                                                       \
        TEST_LEXER_BASE(json, value_number)                                    \
        EXPECT_EQ(value, lex.get_number());                                    \
    } while (0)

TEST(lexer, number) {
    // number
    TEST_LEXER_NUMBER(123, "123");
    TEST_LEXER_NUMBER(0.0, "0");
    TEST_LEXER_NUMBER(0.0, "-0");
    TEST_LEXER_NUMBER(0.0, "-0.0");
    TEST_LEXER_NUMBER(1.0, "1");
    TEST_LEXER_NUMBER(-1.0, "-1");
    TEST_LEXER_NUMBER(1.5, "1.5");
    TEST_LEXER_NUMBER(-1.5, "-1.5");
    TEST_LEXER_NUMBER(3.1416, "3.1416");
    TEST_LEXER_NUMBER(1E10, "1E10");
    TEST_LEXER_NUMBER(1e10, "1e10");
    TEST_LEXER_NUMBER(1E+10, "1E+10");
    TEST_LEXER_NUMBER(1E-10, "1E-10");
    TEST_LEXER_NUMBER(-1E10, "-1E10");
    TEST_LEXER_NUMBER(-1e10, "-1e10");
    TEST_LEXER_NUMBER(-1E+10, "-1E+10");
    TEST_LEXER_NUMBER(-1E-10, "-1E-10");
    TEST_LEXER_NUMBER(1.234E+10, "1.234E+10");
    TEST_LEXER_NUMBER(1.234E-10, "1.234E-10");
    TEST_LEXER_NUMBER(0.0, "1e-10000"); /* must underflow */
    TEST_LEXER_NUMBER(1234, "1234");

    TEST_LEXER_ERROR("+0");
    TEST_LEXER_ERROR("+1");
    TEST_LEXER_ERROR("-a");
    TEST_LEXER_ERROR(".123");
    TEST_LEXER_ERROR("1.");
    TEST_LEXER_ERROR("INF");
    TEST_LEXER_ERROR("inf");
    TEST_LEXER_ERROR("NAN");
    TEST_LEXER_ERROR("nan");
    TEST_LEXER_ERROR("+0e");
    TEST_LEXER_ERROR("1E");

    TEST_LEXER_ERROR("0123");
    TEST_LEXER_ERROR("0x0");
    TEST_LEXER_ERROR("0x123");

    // 边界测试
    // https://en.wikipedia.org/wiki/Double-precision_floating-point_format#Double-precision_examples
    TEST_LEXER_NUMBER(1.0000000000000002,
                      "1.0000000000000002"); /* the smallest number > 1 */
    TEST_LEXER_NUMBER(4.9406564584124654e-324,
                      "4.9406564584124654e-324"); /* minimum denormal */
    TEST_LEXER_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
    TEST_LEXER_NUMBER(2.2250738585072009e-308,
                      "2.2250738585072009e-308"); /* Max subnormal double */
    TEST_LEXER_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    TEST_LEXER_NUMBER(
        2.2250738585072014e-308,
        "2.2250738585072014e-308"); /* Min normal positive double */
    TEST_LEXER_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    TEST_LEXER_NUMBER(1.7976931348623157e+308,
                      "1.7976931348623157e+308"); /* Max double */
    TEST_LEXER_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");

    // too big
    TEST_LEXER_ERROR("1e309");
    TEST_LEXER_ERROR("-1e309");
}

// test other
TEST(lexer, other) {
    TEST_LEXER_TYPE(" [ ", begin_array);
    TEST_LEXER_TYPE(" ] ", end_array);
    TEST_LEXER_TYPE(" { ", begin_object);
    TEST_LEXER_TYPE(" } ", end_object);
    TEST_LEXER_TYPE(" : ", name_separator);
    TEST_LEXER_TYPE(" , ", value_separator);

    // end of input
    TEST_LEXER_TYPE("", end_of_input);
    TEST_LEXER_TYPE(" ", end_of_input);
    TEST_LEXER_TYPE("\t", end_of_input);
    TEST_LEXER_TYPE("\n", end_of_input);
    TEST_LEXER_TYPE(" \t \n\t\t ", end_of_input);

    TEST_LEXER_TYPE(" # ", parse_error);
    TEST_LEXER_TYPE("?", parse_error);
    TEST_LEXER_TYPE(" ?", parse_error);
    TEST_LEXER_TYPE(" ?  ", parse_error);
}

namespace {
using token_t = microlife::detail::lexer::token_t;

TEST(lexer, complex) {
    microlife::detail::lexer lex;
    microlife::detail::json_t::string_t str;

    str = " null \n true \t false [ ] { } , : 123 \"hello\"";
    lex.init(str.begin(), str.end());

    EXPECT_EQ(token_t::literal_null, lex.scan());
    EXPECT_EQ(token_t::literal_true, lex.scan());
    EXPECT_EQ(token_t::literal_false, lex.scan());

    EXPECT_EQ(token_t::begin_array, lex.scan());
    EXPECT_EQ(token_t::end_array, lex.scan());
    EXPECT_EQ(token_t::begin_object, lex.scan());
    EXPECT_EQ(token_t::end_object, lex.scan());
    EXPECT_EQ(token_t::value_separator, lex.scan());
    EXPECT_EQ(token_t::name_separator, lex.scan());

    EXPECT_EQ(token_t::value_number, lex.scan());
    EXPECT_EQ(123, lex.get_number());

    EXPECT_EQ(token_t::value_string, lex.scan());
    EXPECT_EQ("hello", lex.get_string());
}
} // namespace