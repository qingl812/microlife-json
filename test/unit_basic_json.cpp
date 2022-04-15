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

    TEST_JSON_VALUE_CONSTRUCTOR_P(value_t::object, *(new object_t()), object);
    TEST_JSON_VALUE_CONSTRUCTOR_P(value_t::array, *(new array_t()), array);
    TEST_JSON_VALUE_CONSTRUCTOR_P(value_t::string, *(new string_t()), string);
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
        auto j1 = new basic_json(2, value_t::object);
        auto a1 = j1->get<array_t&>().at(0);
        a1->get<object_t&>().emplace(string_t("key1"), new basic_json(true));
        a1->get<object_t&>().emplace("key2", new basic_json(1.23));
        a1->get<object_t&>().emplace("key3", new basic_json(value_t::array));
        a1->get<object_t&>().emplace("key4", new basic_json(value_t::string));
        a1->get<object_t&>().emplace("key5", new basic_json(value_t::object));
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

    object_t obj1 = {{"key1", new basic_json(true)},
                     {"key2", new basic_json(0)}};
    auto obj2 = obj1;
    TEST_BASIC_JSON_CONSTRUCTOR_BASE(obj1, obj2, object_t);
    TEST_BASIC_JSON_CONSTRUCTOR_BASE(std::move(obj1), obj2, object_t);

    TEST_BASIC_JSON_CONSTRUCTOR_BASE(value_t::object, *(new object_t()),
                                     object_t);
    TEST_BASIC_JSON_CONSTRUCTOR_BASE(value_t::array, *(new array_t()), array_t);
    TEST_BASIC_JSON_CONSTRUCTOR_BASE(value_t::string, *(new string_t()),
                                     string_t);
    TEST_BASIC_JSON_CONSTRUCTOR_BASE(value_t::boolean, false, boolean_t);
    TEST_BASIC_JSON_CONSTRUCTOR_BASE(value_t::number, 0, int);
    TEST_BASIC_JSON_CONSTRUCTOR_BASE(value_t::number, 0, double);

    // copy constructor
    {
        auto j1 = new basic_json(2, value_t::object);
        auto j2 = j1->get<array_t&>().at(0);
        j2->get<object_t&>().emplace(string_t("key1"), new basic_json(true));
        j2->get<object_t&>().emplace("key2", new basic_json(1.23));
        j2->get<object_t&>().emplace("key3", new basic_json(value_t::array));
        j2->get<object_t&>().emplace("key4", new basic_json(value_t::string));
        j2->get<object_t&>().emplace("key5", new basic_json(value_t::object));

        basic_json j3(*j1);
        EXPECT_TRUE(j3.is_array());
        auto a1 = j3.get<array_t&>();
        EXPECT_EQ(a1.size(), 2);
        EXPECT_EQ(a1[0]->get<object_t>()["key1"]->get<bool>(), true);
        EXPECT_EQ(a1[0]->get<object_t>()["key2"]->get<double>(), 1.23);
        EXPECT_TRUE(a1[0]->get<object_t>()["key3"]->is_array());
        EXPECT_TRUE(a1[0]->get<object_t>()["key4"]->is_string());
        EXPECT_TRUE(a1[0]->get<object_t>()["key5"]->is_object());
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
        EXPECT_TRUE(j1.get<array_t>().at(0)->is_number());
        EXPECT_TRUE(j1.get<array_t>().at(1)->is_number());
        EXPECT_EQ(3.14, j1.get<array_t>().at(0)->get<double>());
        EXPECT_EQ(3.14, j1.get<array_t>().at(1)->get<double>());
    }
    // operator=(const basic_json& v)
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

// 测试 value_t 是否可以 cout
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