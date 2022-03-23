#include "detail/value_t.hpp"

#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

namespace microlife {
namespace detail {
static std::ostream& operator<<(std::ostream& os, const value_t& type) {
    switch (type) {
    case value_t::array:
        return os << "array";

    case value_t::boolean:
        return os << "boolean";

    case value_t::null:
        return os << "null";

    case value_t::number:
        return os << "number";

    case value_t::object:
        return os << "object";

    case value_t::string:
        return os << "string";

    default:
        return os << "unknown type_t";
    }
}
} // namespace detail
} // namespace microlife
