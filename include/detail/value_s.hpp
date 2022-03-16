#pragma once
#include "type_t.hpp"

#include <assert.h>
#include <map>
#include <stddef.h> // size_t
#include <string>
#include <vector>

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
    using boolean_t = bool;
    using number_t = double;
    using string_t = std::string;
    using array_t = std::vector<value_s*>;
    using object_t = std::map<std::string, value_s*>;

    union {
        boolean_t boolean;
        number_t number;
        string_t* string;
        array_t* array;
        object_t* object;
    };
    type_t type;
};
} // namespace detail
} // namespace microlife