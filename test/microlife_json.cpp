#include "microlife/json.hpp"

#include <gtest/gtest.h>

TEST(JsonTest, constructor) {
    using json = microlife::json;
    json j;

    j = json();
    EXPECT_TRUE(j.is_null());

    j = json(true);
    EXPECT_TRUE(j.is_boolean());
    EXPECT_EQ(true, j.get<bool>());

    j = json(false);
    EXPECT_TRUE(j.is_boolean());
    EXPECT_EQ(false, j.get<bool>());

    j = json(123);
    EXPECT_TRUE(j.is_number());
    EXPECT_EQ(123, j.get<int>());

    j = json(json::value_t::null);
    EXPECT_TRUE(j.is_null());

    json t(true);
    j = json(t);
    EXPECT_TRUE(j.is_boolean());
    EXPECT_EQ(true, j.get<bool>());

    j = json(std::move(t));
    EXPECT_TRUE(j.is_boolean());
    EXPECT_EQ(true, j.get<bool>());
    EXPECT_TRUE(t.is_null());

    j = json(3, t);
    EXPECT_TRUE(j.is_array());
    EXPECT_EQ(3, j.get<json::array_t>().size());
    EXPECT_TRUE(j.get<json::array_t>().at(0).is_null());
    EXPECT_TRUE(j.get<json::array_t>().at(1).is_null());
    EXPECT_TRUE(j.get<json::array_t>().at(2).is_null());

    // j = {nullptr, true, 2};
    // EXPECT_TRUE(j.is_array());
    // EXPECT_EQ(3, j.get<json::array_t>().size());
    // EXPECT_TRUE(j.get<json::array_t>().at(0).is_null());
    // EXPECT_TRUE(j.get<json::array_t>().at(1).is_boolean());
    // EXPECT_EQ(true, j.get<json::array_t>().at(1).get<bool>());
    // EXPECT_TRUE(j.get<json::array_t>().at(2).is_number());
    // EXPECT_EQ(2, j.get<json::array_t>().at(2).get<int>());
}
