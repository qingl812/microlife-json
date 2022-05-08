#define JSON_TESTS_PRIVATE

#include "microlife/detail/basic_json.hpp"
#include "microlife/detail/token_t.hpp"

#include <gtest/gtest.h>

using basic_json = microlife::detail::basic_json;
using json_value = basic_json::json_value;
using value_t = basic_json::value_t;
using array_t = basic_json::array_t;
using object_t = basic_json::object_t;
using string_t = basic_json::string_t;
using number_t = basic_json::number_t;
using boolean_t = basic_json::boolean_t;

// basic_json::json_value
#define TEST_JSON_VALUE_CONSTRUCTOR_BASE(_value, _expected, _type)             \
    do {                                                                       \
        json_value v1(_value);                                                 \
        EXPECT_EQ(_expected, v1._type);                                        \
    } while (0)

#define TEST_JSON_VALUE_CONSTRUCTOR_P(_value, _expected, _type)                \
    do {                                                                       \
        json_value v1(_value);                                                 \
        EXPECT_EQ(_expected, *v1._type);                                       \
    } while (0)

TEST(basic_json, json_value) {
    // constructor
    TEST_JSON_VALUE_CONSTRUCTOR_BASE(true, true, boolean);
    TEST_JSON_VALUE_CONSTRUCTOR_BASE(false, false, boolean);
    TEST_JSON_VALUE_CONSTRUCTOR_BASE(0, 0, number);
    TEST_JSON_VALUE_CONSTRUCTOR_BASE(-10, -10, number);
    TEST_JSON_VALUE_CONSTRUCTOR_BASE(123, 123, number);
    TEST_JSON_VALUE_CONSTRUCTOR_BASE(3.1415, 3.1415, number);

    std::string s1 = "json_value constructor test";
    auto s2 = s1;
    TEST_JSON_VALUE_CONSTRUCTOR_P(string_t(""), "", string);
    TEST_JSON_VALUE_CONSTRUCTOR_P(s1, s2, string);
    TEST_JSON_VALUE_CONSTRUCTOR_P(std::move(s1), s2, string);

    array_t a1 = {nullptr, nullptr};
    auto a2 = a1;
    TEST_JSON_VALUE_CONSTRUCTOR_P(a1, a2, array);
    TEST_JSON_VALUE_CONSTRUCTOR_P(std::move(a1), a2, array);

    object_t obj1 = {{"key1", nullptr}, {"key2", nullptr}};
    auto obj2 = obj1;
    TEST_JSON_VALUE_CONSTRUCTOR_P(obj1, obj2, object);
    TEST_JSON_VALUE_CONSTRUCTOR_P(std::move(obj1), obj2, object);

    TEST_JSON_VALUE_CONSTRUCTOR_P(value_t::object, object_t(), object);
    TEST_JSON_VALUE_CONSTRUCTOR_P(value_t::array, array_t(), array);
    TEST_JSON_VALUE_CONSTRUCTOR_P(value_t::string, string_t(), string);
    TEST_JSON_VALUE_CONSTRUCTOR_BASE(value_t::boolean, false, boolean);
    TEST_JSON_VALUE_CONSTRUCTOR_BASE(value_t::number, 0, number);
    TEST_JSON_VALUE_CONSTRUCTOR_BASE(value_t::null, nullptr, object);

    // destroy
    json_value v1(true);
    v1.destroy(value_t::boolean);

    v1 = json_value(0);
    v1.destroy(value_t::number);

    v1 = json_value(string_t("json_value destroy test"));
    v1.destroy(value_t::string);

    v1 = json_value(value_t::object);
    v1.destroy(value_t::object);

    v1 = json_value(value_t::array);
    v1.destroy(value_t::array);

    {
        json_value v1(value_t::array);
        v1.array->push_back(nullptr);
        v1.array->push_back(true);
        v1.array->push_back(3.1415);
        v1.array->push_back(string_t("hello"));
        v1.destroy(value_t::array);
        v1.destroy(value_t::null);

        auto j1 = new basic_json(2, basic_json(value_t::object));
        EXPECT_TRUE(j1->is_array());
        auto a1 = j1->get<array_t&>();
        EXPECT_EQ(2, a1.size());
        auto& o1 = a1[0];
        EXPECT_TRUE(o1.is_object());
        o1.get<object_t&>().emplace(string_t("key1"), basic_json(true));
        o1.get<object_t&>().emplace("key2", basic_json(1.23));
        o1.get<object_t&>().emplace("key3", basic_json(value_t::array));
        o1.get<object_t&>().emplace("key4", basic_json(value_t::string));
        o1.get<object_t&>().emplace("key5", basic_json(value_t::object));
        delete j1;
    }
}

// // basic_json 构造函数
#define TEST_BASIC_JSON_CONSTRUCTOR_BASE(_value, _expected, _type)             \
    do {                                                                       \
        basic_json v1(_value);                                                 \
        EXPECT_EQ(_expected, v1.get<_type>());                                 \
    } while (0)

TEST(basic_json, constructor) {
    basic_json j1;
    EXPECT_EQ(j1.type(), value_t::null);

    basic_json j2(nullptr);
    EXPECT_EQ(j2.type(), value_t::null);

    TEST_BASIC_JSON_CONSTRUCTOR_BASE(true, true, bool);
    TEST_BASIC_JSON_CONSTRUCTOR_BASE(false, false, boolean_t);
    TEST_BASIC_JSON_CONSTRUCTOR_BASE(3.14, 3.14, number_t);
    TEST_BASIC_JSON_CONSTRUCTOR_BASE(0, 0, number_t);

    std::string s1 = "basic_json constructor test";
    auto s2 = s1;
    TEST_BASIC_JSON_CONSTRUCTOR_BASE(s1, s2, string_t);
    TEST_BASIC_JSON_CONSTRUCTOR_BASE(std::move(s1), s2, string_t);

    array_t a1 = {nullptr, nullptr};
    auto a2 = a1;
    TEST_BASIC_JSON_CONSTRUCTOR_BASE(a1, a2, array_t);
    TEST_BASIC_JSON_CONSTRUCTOR_BASE(std::move(a1), a2, array_t);

    object_t obj1 = {{"key1", true}, {"key2", 0}};
    auto obj2 = obj1;
    TEST_BASIC_JSON_CONSTRUCTOR_BASE(obj1, obj2, object_t);
    TEST_BASIC_JSON_CONSTRUCTOR_BASE(std::move(obj1), obj2, object_t);

    TEST_BASIC_JSON_CONSTRUCTOR_BASE(value_t::object, object_t(), object_t);
    TEST_BASIC_JSON_CONSTRUCTOR_BASE(value_t::array, array_t(), array_t);
    TEST_BASIC_JSON_CONSTRUCTOR_BASE(value_t::string, string_t(), string_t);
    TEST_BASIC_JSON_CONSTRUCTOR_BASE(value_t::boolean, false, boolean_t);
    TEST_BASIC_JSON_CONSTRUCTOR_BASE(value_t::number, 0, int);
    TEST_BASIC_JSON_CONSTRUCTOR_BASE(value_t::number, 0, double);

    // copy constructor
    {
        auto j1 = basic_json(3.1415);
        EXPECT_EQ(j1.type(), value_t::number);
        EXPECT_EQ(3.1415, j1.get<double>());
        auto j2 = j1;
        EXPECT_EQ(j2.type(), value_t::number);
        EXPECT_EQ(3.1415, j2.get<double>());
    }
    {
        basic_json j1 = array_t({true, 3.14159});
        EXPECT_EQ(j1.type(), value_t::array);
        auto a1 = j1.get<array_t&>();
        EXPECT_EQ(2, a1.size());
        EXPECT_EQ(true, a1[0].get<bool>());
        EXPECT_EQ(3.14159, a1[1].get<double>());

        auto j2 = j1;
        EXPECT_EQ(j2.type(), value_t::array);
        a1 = j2.get<array_t&>();
        EXPECT_EQ(2, a1.size());
        EXPECT_EQ(true, a1[0].get<bool>());
        EXPECT_EQ(3.14159, a1[1].get<double>());
    }
    {
        auto j1 = basic_json(2, value_t::object);
        auto& j2 = j1.get<array_t&>().at(0);
        j2.get<object_t&>().emplace("key1", basic_json(true));
        j2.get<object_t&>().emplace("key2", basic_json(1.23));
        j2.get<object_t&>().emplace("key3", basic_json(value_t::array));
        j2.get<object_t&>().emplace("key4", basic_json(value_t::string));
        j2.get<object_t&>().emplace("key5", basic_json(value_t::object));

        basic_json j3(j1);
        EXPECT_TRUE(j3.is_array());
        auto a1 = j3.get<array_t&>();
        EXPECT_EQ(2, a1.size());

        auto obj = a1[0];
        EXPECT_EQ(obj.type(), value_t::object);
        EXPECT_EQ(5, obj.get<object_t&>().size());
        EXPECT_TRUE(obj.get<object_t&>().count("key1") != 0);
        EXPECT_TRUE(obj.get<object_t&>().count("key2") != 0);
        EXPECT_TRUE(obj.get<object_t&>().count("key3") != 0);
        EXPECT_TRUE(obj.get<object_t&>().count("key4") != 0);
        EXPECT_TRUE(obj.get<object_t&>().count("key5") != 0);

        EXPECT_EQ(obj.get<object_t>()["key1"].get<bool>(), true);
        EXPECT_EQ(obj.get<object_t>()["key2"].get<double>(), 1.23);
        EXPECT_TRUE(obj.get<object_t>()["key3"].is_array());
        EXPECT_TRUE(obj.get<object_t>()["key4"].is_string());
        EXPECT_TRUE(obj.get<object_t>()["key5"].is_object());
    }
    // move constructor
    {
        basic_json j1(true);
        EXPECT_TRUE(j1.is_boolean());

        basic_json j2(std::move(j1));
        EXPECT_TRUE(j2.is_boolean());
        EXPECT_EQ(true, j2.get<bool>());

        EXPECT_TRUE(j1.is_null());
    }
    // basic_json(size_t cnt, const basic_json& val)
    {
        basic_json j1(2, 3.14);
        EXPECT_TRUE(j1.is_array());
        EXPECT_EQ(2, j1.get<array_t>().size());
        EXPECT_TRUE(j1.get<array_t>().at(0).is_number());
        EXPECT_TRUE(j1.get<array_t>().at(1).is_number());
        EXPECT_EQ(3.14, j1.get<array_t>().at(0).get<double>());
        EXPECT_EQ(3.14, j1.get<array_t>().at(1).get<double>());
    } // operator=(const basic_json& v)
    {
        basic_json j1(true);
        EXPECT_TRUE(j1.is_boolean());

        basic_json j2 = j1;
        EXPECT_TRUE(j2.is_boolean());
        EXPECT_EQ(j1.get<bool>(), j2.get<bool>());
    }
    // operator=(basic_json&& other)
    {
        basic_json j1(true);
        EXPECT_TRUE(j1.is_boolean());

        basic_json j2 = std::move(j1);
        EXPECT_TRUE(j2.is_boolean());
        EXPECT_EQ(true, j2.get<bool>());

        EXPECT_TRUE(j1.is_null());
    }
}

// 测试 target 是否可以 cout
#define TEST_OS(expected, target)                                              \
    do {                                                                       \
        std::ostringstream osstring;                                           \
        std::ostream& os = osstring;                                           \
        os << microlife::detail::target;                                       \
        EXPECT_EQ(expected, osstring.str());                                   \
    } while (0)

// cout value_t
#define TEST_OS_VALUE_T(expected, target) TEST_OS(expected, value_t::target)

TEST(basic_json, value_t) {
    TEST_OS_VALUE_T("array", array);
    TEST_OS_VALUE_T("boolean", boolean);
    TEST_OS_VALUE_T("null", null);
    TEST_OS_VALUE_T("number", number);
    TEST_OS_VALUE_T("object", object);
    TEST_OS_VALUE_T("string", string);

    // unknown type
    std::ostringstream osstring;
    std::ostream& os = osstring;
    os << microlife::detail::value_t(100);
    EXPECT_EQ("unknown type_t", osstring.str());
}

// cout token_t
#define TEST_OS_TOKEN_T(expected, target) TEST_OS(expected, token_t::target)

TEST(basic_json, token_t) {
    TEST_OS_TOKEN_T("literal_true", literal_true);
    TEST_OS_TOKEN_T("literal_false", literal_false);
    TEST_OS_TOKEN_T("literal_null", literal_null);
    TEST_OS_TOKEN_T("value_string", value_string);
    TEST_OS_TOKEN_T("value_number", value_number);
    TEST_OS_TOKEN_T("begin_array", begin_array);
    TEST_OS_TOKEN_T("begin_object", begin_object);
    TEST_OS_TOKEN_T("end_array", end_array);
    TEST_OS_TOKEN_T("end_object", end_object);
    TEST_OS_TOKEN_T("name_separator", name_separator);
    TEST_OS_TOKEN_T("value_separator", value_separator);
    TEST_OS_TOKEN_T("parse_error", parse_error);
    TEST_OS_TOKEN_T("end_of_input", end_of_input);

    // unknown type
    std::ostringstream osstring;
    std::ostream& os = osstring;
    os << microlife::detail::token_t(100);
    EXPECT_EQ("unknown token_t", osstring.str());
}

TEST(basic_json, public_functions) {
    // unknown type
    std::ostringstream osstring;
    std::ostream& os = osstring;
    os << microlife::detail::basic_json(nullptr);
    EXPECT_EQ("null", osstring.str());

    // compare
    EXPECT_TRUE(basic_json::compare(true, true) == 0);
    EXPECT_TRUE(basic_json::compare(false, false) == 0);
    EXPECT_TRUE(basic_json::compare(false, true) == -1);
    EXPECT_TRUE(basic_json::compare(true, false) == 1);

    EXPECT_TRUE(basic_json::compare(1, 1) == 0);
    EXPECT_TRUE(basic_json::compare(3.14, 1) == 1);
    EXPECT_TRUE(basic_json::compare(1, 10) == -1);

    EXPECT_EQ(1, basic_json::compare(true, nullptr));
    EXPECT_EQ(-1, basic_json::compare(nullptr, true));

    EXPECT_TRUE(basic_json::compare(nullptr, nullptr) == 0);

    EXPECT_TRUE(basic_json::compare(value_t::array, value_t::array) == 0);
    EXPECT_TRUE(basic_json::compare(value_t::object, value_t::array) == 1);

    array_t a1 = {1, 2, 3};
    array_t a2 = {1};
    EXPECT_TRUE(basic_json::compare(a1, a2) == 1);

    object_t o1 = {{"a", 1}, {"b", 2}};
    object_t o2 = {{"a", 1}};
    EXPECT_TRUE(basic_json::compare(o1, o2) == 1);

    o2 = {{"a", 1}, {"b", 10}};
    EXPECT_TRUE(basic_json::compare(o2, o1) == 1);
}
