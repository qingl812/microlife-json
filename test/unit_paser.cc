#include "detail/basic_json.hpp"
#include "unit.hpp"

#include <gtest/gtest.h>

using namespace microlife;
using namespace detail;
using namespace parser;

TEST(parser, expect_value) {
    value_s value;

    value.type = type_t::object;
    EXPECT_EQ(parse_t::expect_value, parse(&value, ""));
    EXPECT_EQ(type_t::null, get_type(&value));

    value.type = type_t::object;
    EXPECT_EQ(parse_t::expect_value, parse(&value, " "));
    EXPECT_EQ(type_t::null, get_type(&value));

    value.type = type_t::object;
    EXPECT_EQ(parse_t::expect_value, parse(&value, " \t \n\t\t "));
    EXPECT_EQ(type_t::null, get_type(&value));
}

TEST(parser, invalid_value) {
    value_s value;

    value.type = type_t::object;
    EXPECT_EQ(parse_t::invalid_value, parse(&value, "nul"));
    EXPECT_EQ(type_t::null, get_type(&value));

    value.type = type_t::object;
    EXPECT_EQ(parse_t::invalid_value, parse(&value, "?"));
    EXPECT_EQ(type_t::null, get_type(&value));

    value.type = type_t::object;
    EXPECT_EQ(parse_t::invalid_value, parse(&value, " ?"));
    EXPECT_EQ(type_t::null, get_type(&value));

    value.type = type_t::object;
    EXPECT_EQ(parse_t::invalid_value, parse(&value, " ?  "));
    EXPECT_EQ(type_t::null, get_type(&value));
}

TEST(parser, root_not_singular) {
    value_s value;

    value.type = type_t::object;
    EXPECT_EQ(parse_t::root_not_singular, parse(&value, "null x"));
    EXPECT_EQ(type_t::null, get_type(&value));

    value.type = type_t::object;
    EXPECT_EQ(parse_t::root_not_singular, parse(&value, "  null  x  "));
    EXPECT_EQ(type_t::null, get_type(&value));
}

TEST(parser, null) {
    value_s value;

    value.type = type_t::object;
    EXPECT_EQ(parse_t::ok, parse(&value, "null"));
    EXPECT_EQ(type_t::null, get_type(&value));

    value.type = type_t::object;
    EXPECT_EQ(parse_t::ok, parse(&value, " null "));
    EXPECT_EQ(type_t::null, get_type(&value));

    value.type = type_t::object;
    EXPECT_EQ(parse_t::invalid_value, parse(&value, "unull"));
    EXPECT_EQ(type_t::null, get_type(&value));

    value.type = type_t::object;
    EXPECT_EQ(parse_t::invalid_value, parse(&value, "nnull"));
    EXPECT_EQ(type_t::null, get_type(&value));

    value.type = type_t::object;
    EXPECT_EQ(parse_t::invalid_value, parse(&value, "nulll"));
    EXPECT_EQ(type_t::null, get_type(&value));
}

TEST(parser, boolean) {
    value_s value;

    value.type = type_t::null;
    EXPECT_EQ(parse_t::ok, parse(&value, "true"));
    EXPECT_EQ(type_t::boolean, get_type(&value));
    EXPECT_EQ(true, value.value.boolean);

    value.type = type_t::null;
    EXPECT_EQ(parse_t::ok, parse(&value, "false"));
    EXPECT_EQ(type_t::boolean, get_type(&value));
    EXPECT_EQ(false, value.value.boolean);

    value.type = type_t::object;
    EXPECT_EQ(parse_t::invalid_value, parse(&value, "faase"));
    EXPECT_EQ(type_t::null, get_type(&value));

    value.type = type_t::object;
    EXPECT_EQ(parse_t::invalid_value, parse(&value, "truee"));
    EXPECT_EQ(type_t::null, get_type(&value));
}