#include "detail/basic_json.hpp"

#include <iostream>

namespace microlife {
namespace detail {
std::ostream& operator<<(std::ostream& os, const parse_t& parse) {
    switch (parse) {
    case microlife::detail::parse_t::ok:
        return os << "ok";

    case microlife::detail::parse_t::expect_value:
        return os << "expect_value";

    case microlife::detail::parse_t::invalid_value:
        return os << "invalid_value";

    case microlife::detail::parse_t::root_not_singular:
        return os << "root_not_singular";

    default:
        return os << "unknown type";
    }
}
} // namespace detail
} // namespace microlife