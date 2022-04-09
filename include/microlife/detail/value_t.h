#pragma once

#include <cstdint> // uint8_t

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
} // namespace detail
} // namespace microlife
