#define JSON_TESTS_PRIVATE

#include "microlife/detail/basic_json.h"

#include <gtest/gtest.h>

using basic_json = microlife::detail::basic_json;
using json_value = basic_json::json_value;
using value_t = basic_json::value_t;
using array_t = basic_json::array_t;
using object_t = basic_json::object_t;
using string_t = basic_json::string_t;
using number_t = basic_json::number_t;
using boolean_t = basic_json::boolean_t;

// 假定解析模块没有错误
#define TEST_DUMP_BASE(_value)                                                 \
    do {                                                                       \
        basic_json v1(_value);                                                 \
        EXPECT_EQ(_value, v1.dump());                                          \
    } while (0)

TEST(basic_json, dump) {
    // number
    // TEST_DUMP_BASE("0");
    // TEST_DUMP_BASE("-0");
    // TEST_DUMP_BASE("1");
    // TEST_DUMP_BASE("-1");
    // TEST_DUMP_BASE("1.5");
    // TEST_DUMP_BASE("-1.5");
    // TEST_DUMP_BASE("3.25");
    // TEST_DUMP_BASE("1e+20");
    // TEST_DUMP_BASE("1.234e+20");
    // TEST_DUMP_BASE("1.234e-20");

    // TEST_DUMP_BASE("1.0000000000000002");      /* the smallest number > 1 */
    // TEST_DUMP_BASE("4.9406564584124654e-324"); /* minimum denormal */
    // TEST_DUMP_BASE("-4.9406564584124654e-324");
    // TEST_DUMP_BASE("2.2250738585072009e-308"); /* Max subnormal double */
    // TEST_DUMP_BASE("-2.2250738585072009e-308");
    // TEST_DUMP_BASE("2.2250738585072014e-308"); /* Min normal positive double
    // */ TEST_DUMP_BASE("-2.2250738585072014e-308");
    // TEST_DUMP_BASE("1.7976931348623157e+308"); /* Max double */
    // TEST_DUMP_BASE("-1.7976931348623157e+308");

    // string
    // TEST_DUMP_BASE("\"\"");
    // TEST_DUMP_BASE("\"Hello\"");
    // TEST_DUMP_BASE("\"Hello\\nWorld\"");
    // TEST_DUMP_BASE("\"\\\" \\\\ / \\b \\f \\n \\r \\t\"");
    // TEST_DUMP_BASE("\"Hello\\u0000World\"");

    // array
    // TEST_DUMP_BASE("[]");
    // TEST_DUMP_BASE("[null,false,true,123,\"abc\",[1,2,3]]");

    // object
    // TEST_DUMP_BASE("{}");
    // TEST_DUMP_BASE(
    //     "{\"n\":null,\"f\":false,\"t\":true,\"i\":123,\"s\":\"abc\",\"a\":[1,2,"
    //     "3],\"o\":{\"1\":1,\"2\":2,\"3\":3}}");

    // literal
    // TEST_DUMP_BASE("null");
    // TEST_DUMP_BASE("false");
    // TEST_DUMP_BASE("true");
}