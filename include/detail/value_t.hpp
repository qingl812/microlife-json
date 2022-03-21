#pragma once

#include <cstdint> // uint8_t

namespace microlife {
namespace detail {
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