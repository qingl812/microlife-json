#pragma once
#include "type_t.hpp"

#include <assert.h>
#include <stddef.h> // size_t

namespace microlife {
namespace detail {
//////////////
// JSON 值 //
/////////////

/*!
 * @brief JSON 是一个树形结构，我们最终需要实现一个树的数据结构,
 * 每个节点使用 value_s 结构体表示，我们会称它为一个 JSON 值（JSON value）。
 */
struct value_s {
    union {
        bool boolean;
        double number;
        struct {
            char* str;
            size_t length;
        };
    };
    type_t type;
};
} // namespace detail
} // namespace microlife