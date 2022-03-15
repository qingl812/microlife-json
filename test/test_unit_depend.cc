#include "unit_depend.hpp"

// 测试 parse_t 是否可以 cout
#define TEST_OS(expected, target)                                              \
    do {                                                                       \
        std::ostringstream osstring;                                           \
        std::ostream& os = osstring;                                           \
        os << microlife::detail::target;                                       \
        EXPECT_EQ(expected, osstring.str());                                   \
    } while (0)

// os parse_t
#define TEST_OS_PARSE_T(expected, target)                                      \
    TEST_OS(expected, parser::error_t::target)
TEST(unitHpp, os_error_t) {
    TEST_OS_PARSE_T("ok", ok);
    TEST_OS_PARSE_T("expect_value", expect_value);
    TEST_OS_PARSE_T("invalid_value", invalid_value);
    TEST_OS_PARSE_T("root_not_singular", root_not_singular);
    TEST_OS_PARSE_T("number_too_big", number_too_big);
    TEST_OS_PARSE_T("invalid_string_char", invalid_string_char);
    TEST_OS_PARSE_T("miss_quotation_mark", miss_quotation_mark);
    TEST_OS_PARSE_T("invalid_string_escape", invalid_string_escape);
    TEST_OS_PARSE_T("invalid_unicode_surrogate", invalid_unicode_surrogate);
    TEST_OS_PARSE_T("invalid_unicode_hex", invalid_unicode_hex);

    // unknown type
    std::ostringstream osstring;
    std::ostream& os = osstring;
    os << microlife::detail::parser::error_t(100);
    EXPECT_EQ("unknown parse_t", osstring.str());
}

// os type_t
#define TEST_OS_TYPE_T(expected, target) TEST_OS(expected, type_t::target)
TEST(unitHpp, os_type_t) {
    TEST_OS_TYPE_T("array", array);
    TEST_OS_TYPE_T("boolean", boolean);
    TEST_OS_TYPE_T("null", null);
    TEST_OS_TYPE_T("number", number);
    TEST_OS_TYPE_T("object", object);
    TEST_OS_TYPE_T("string", string);

    // unknown type
    std::ostringstream osstring;
    std::ostream& os = osstring;
    os << microlife::detail::type_t(100);
    EXPECT_EQ("unknown type_t", osstring.str());
}