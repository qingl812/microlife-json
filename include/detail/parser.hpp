#pragma once
#include "context_s.hpp"
#include "value_s.hpp"

#include <assert.h>
#include <errno.h> // errno(strtod)
#include <math.h>  // HUGE_VAL(strtod)
#include <memory.h>
#include <stdlib.h>
#include <string> // string_t

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
    // 错误信息
    enum class error_t : uint8_t
    {
        ok,                    // 无错误
        expect_value,          // 预期值
        invalid_value,         // 错误的值
        root_not_singular,     // 多个值
        number_too_big,        // 数字过大
        miss_quotation_mark,   // 缺失右引号
        invalid_string_escape, // 无效的转义字符
        invalid_string_char    // 无效的 string 字符
    };

public:
    // 存储着解析出来的 json 值
    value_s* m_value;
    // 错误信息
    error_t m_error;

private:
    using string_p = std::string::const_iterator;

    string_p m_cur, // 当前解析到的字符位置
        m_end;      // 字符串的尾部指针

public:
    parser() : m_value(nullptr) {}
    ~parser() {
        if (!m_value)
            delete m_value;
    }

    // 传入的字符串一定是以 '/0' 结尾的
    bool parse(const std::string& json) {
        assert(*json.end() == '\0');

        if (m_value)
            delete m_value;
        m_value = new value_s();

        m_cur = json.begin();
        m_end = json.end();
        m_error = error_t::ok;
        parse_basic(m_value);
        return m_error == error_t::ok;
    }

private: // 辅助函数
    // 解析是遇到错误，设置 m_error，可以写作 return
    // failed(error_t::invalid_value);
    inline void failed(const error_t& error) { m_error = error; }

    static bool is_whitespace(char c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }

private: // private static
    // 解析一个 json 字符串, 返回值为解析是否成功
    void parse_basic(value_s* value) {
        value->type = type_t::null;
        parse_whitespace();
        parse_value(value);
        if (m_error == error_t::ok) {
            parse_whitespace();
            if (*m_cur != '\0') {
                value->type = type_t::null;
                return failed(error_t::root_not_singular);
            }
        }
    }

    // 解析一个 JSON 值
    void parse_value(value_s* value) {
        // 解析字面量 null true false
        auto parse_literal =
            [&](const char* literal, // 预期解析结果，假定第一个字符一定符合预期
                const type_t type // 解析结果类型
            ) {
                auto p = m_cur;
                p++;
                // strcmp
                while (*literal != '\0') {
                    if (*p != *literal)
                        return failed(error_t::invalid_value);
                    p++;
                    literal++;
                }

                m_cur = p;
                value->type = type;
            };

        switch (*m_cur) {
        case 'n':
            // null
            parse_literal("ull", type_t::null);
            break;

        case 't':
            // true
            parse_literal("rue", type_t::boolean);
            if (m_error == error_t::ok)
                value->boolean = true;
            break;

        case 'f':
            // false
            parse_literal("alse", type_t::boolean);
            if (m_error == error_t::ok)
                value->boolean = false;
            break;

        case '\"':
            parse_string(value);
            break;

        case '\0':
            return failed(error_t::expect_value);

        default:
            parse_number(value);
            break;
            // return parse_t::invalid_value;
        }
    }

    // 解析所有空格
    void parse_whitespace() {
        auto p = m_cur;
        while (is_whitespace(*p))
            p++;
        m_cur = p;
    }

    // 解析数字
    // 使用标准库的 strtod() 来进行转换
    // strtod() 可转换 JSON 所要求的格式，但问题是，一些 JSON 不容许的格式
    // strtod() 也可转换，所以我们需要自行做格式校验。
    // strtod() https://en.cppreference.com/w/c/string/byte/strtof
    void parse_number(value_s* value) {
        // 判断字符串是否为符合 json 格式的 number
        // 详见 docs/ECMA-404_2nd_edition_december_2017.pdf 第四页

        // 判断字符是否为 1-9 的数字
        auto isDigital = [](char c) { return c >= '0' && c <= '9'; };
        auto p = m_cur;
        if (*p == '-')
            p++;

        if (isDigital(*p)) {
            if (*p != '0') {
                p++;
                while (isDigital(*p))
                    p++;
            } else
                p++;
        } else
            return failed(error_t::invalid_value);

        if (*p == '.') {
            p++;
            // 至少要有一个数字
            // 移动指针到下一个非数字的字符
            if (!isDigital(*p))
                return failed(error_t::invalid_value);
            do {
                p++;
            } while (isDigital(*p));
        }

        if (*p == 'e' || *p == 'E') {
            p++;
            if (*p == '+' || *p == '-')
                p++;
            // 至少要有一个数字
            // 移动指针到下一个非数字的字符
            if (!isDigital(*p))
                return failed(error_t::invalid_value);
            do {
                p++;
            } while (isDigital(*p));
        }

        // 数字格式校验正确
        // strtod
        errno = 0;
        value->number = strtod(m_cur.base(), nullptr);
        if (errno == ERANGE &&
            (value->number == HUGE_VAL || value->number == -HUGE_VAL)) {
            return failed(error_t::number_too_big);
        }

        m_cur = p;
        value->type = type_t::number;
    }

    // 解析 string
    void parse_string(value_s* value) {
        assert(*m_cur == '\"');

        auto p = m_cur + 1;
        while (true) {
            if (*p == '\0')
                return failed(error_t::miss_quotation_mark);
            else if (*p == '\"') {
                value->string = new std::string(m_cur + 1, p);
                value->type = type_t::string;
                m_cur = p + 1;
                return;
            } else
                p++;
        }
    }
};
} // namespace detail
} // namespace microlife
