#pragma once
#include <cstdint> // uint8_t

namespace microlife {
namespace detail {
///////////////////
// JSON 解析结果 //
///////////////////

/*
 * @brief 用于判断 JSON 数据解析结果的正确性
 */
enum class parse_t : uint8_t {
    ok,
    expect_value,
    invalid_value,
    root_not_singular
};
} // namespace detail
} // namespace microlife