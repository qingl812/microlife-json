#include "detail/basic_json.hpp"

#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

namespace microlife {
namespace detail {
static std::ostream& operator<<(std::ostream& os, const parse_t& parse) {
    switch (parse) {
    case microlife::detail::parse_t::ok:
        return os << "ok";

    case microlife::detail::parse_t::expect_value:
        return os << "expect_value";

    case microlife::detail::parse_t::invalid_value:
        return os << "invalid_value";

    case microlife::detail::parse_t::root_not_singular:
        return os << "root_not_singular";

    case microlife::detail::parse_t::number_too_big:
        return os << "number_too_big";

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
