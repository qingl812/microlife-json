#pragma once
#include "context_s.hpp"
#include "parse_t.hpp"
#include "value_s.hpp"

#include <assert.h>

// JSON 解析 的具体实现
namespace microlife {
namespace detail {
////////////////
// JSON 解析 //
///////////////

/*
 * @brief 用于解析 JSON
 */
namespace parser {
inline bool is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

// 解析所有空格
inline void parse_whitespace(context_s* context) {
    const char* p = context->json;
    while (is_whitespace(*p))
        p++;
    context->json = p;
}

// 解析字面量 null true false
inline parse_t parse_literal(
    context_s* context, // 要解析的内容
    value_s* value,     // 解析的结果
    const char* literal, // 预期解析结果，假定第一个字符一定符合预期
    type_t type          // 解析结果类型
) {
    auto json = context->json;
    json++;
    // strcmp
    while (*literal != '\0') {
        if (*json != *literal) {
            return parse_t::invalid_value;
        }
        json++;
        literal++;
    }
    // 判断 json 的这个单词已经结束
    if (is_whitespace(*json) || *json == '\0') {
        context->json = json;
        value->type = type;
        return parse_t::ok;
    } else
        return parse_t::invalid_value;
}

// 解析一个 JSON 值
inline parse_t parse_value(context_s* context, value_s* value) {
    parse_t ret;

    switch (*context->json) {
    case 'n':
        // null
        return parse_literal(context, value, "ull", type_t::null);

    case 't':
        // true
        ret = parse_literal(context, value, "rue", type_t::boolean);
        if (ret == parse_t::ok)
            value->value.boolean = true;
        return ret;

    case 'f':
        // false
        ret = parse_literal(context, value, "alse", type_t::boolean);
        if (ret == parse_t::ok)
            value->value.boolean = false;
        return ret;

    case '\0':
        return parse_t::expect_value;
    default:
        return parse_t::invalid_value;
    }
}

// 解析一个 json 字符串, 返回值为解析是否成功
inline parse_t parse(value_s* value, const char* json) {
    assert(value != nullptr);

    context_s context;
    context.json = json;
    value->type = type_t::null;

    parse_whitespace(&context);
    auto ret = parse_value(&context, value);
    if (ret == parse_t::ok) {
        parse_whitespace(&context);
        if (*context.json != '\0')
            return parse_t::root_not_singular;
    }
    return ret;
}

// 获取 JSON 值的类型
inline type_t get_type(const value_s* value) {
    assert(value != nullptr);

    return value->type;
}
} // namespace parser
} // namespace detail
} // namespace microlife
