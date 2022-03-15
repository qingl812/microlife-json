#include "detail/basic_json.hpp"

#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

namespace microlife {
namespace detail {
static std::ostream& operator<<(std::ostream& os,
                                const parser::error_t& error) {
    switch (error) {
    case parser::error_t::ok:
        return os << "ok";

    case parser::error_t::expect_value:
        return os << "expect_value";

    case parser::error_t::invalid_value:
        return os << "invalid_value";

    case parser::error_t::root_not_singular:
        return os << "root_not_singular";

    case parser::error_t::number_too_big:
        return os << "number_too_big";

    case parser::error_t::miss_quotation_mark:
        return os << "miss_quotation_mark";

    case parser::error_t::invalid_string_char:
        return os << "invalid_string_char";

    case parser::error_t::invalid_string_escape:
        return os << "invalid_string_escape";

    case parser::error_t::invalid_unicode_hex:
        return os << "invalid_unicode_hex";

    case parser::error_t::invalid_unicode_surrogate:
        return os << "invalid_unicode_surrogate";

    default:
        return os << "unknown parse_t";
    }
}

static std::ostream& operator<<(std::ostream& os, const type_t& type) {
    switch (type) {
    case microlife::detail::type_t::array:
        return os << "array";

    case microlife::detail::type_t::boolean:
        return os << "boolean";

    case microlife::detail::type_t::null:
        return os << "null";

    case microlife::detail::type_t::number:
        return os << "number";

    case microlife::detail::type_t::object:
        return os << "object";

    case microlife::detail::type_t::string:
        return os << "string";

    default:
        return os << "unknown type_t";
    }
}
} // namespace detail
} // namespace microlife
