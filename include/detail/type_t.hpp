#pragma once
#include <cstdint> // uint8_t

namespace microlife {
namespace detail {
///////////////////
// JSON 类型枚举 //
///////////////////

/*
 * @brief JSON 类型枚举，用于区分不同类型的 JSON 数值
 */
enum class type_t : uint8_t { null, boolean, number, string, array, object };
} // namespace detail
} // namespace microlife