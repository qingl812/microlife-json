#include "unit_cout.hpp"

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
TEST(unitHpp, os_type_t) {
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