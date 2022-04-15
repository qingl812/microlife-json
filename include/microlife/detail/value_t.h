#pragma once

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
    object,
    array,
    string,
    boolean,
    number,
    null
};

/***
 * @brief cout value_t
 * @author qingl
 * @date 2022_04_15
 */
inline std::ostream& operator<<(std::ostream& os, const value_t& type) {
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
