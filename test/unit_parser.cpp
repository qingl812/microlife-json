#include "microlife/detail/parser.h"

#include <gtest/gtest.h>

#define TEST_PARSER_PARSE_BASE(_json, _judge, _type)                           \
    do {                                                                       \
        microlife::detail::basic_json::string_t str = _json;                   \
        microlife::detail::basic_json j;                                       \
        _judge(microlife::detail::parser::parse(str, j));                      \
        EXPECT_EQ(microlife::detail::value_t::_type, j.type());                \
    } while (0)

#define TEST_PARSER_PARSE_TRUE(_type, _json)                                   \
    TEST_PARSER_PARSE_BASE(_json, EXPECT_TRUE, _type)

#define TEST_PARSER_PARSE_FALSE(_json)                                         \
    TEST_PARSER_PARSE_BASE(_json, EXPECT_FALSE, null)

TEST(parser, parse) {
    // false
    TEST_PARSER_PARSE_FALSE("nulll");
    TEST_PARSER_PARSE_FALSE("t");
    TEST_PARSER_PARSE_FALSE("truee");
    TEST_PARSER_PARSE_FALSE("-0.-");
    TEST_PARSER_PARSE_FALSE("1e0.3");
    TEST_PARSER_PARSE_FALSE("\"a");
    TEST_PARSER_PARSE_FALSE("\"a\a");
    TEST_PARSER_PARSE_FALSE("[123");
    TEST_PARSER_PARSE_FALSE("{null");
    TEST_PARSER_PARSE_FALSE("null}");
    TEST_PARSER_PARSE_FALSE("[");
    TEST_PARSER_PARSE_FALSE("]");
    TEST_PARSER_PARSE_FALSE("{");
    TEST_PARSER_PARSE_FALSE("}");
    TEST_PARSER_PARSE_FALSE("[[]");
    TEST_PARSER_PARSE_FALSE("{{}");
    TEST_PARSER_PARSE_FALSE("{{[[]}}");
    TEST_PARSER_PARSE_FALSE("[123,true,false,]");
    TEST_PARSER_PARSE_FALSE("[123,true,,false]");
    TEST_PARSER_PARSE_FALSE("[123,truee,false]");
    TEST_PARSER_PARSE_FALSE("[123,t,false]");
    TEST_PARSER_PARSE_FALSE("[123,\",false]");
    TEST_PARSER_PARSE_FALSE("{\"a\":null:}");
    TEST_PARSER_PARSE_FALSE("{\"a\":,null}");
    TEST_PARSER_PARSE_FALSE("{\"a\"null}");
    TEST_PARSER_PARSE_FALSE("{,\"a\":null}");
    TEST_PARSER_PARSE_FALSE("{\"a\":null,}");
    TEST_PARSER_PARSE_FALSE("{[true]:null}");

    TEST_PARSER_PARSE_FALSE("0123");
    TEST_PARSER_PARSE_FALSE("0x0");
    TEST_PARSER_PARSE_FALSE("0x123");

    // true
    TEST_PARSER_PARSE_TRUE(null, "null");
    TEST_PARSER_PARSE_TRUE(boolean, "true");
    TEST_PARSER_PARSE_TRUE(boolean, "false");
    TEST_PARSER_PARSE_TRUE(number, "123");
    TEST_PARSER_PARSE_TRUE(string, "\"hello\"");

    TEST_PARSER_PARSE_TRUE(array, "[123,true,false]");
    TEST_PARSER_PARSE_TRUE(array, "[123,true,[false]]");
    TEST_PARSER_PARSE_TRUE(array, "[123,[[true]],[false]]");
    TEST_PARSER_PARSE_TRUE(array, "[123]");
    TEST_PARSER_PARSE_TRUE(array, "[[[[123]]]]");
    TEST_PARSER_PARSE_TRUE(array, "[[[[{\"a\":null}]]]]");
    TEST_PARSER_PARSE_TRUE(object, "{}");
    TEST_PARSER_PARSE_TRUE(object, "{\"a\":null}");
    TEST_PARSER_PARSE_TRUE(object, "{\"v\":[{\"a\":null}]}");
}
