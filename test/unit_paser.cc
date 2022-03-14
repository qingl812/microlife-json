#include "unit_depend.hpp"

// 测试 parse_t parse(); 函数返回值
// 以及其 value.type 是否符合预期
#define TEST_RETURN(json, ret, type)                                           \
    do {                                                                       \
        microlife::detail::value_s value;                                      \
        EXPECT_EQ(microlife::detail::parse_t::ret,                             \
                  microlife::detail::parser::parse(&value, json));             \
        EXPECT_EQ(microlife::detail::type_t::type,                             \
                  microlife::detail::parser::get_type(&value));                \
    } while (0)

// 带有 VALUE 的测试
#define TEST_VALUE(expected, json, type, get_function)                         \
    do {                                                                       \
        microlife::detail::value_s value;                                      \
        EXPECT_EQ(microlife::detail::parse_t::ok,                              \
                  microlife::detail::parser::parse(&value, json));             \
        EXPECT_EQ(microlife::detail::type_t::type,                             \
                  microlife::detail::parser::get_type(&value));                \
        EXPECT_EQ(expected, microlife::detail::parser::get_function(&value));  \
    } while (0)

// 返回 parse_t::expect_value 的测试
#define TEST_EXPECT(json) TEST_RETURN(json, expect_value, null)
TEST(parser, expect) {
    TEST_EXPECT("");
    TEST_EXPECT(" ");
    TEST_EXPECT("\t");
    TEST_EXPECT("\n");
    TEST_EXPECT(" \t \n\t\t ");
}

// invalid
#define TEST_INVALID(json) TEST_RETURN(json, invalid_value, null)
TEST(parser, invalid) {
    TEST_INVALID("?");
    TEST_INVALID(" ?");
    TEST_INVALID(" ?  ");
}

// root_not_singular
#define TEST_ROOT_NOT_SINGULAR(json) TEST_RETURN(json, root_not_singular, null)
TEST(parser, root_not_singular) {
    TEST_ROOT_NOT_SINGULAR("null x");
    TEST_ROOT_NOT_SINGULAR("  null  x  ");
}

// null
#define TEST_NULL(json) TEST_RETURN(json, ok, null)
TEST(parser, null) {
    TEST_NULL("null");
    TEST_NULL(" null ");

    TEST_INVALID("n");
    TEST_INVALID("nul");
    TEST_INVALID("unull");
    TEST_INVALID("nnull");

    TEST_ROOT_NOT_SINGULAR("nulll");
}

// boolean
#define TEST_BOOLEAN(expected, json)                                           \
    TEST_VALUE(expected, json, boolean, get_boolean)
TEST(parser, boolean) {
    TEST_BOOLEAN(true, "true");
    TEST_BOOLEAN(false, "false");
    TEST_BOOLEAN(true, "true");

    TEST_INVALID("faase");

    TEST_ROOT_NOT_SINGULAR("truee");
}

// number 的测试
#define TEST_NUMBER(expected, json)                                            \
    TEST_VALUE(expected, json, number, get_number)
#define TEST_NUMBER_TOO_BIG(json) TEST_RETURN(json, number_too_big, null)
TEST(parser, number) {
    TEST_NUMBER(0.0, "0");
    TEST_NUMBER(0.0, "-0");
    TEST_NUMBER(0.0, "-0.0");
    TEST_NUMBER(1.0, "1");
    TEST_NUMBER(-1.0, "-1");
    TEST_NUMBER(1.5, "1.5");
    TEST_NUMBER(-1.5, "-1.5");
    TEST_NUMBER(3.1416, "3.1416");
    TEST_NUMBER(1E10, "1E10");
    TEST_NUMBER(1e10, "1e10");
    TEST_NUMBER(1E+10, "1E+10");
    TEST_NUMBER(1E-10, "1E-10");
    TEST_NUMBER(-1E10, "-1E10");
    TEST_NUMBER(-1e10, "-1e10");
    TEST_NUMBER(-1E+10, "-1E+10");
    TEST_NUMBER(-1E-10, "-1E-10");
    TEST_NUMBER(1.234E+10, "1.234E+10");
    TEST_NUMBER(1.234E-10, "1.234E-10");
    TEST_NUMBER(0.0, "1e-10000"); /* must underflow */
    TEST_NUMBER(1234, "1234");

    TEST_INVALID("+0");
    TEST_INVALID("+1");
    TEST_INVALID(".123");
    TEST_INVALID("1.");
    TEST_INVALID("INF");
    TEST_INVALID("inf");
    TEST_INVALID("NAN");
    TEST_INVALID("nan");
    TEST_INVALID("+0e");
    TEST_INVALID("1E");

    TEST_ROOT_NOT_SINGULAR("0123");
    TEST_ROOT_NOT_SINGULAR("0x0");
    TEST_ROOT_NOT_SINGULAR("0x123");

    // 边界测试
    // https://en.wikipedia.org/wiki/Double-precision_floating-point_format#Double-precision_examples
    TEST_NUMBER(1.0000000000000002,
                "1.0000000000000002"); /* the smallest number > 1 */
    TEST_NUMBER(4.9406564584124654e-324,
                "4.9406564584124654e-324"); /* minimum denormal */
    TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
    TEST_NUMBER(2.2250738585072009e-308,
                "2.2250738585072009e-308"); /* Max subnormal double */
    TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    TEST_NUMBER(2.2250738585072014e-308,
                "2.2250738585072014e-308"); /* Min normal positive double */
    TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    TEST_NUMBER(1.7976931348623157e+308,
                "1.7976931348623157e+308"); /* Max double */
    TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");

    TEST_NUMBER_TOO_BIG("1e309");
    TEST_NUMBER_TOO_BIG("-1e309");
}