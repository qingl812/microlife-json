#include "detail/basic_json.hpp"

#include <gtest/gtest.h>
#include <string>

using namespace microlife;
using namespace detail;
using namespace parser;

TEST(JsonDeathTest, death) {
    // paser
    value_s value;
    context_s context;
    char* json = new char[32];

    strcpy(json, "aaaaa");
    context.json = json;
    value.type = type_t::object;
    EXPECT_DEATH(parse_null(&context, &value), ".*");
    EXPECT_DEATH(parse_boolean(&context, &value), ".*");
}