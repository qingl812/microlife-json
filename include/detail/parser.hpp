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
// 解析所有空格
inline void parse_whitespace(context_s* context) {
    const char* p = context->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    context->json = p;
}

// 解析一个 null 值
inline parse_t parse_null(context_s* c, value_s* v) {
    assert(*c->json == 'n');

    if (c->json[1] == 'u' && c->json[2] == 'l' && c->json[3] == 'l') {
        c->json += 4;
        v->type = type_t::null;
        return parse_t::ok;
    } else {
        return parse_t::invalid_value;
    }
}

// 解析一个 boolean 值
inline parse_t parse_boolean(context_s* c, value_s* v) {
    assert(*c->json == 't' || *c->json == 'f');

    if (c->json[1] == 'r' && c->json[2] == 'u' && c->json[3] == 'e') {
        c->json += 4;
        v->value.boolean = true;
    } else if (c->json[1] == 'a' && c->json[2] == 'l' && c->json[3] == 's' &&
               c->json[4] == 'e') {
        c->json += 5;
        v->value.boolean = false;
    } else {
        return parse_t::invalid_value;
    }
    v->type = type_t::boolean;
    return parse_t::ok;
}

// 解析一个 JSON 值
inline parse_t parse_value(context_s* context, value_s* value) {
    // 检查第一个字符
    switch (*context->json) {
    case 'n':
        return parse_null(context, value);
    case 't':
    case 'f':
        return parse_boolean(context, value);
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
    if (ret == parse_t::expect_value || ret == parse_t::ok) {
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
