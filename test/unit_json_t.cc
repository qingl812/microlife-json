#include "detail/lexer.hpp"

#include <gtest/gtest.h>

using json_t = microlife::detail::json_t;
using value_t = json_t::value_t;

TEST(jsonType, constructor) {
    json_t j;
    EXPECT_EQ(value_t::null, j.type());

    // null
    j = json_t();
    EXPECT_EQ(value_t::null, j.type());
    EXPECT_TRUE(j.is_null());

    // boolean
    j = json_t(true);
    EXPECT_EQ(value_t::boolean, j.type());
    EXPECT_TRUE(j.is_boolean());
    EXPECT_EQ(true, j.get_boolean());

    j = json_t(false);
    EXPECT_EQ(value_t::boolean, j.type());
    EXPECT_TRUE(j.is_boolean());
    EXPECT_EQ(false, j.get_boolean());

    // number
    j = json_t(1.0);
    EXPECT_EQ(value_t::number, j.type());
    EXPECT_TRUE(j.is_number());
    EXPECT_EQ(1.0, j.get_number());

    j = json_t(-1.2);
    EXPECT_EQ(value_t::number, j.type());
    EXPECT_TRUE(j.is_number());
    EXPECT_EQ(-1.2, j.get_number());

    // string
    j = json_t(json_t::string_t("hello"));
    EXPECT_EQ(value_t::string, j.type());
    EXPECT_TRUE(j.is_string());
    EXPECT_EQ("hello", *j.get_string());

    // array
    json_t::array_t array;
    array.push_back(new json_t(json_t::string_t("hello")));
    j = json_t(std::move(array));
    EXPECT_EQ(value_t::array, j.type());
    EXPECT_TRUE(j.is_array());
    EXPECT_EQ(1, j.get_array()->size());
    EXPECT_TRUE(j.get_array()->at(0)->is_string());
    EXPECT_EQ("hello", *j.get_array()->at(0)->get_string());

    // object
    json_t::object_t object;
    object.emplace("a", new json_t(1.0));
    j = json_t(std::move(object));
    EXPECT_EQ(value_t::object, j.type());
    EXPECT_TRUE(j.is_object());
    EXPECT_EQ(1, j.get_object()->count("a"));
    EXPECT_TRUE(j.get_object()->at("a")->is_number());
    EXPECT_EQ(1.0, j.get_object()->at("a")->get_number());
}

TEST(jsonType, move) {
    json_t::string_t str = "hello";
    auto b = new json_t(std::move(str));

    EXPECT_EQ(value_t::string, b->type());
    EXPECT_TRUE(b->is_string());
    EXPECT_EQ("hello", *b->get_string());

    EXPECT_EQ("", str);
    delete b;

    // 测试 json_value 中
    // : string(new string_t(std::move(value)))
    // 写法的正确性
    str = "hello";
    json_t::string_t* strp = new json_t::string_t(std::move(std::move(str)));
    EXPECT_EQ("hello", *strp);
    EXPECT_EQ("", str);
}

TEST(jsonType, operator) {
    auto a = new json_t(true);
    EXPECT_EQ(value_t::boolean, a->type());
    EXPECT_TRUE(a->is_boolean());
    EXPECT_EQ(true, a->get_boolean());

    auto b = new json_t(json_t::string_t("hello"));
    EXPECT_EQ(value_t::string, b->type());
    EXPECT_TRUE(b->is_string());
    EXPECT_EQ("hello", *b->get_string());

    *a = std::move(*b);
    EXPECT_EQ(value_t::string, a->type());
    EXPECT_TRUE(a->is_string());
    EXPECT_EQ("hello", *a->get_string());

    EXPECT_EQ(value_t::null, b->type());

    delete a;
    delete b;
}