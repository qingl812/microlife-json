#pragma once
#include <array>    // array
#include <cstdint>  // uint8_t
#include <iostream> // os << value_t

namespace microlife {
namespace detail {
/***
 * @brief JSON value type
 * @author qingl
 * @date 2022_04_09
 */
enum class value_t : uint8_t
{
    null,
    boolean,
    number,
    string,
    array,
    object
};

/***
 * @brief cout value_t
 * @author qingl
 * @date 2022_04_15
 */
inline std::ostream& operator<<(std::ostream& os, const value_t type) {
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

/***
 * @brief compare value_t
 * @author qingl
 * @date 2022_04_19
 */
inline bool operator<(const value_t lhs, const value_t rhs) noexcept {
    static constexpr std::array<std::uint8_t, 6> order = {{
        0 /* null */, 1 /* boolean */, 2 /* number */, 3 /* string */,
        4 /* array */, 5 /* object */
    }};

    const auto l_index = static_cast<std::size_t>(lhs);
    const auto r_index = static_cast<std::size_t>(rhs);
    return l_index < order.size() && r_index < order.size() &&
           order[l_index] < order[r_index];
}
} // namespace detail
} // namespace microlife
