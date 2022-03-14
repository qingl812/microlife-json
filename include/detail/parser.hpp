#pragma once
#include "context_s.hpp"
#include "parse_t.hpp"
#include "value_s.hpp"

#include <assert.h>
#include <errno.h> // errno(strtod)
#include <math.h>  // HUGE_VAL(strtod)
#include <memory.h>
#include <stdlib.h>
#include <string>

// JSON 解析 的具体实现
namespace microlife {
namespace detail {
////////////////
// JSON 解析 //
///////////////

/*
 * @brief 用于解析 JSON
 */
class parser {
public:
    // 存储着解析出来的 json 值
    value_s* value;

public:
    parser() { value = nullptr; }
    ~parser() {
        if (!value)
            delete value;
    }

    parse_t parse(const char* json) {
        if (this->value)
            delete this->value;
        this->value = new value_s();
        return parse_basic(json, this->value);
    }

private: // 辅助函数
    static bool is_whitespace(char c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }

private: // private static
    // 解析一个 json 字符串, 返回值为解析是否成功
    static parse_t parse_basic(const char* json, value_s* value) {
        assert(value != nullptr);

        value->type = type_t::null;
        parse_whitespace(&json);
        auto ret = parse_value(&json, value);
        if (ret == parse_t::ok) {
            parse_whitespace(&json);
            if (*json != '\0') {
                value->type = type_t::null;
                return parse_t::root_not_singular;
            }
        }
        return ret;
    }

    // 解析一个 JSON 值
    static parse_t parse_value(const char** json, value_s* value) {
        parse_t ret;

        switch (**json) {
        case 'n':
            // null
            return parse_literal(json, value, "ull", type_t::null);

        case 't':
            // true
            ret = parse_literal(json, value, "rue", type_t::boolean);
            if (ret == parse_t::ok)
                value->boolean = true;
            return ret;

        case 'f':
            // false
            ret = parse_literal(json, value, "alse", type_t::boolean);
            if (ret == parse_t::ok)
                value->boolean = false;
            return ret;

        case '\"':
            return parse_string(json, value);

        case '\0':
            return parse_t::expect_value;
        default:
            return parse_number(json, value);
            // return parse_t::invalid_value;
        }
    }

    // 解析所有空格
    static void parse_whitespace(const char** json) {
        const char* p = *json;
        while (is_whitespace(*p))
            p++;
        *json = p;
    }

    // 解析字面量 null true false
    static parse_t parse_literal(
        const char** json,    // 要解析的内容
        value_s* const value, // 解析的结果
        const char* literal, // 预期解析结果，假定第一个字符一定符合预期
        const type_t type // 解析结果类型
    ) {
        auto p = *json;
        p++;
        // strcmp
        while (*literal != '\0') {
            if (*p != *literal) {
                return parse_t::invalid_value;
            }
            p++;
            literal++;
        }

        *json = p;
        value->type = type;
        return parse_t::ok;
    }

    // 判断字符串是否为符合 json 格式的 number
    // 详见 docs/ECMA-404_2nd_edition_december_2017.pdf 第四页
    static bool isNumber(const char* json, const char** end) {
        // 辅助函数
        // 判断字符是否为 1-9 的数字
        auto isDigital = [](char c) { return c >= '0' && c <= '9'; };

        if (*json == '-')
            json++;

        // 以下代码的优化
        // auto isDigitalWithoutZero = [](char c) { return c >= '1' && c <= '9';
        // }; if (*json == '0')
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
    static parse_t parse_number(const char** json, value_s* value) {
        const char* end;
        // 格式校验
        if (!isNumber(*json, &end))
            return parse_t::invalid_value;

        // strtod
        errno = 0;
        value->number = strtod(*json, nullptr);
        if (errno == ERANGE &&
            (value->number == HUGE_VAL || value->number == -HUGE_VAL))
            return parse_t::number_too_big;

        *json = end;
        value->type = type_t::number;
        return parse_t::ok;
    }

    // 解析 string
    static parse_t parse_string(const char** json, value_s* value) {
        assert(**json == '\"');

        auto p = *json;
        std::string str;
        for (;;) {
            char ch = *p++;
            switch (ch) {
            case '\"': {
                *json = p;
                if (value->str != nullptr) {
                    free(value->str);
                }

                value->type = type_t::string;
                value->length = str.size();
                value->str = (char*)malloc(value->length + 1);
                memcpy(value->str, str.c_str(), value->length);
                value->str[value->length] = '\0';
                return parse_t::ok;
            }
            case '\0':
                return parse_t::miss_quotation_mark;
            default:
                str.push_back(ch);
            }
        }
    }
};
} // namespace detail
} // namespace microlife
