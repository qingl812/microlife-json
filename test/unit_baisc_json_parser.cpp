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

#define TEST_PARSER(_expeted, _input)                                          \
    do {                                                                       \
        basic_json j1 = _expeted, j2;                                          \
        EXPECT_TRUE(j2.parse(_input));                                         \
        EXPECT_EQ(j1, j2);                                                     \
    } while (0)

TEST(basic_json, parser) {
    TEST_PARSER(nullptr, "null");
    TEST_PARSER(true, "true");
    TEST_PARSER(false, "false");
    TEST_PARSER(123, "123");
    TEST_PARSER(3.14, "3.14");
    TEST_PARSER("hello", "\"hello\"");
    TEST_PARSER(array_t({123, true, false}), "[123,true,false]");

    auto ary = array_t({123, true, nullptr});
    ary[2] = array_t({false});
    TEST_PARSER(ary, "[123,true,[false]]");

    ary[1] = array_t({array_t({true})});
    TEST_PARSER(ary, "[123,[[true]],[false]]");

    TEST_PARSER(array_t({123}), "[123]");
    TEST_PARSER(array_t(), "[]");

    basic_json j = value_t::array;
    auto* a = &j.get<array_t&>();
    for (int i = 0; i < 3; i++) {
        a->push_back(value_t::array);
        a = &a->back().get<array_t&>();
    }
    a->push_back(123);
    TEST_PARSER(j, "[[[[123]]]]");

    a->back() = object_t({{"a", nullptr}});
    TEST_PARSER(j, "[[[[{\"a\":null}]]]]");

    TEST_PARSER(object_t(), "{}");

    j = object_t({{"a", nullptr}});
    TEST_PARSER(j, "{\"a\":null}");

    j = object_t({{"v", array_t({object_t({{"a", nullptr}})})}});
    TEST_PARSER(j, "{\"v\":[{\"a\":null}]}");
}
