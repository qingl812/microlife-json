#pragma once
#include "context_s.hpp"
#include "parse_t.hpp"
#include "value_s.hpp"

#include <assert.h>
#include <errno.h> // errno(strtod)
#include <math.h>  // HUGE_VAL(strtod)
#include <stdlib.h>

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
    context_s* const context, // 要解析的内容
    value_s* const value,     // 解析的结果
    const char* literal, // 预期解析结果，假定第一个字符一定符合预期
    const type_t type // 解析结果类型
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

    context->json = json;
    value->type = type;
    return parse_t::ok;
}

// 判断字符串是否为符合 json 格式的 number
// 详见 docs/ECMA-404_2nd_edition_december_2017.pdf 第四页
inline bool isNumber(const char* json, const char** end) {
    // 辅助函数
    // 判断字符是否为 1-9 的数字
    auto isDigital = [](char c) { return c >= '0' && c <= '9'; };

    if (*json == '-')
        json++;

    // 以下代码的优化
    // auto isDigitalWithoutZero = [](char c) { return c >= '1' && c <= '9'; };
    // if (*json == '0')
    //     json++;
    // else if (isDigitalWithoutZero(*json)) {
    //     json++;
    //     while (isDigital(*json))
    //         json++;
    // } else
    //     return false;
    if (isDigital(*json)) {
        if (*json != '0') {
            json++;
            while (isDigital(*json))
                json++;
        } else
            json++;
    } else
        return false;

    if (*json == '.') {
        json++;
        // 至少要有一个数字
        // 移动指针到下一个非数字的字符
        if (!isDigital(*json))
            return false;
        do {
            json++;
        } while (isDigital(*json));
    }

    if (*json == 'e' || *json == 'E') {
        json++;
        if (*json == '+' || *json == '-')
            json++;
        // 至少要有一个数字
        // 移动指针到下一个非数字的字符
        if (!isDigital(*json))
            return false;
        do {
            json++;
        } while (isDigital(*json));
    }

    *end = json;
    return true;
}

// 解析数字
// 使用标准库的 strtod() 来进行转换
// strtod() 可转换 JSON 所要求的格式，但问题是，一些 JSON 不容许的格式
// strtod() 也可转换，所以我们需要自行做格式校验。
// strtod() https://en.cppreference.com/w/c/string/byte/strtof
inline parse_t parse_number(context_s* context, value_s* value) {
    const char* end;
    // 格式校验
    if (!isNumber(context->json, &end))
        return parse_t::invalid_value;

    // strtod
    errno = 0;
    value->value.number = strtod(context->json, nullptr);
    if (errno == ERANGE &&
        (value->value.number == HUGE_VAL || value->value.number == -HUGE_VAL))
        return parse_t::number_too_big;

    context->json = end;
    value->type = type_t::number;
    return parse_t::ok;
}

// 解析一个 JSON 值
static parse_t parse_value(context_s* context, value_s* value) {
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
        return parse_number(context, value);
        // return parse_t::invalid_value;
    }
}

// 解析一个 json 字符串, 返回值为解析是否成功
static parse_t parse(value_s* value, const char* json) {
    assert(value != nullptr);

    context_s context;
    context.json = json;
    value->type = type_t::null;

    parse_whitespace(&context);
    auto ret = parse_value(&context, value);
    if (ret == parse_t::ok) {
        parse_whitespace(&context);
        if (*context.json != '\0') {
            value->type = type_t::null;
            return parse_t::root_not_singular;
        }
    }
    return ret;
}

// 获取 JSON 值的类型
static type_t get_type(const value_s* value) {
    assert(value != nullptr);

    return value->type;
}

// 获取 boolean 值
static bool get_boolean(const value_s* value) {
    assert(value != NULL && value->type == type_t::boolean);
    return value->value.boolean;
}

// 获取 number 值
static double get_number(const value_s* value) {
    assert(value != NULL && value->type == type_t::number);
    return value->value.number;
}
} // namespace parser
} // namespace detail
} // namespace microlife
