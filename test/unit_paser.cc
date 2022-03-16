#include "unit_depend.hpp"

// 测试 parser 构造函数和析构函数
TEST(parser, constructor) {
    microlife::detail::parser* parser;

    parser = new microlife::detail::parser();
    delete parser;

    parser = new microlife::detail::parser();
    ASSERT_TRUE(parser->parse("null"));
    ASSERT_TRUE(parser->parse("true"));
    delete parser;
}

#define _TEST_BASE(_json, _ret, _type)                                         \
    microlife::detail::parser parser;                                          \
    parser.parse(_json);                                                       \
    EXPECT_EQ(microlife::detail::parser::error_t::_ret, parser.m_error);       \
    EXPECT_EQ(microlife::detail::type_t::_type, parser.m_value->type);

// 测试 parse_t parse(); 函数返回值
// 以及其 value.type 是否符合预期
#define TEST_RETURN(_json, _ret, _type)                                        \
    do {                                                                       \
        _TEST_BASE(_json, _ret, _type);                                        \
    } while (0)

// 带有 VALUE 的测试
#define TEST_VALUE(_expected, _json, _type, _value_type)                       \
    do {                                                                       \
        _TEST_BASE(_json, ok, _type);                                          \
        EXPECT_EQ(_expected, parser.m_value->_value_type);                     \
    } while (0)

// 带有 指针形式VALUE 的测试
#define TEST_PVALUE(_expected, _json, _type, _value_type)                      \
    do {                                                                       \
        _TEST_BASE(_json, ok, _type);                                          \
        EXPECT_EQ(_expected, *parser.m_value->_value_type);                    \
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
    TEST_VALUE(expected, json, boolean, boolean)
TEST(parser, boolean) {
    TEST_BOOLEAN(true, "true");
    TEST_BOOLEAN(false, "false");
    TEST_BOOLEAN(true, "true");

    TEST_INVALID("faase");

    TEST_ROOT_NOT_SINGULAR("truee");
}

// number 的测试
#define TEST_NUMBER(expected, json) TEST_VALUE(expected, json, number, number)
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

// string 的测试
#define TEST_STRING(expected, json) TEST_PVALUE(expected, json, string, string)

#define TEST_MISS_QUOTATION_MARK(json)                                         \
    TEST_RETURN(json, miss_quotation_mark, null)

#define TEST_INVALID_STRING_ESCAPE(json)                                       \
    TEST_RETURN(json, invalid_string_escape, null)

#define TEST_INVALID_UNICODE_HEX(json)                                         \
    TEST_RETURN(json, invalid_unicode_hex, null)

#define TEST_INVALID_UNICODE_SURROGATE(json)                                   \
    TEST_RETURN(json, invalid_unicode_surrogate, null)

#define TEST_INVALID_STRING_CHAR(json)                                         \
    TEST_RETURN(json, invalid_string_char, null)
TEST(parser, string) {
    TEST_STRING("hello", R"("hello")");
    TEST_STRING("\" \\ / \b \f \n \r \t",
                "\"\\\" \\\\ / \\b \\f \\n \\r \\t\""); // 与下面语句相同
    TEST_STRING("\" \\ / \b \f \n \r \t", R"("\" \\ / \b \f \n \r \t")");
    TEST_STRING("Hello\nWorld", R"("Hello\nWorld")");

    // 连续多次解析
    microlife::detail::parser parser;

    parser.parse("\"Hello world!\"");
    ASSERT_EQ(parser.m_value->type, microlife::detail::type_t::string);
    EXPECT_EQ("Hello world!", *parser.m_value->string);

    parser.parse("\"good\"");
    ASSERT_EQ(parser.m_value->type, microlife::detail::type_t::string);
    EXPECT_EQ("good", *parser.m_value->string);

    // 缺失右引号
    TEST_MISS_QUOTATION_MARK("\"");
    TEST_MISS_QUOTATION_MARK("\"abc");

    TEST_INVALID_STRING_ESCAPE("\"\\v\"");
    TEST_INVALID_STRING_ESCAPE("\"\\'\"");
    TEST_INVALID_STRING_ESCAPE("\"\\0\"");
    TEST_INVALID_STRING_ESCAPE("\"\\x12\"");

    TEST_INVALID_STRING_CHAR("\"\x01\"");
    TEST_INVALID_STRING_CHAR("\"\x1F\"");

    // utf-8
    TEST_STRING(std::string("Hello") + '\0' + "World", "\"Hello\\u0000World\"");
    TEST_STRING("\x24", "\"\\u0024\"");         /* Dollar sign U+0024 */
    TEST_STRING("\xC2\xA2", "\"\\u00A2\"");     /* Cents sign U+00A2 */
    TEST_STRING("\xE2\x82\xAC", "\"\\u20AC\""); /* Euro sign U+20AC */
    TEST_STRING("\xF0\x9D\x84\x9E",
                "\"\\uD834\\uDD1E\""); /* G clef sign U+1D11E */
    TEST_STRING("\xF0\x9D\x84\x9E",
                "\"\\ud834\\udd1e\""); /* G clef sign U+1D11E */

    TEST_INVALID_UNICODE_HEX("\"\\u\"");
    TEST_INVALID_UNICODE_HEX("\"\\u0\"");
    TEST_INVALID_UNICODE_HEX("\"\\u01\"");
    TEST_INVALID_UNICODE_HEX("\"\\u012\"");
    TEST_INVALID_UNICODE_HEX("\"\\u/000\"");
    TEST_INVALID_UNICODE_HEX("\"\\uG000\"");
    TEST_INVALID_UNICODE_HEX("\"\\u0/00\"");
    TEST_INVALID_UNICODE_HEX("\"\\u0G00\"");
    TEST_INVALID_UNICODE_HEX("\"\\u00/0\"");
    TEST_INVALID_UNICODE_HEX("\"\\u00G0\"");
    TEST_INVALID_UNICODE_HEX("\"\\u000/\"");
    TEST_INVALID_UNICODE_HEX("\"\\u000G\"");
    TEST_INVALID_UNICODE_HEX("\"\\u 123\"");
    TEST_INVALID_UNICODE_HEX("\"\\uD800\\u0/00\"");

    TEST_INVALID_UNICODE_SURROGATE("\"\\uD800\"");
    TEST_INVALID_UNICODE_SURROGATE("\"\\uDBFF\"");
    TEST_INVALID_UNICODE_SURROGATE("\"\\uD800\\\\\"");
    TEST_INVALID_UNICODE_SURROGATE("\"\\uD800\\uDBFF\"");
    TEST_INVALID_UNICODE_SURROGATE("\"\\uD800\\uE000\"");
}

// array 的测试
#define TEST_MISS_COMMA_OR_SQUARE_BRACKET(json)                                \
    TEST_RETURN(json, miss_comma_or_square_bracket, null)
TEST(parser, array) {
    microlife::detail::parser parser;

    // test 1
    EXPECT_TRUE(parser.parse("[ ]"));
    EXPECT_EQ(microlife::detail::parser::error_t::ok, parser.m_error);
    EXPECT_EQ(microlife::detail::type_t::array, parser.m_value->type);
    EXPECT_EQ(0, parser.m_value->array->size());

    // test 2
    EXPECT_TRUE(parser.parse("[ null , false , true , 123 , \"abc\" ]"));
    EXPECT_EQ(microlife::detail::parser::error_t::ok, parser.m_error);
    EXPECT_EQ(microlife::detail::type_t::array, parser.m_value->type);
    EXPECT_EQ(5, parser.m_value->array->size());

    EXPECT_EQ(microlife::detail::type_t::null,
              parser.m_value->array->at(0)->type);

    EXPECT_EQ(microlife::detail::type_t::boolean,
              parser.m_value->array->at(1)->type);
    EXPECT_EQ(false, parser.m_value->array->at(1)->boolean);

    EXPECT_EQ(microlife::detail::type_t::boolean,
              parser.m_value->array->at(2)->type);
    EXPECT_EQ(true, parser.m_value->array->at(2)->boolean);

    EXPECT_EQ(microlife::detail::type_t::number,
              parser.m_value->array->at(3)->type);
    EXPECT_EQ(123, parser.m_value->array->at(3)->number);

    EXPECT_EQ(microlife::detail::type_t::string,
              parser.m_value->array->at(4)->type);
    EXPECT_EQ("abc", *parser.m_value->array->at(4)->string);

    // test 3
    EXPECT_TRUE(parser.parse("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]"));
    EXPECT_EQ(microlife::detail::parser::error_t::ok, parser.m_error);
    EXPECT_EQ(microlife::detail::type_t::array, parser.m_value->type);
    EXPECT_EQ(4, parser.m_value->array->size());

    for (int i = 0; i < 3; i++) {
        EXPECT_EQ(microlife::detail::type_t::array,
                  parser.m_value->array->at(i)->type);
        EXPECT_EQ(i, parser.m_value->array->at(i)->array->size());

        for (int j = 0; j < i; j++) {
            EXPECT_EQ(microlife::detail::type_t::array,
                      parser.m_value->array->at(i)->type);
            EXPECT_EQ(j, parser.m_value->array->at(i)->array->at(j)->number);
        }
    }

    // failed
    TEST_MISS_COMMA_OR_SQUARE_BRACKET("[1");
    TEST_MISS_COMMA_OR_SQUARE_BRACKET("[1}");
    TEST_MISS_COMMA_OR_SQUARE_BRACKET("[1 2");
    TEST_MISS_COMMA_OR_SQUARE_BRACKET("[[]");

    TEST_INVALID("[trua]");
}