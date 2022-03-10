#pragma once
#include <cstdint> // uint8_t

namespace microlife {
namespace detail {
///////////////
// JSON 内容 //
///////////////

/*
 * @brief 为了减少解析函数之间传递多个参数,
 * 我们把这些数据都放进一个 context_s 结构体
 */
struct context_s {
    const char* json;
};
} // namespace detail
} // namespace microlife