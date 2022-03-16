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
        ok,                           // 无错误
        expect_value,                 // 预期值
        invalid_value,                // 错误的值
        root_not_singular,            // 多个值
        number_too_big,               // 数字过大
        miss_quotation_mark,          // 缺失右引号
        invalid_string_escape,        // 无效的转义字符
        invalid_string_char,          // 无效的 string 字符
        invalid_unicode_hex,          // 无效的 unicode 16 进制编码
        invalid_unicode_surrogate,    // 无效的 unicode 代理编码
        miss_comma_or_square_bracket, // 缺失逗号或方括号
        miss_key,                     // 缺失键值
        miss_colon,                   // 缺失冒号
        miss_comma_or_curly_bracket   // 缺失逗号或大括号
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
            return parse_string(value);

        case '\0':
            return failed(error_t::expect_value);

        case '[':
            return parse_array(value);

        case '{':
            return parse_object(value);

        default:
            return parse_number(value);
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
        value->string = new std::string(m_cur + 1, p);
        while (true) {
            switch (*p) {
            case '\0':
                delete value->string;
                return failed(error_t::miss_quotation_mark);

            case '\"':
                value->type = type_t::string;
                m_cur = p + 1;
                return;

            case '\\':
                p++;
                switch (*p) {
                case '\"':
                case '\\':
                case '/':
                    value->string->push_back(*p);
                    break;
                case 'b':
                    value->string->push_back('\b');
                    break;
                case 'f':
                    value->string->push_back('\f');
                    break;
                case 'n':
                    value->string->push_back('\n');
                    break;
                case 'r':
                    value->string->push_back('\r');
                    break;
                case 't':
                    value->string->push_back('\t');
                    break;
                case 'u': {
                    // 解析 utf-8
                    p++;
                    unsigned u, u2;
                    // 解析 4 位的 16 进制数
                    auto parse_hex4 = [&](unsigned& u) {
                        u = 0;
                        for (int i = 0; i < 4; i++) {
                            char ch = *p++;
                            u <<= 4;
                            if (ch >= '0' && ch <= '9')
                                u |= ch - '0';
                            else if (ch >= 'A' && ch <= 'F')
                                u |= ch - ('A' - 10);
                            else if (ch >= 'a' && ch <= 'f')
                                u |= ch - ('a' - 10);
                            else
                                return false;
                        }
                        return true;
                    };

                    // 为什么要做 x & 0xFF 这种操作呢？
                    // 这是因为 u 是 unsigned
                    // 类型，一些编译器可能会警告这个转型可能会截断数据。
                    // 但实际上，配合了范围的检测然后右移之后，可以保证写入的是
                    // 0~255 内的值。 为了避免一些编译器的警告误判，我们加上 x &
                    // 0xFF。
                    // 一般来说，编译器在优化之后，这与操作是会被消去的，不会影响性能。
                    auto encode_utf8 = [&value](unsigned u) {
                        if (u <= 0x7F)
                            value->string->push_back(u & 0xFF);
                        else if (u <= 0x7FF) {
                            value->string->push_back(0xC0 | ((u >> 6) & 0xFF));
                            value->string->push_back(0x80 | (u & 0x3F));
                        } else if (u <= 0xFFFF) {
                            value->string->push_back(0xE0 | ((u >> 12) & 0xFF));
                            value->string->push_back(0x80 | ((u >> 6) & 0x3F));
                            value->string->push_back(0x80 | (u & 0x3F));
                        } else {
                            assert(u <= 0x10FFFF);
                            value->string->push_back(0xF0 | ((u >> 18) & 0xFF));
                            value->string->push_back(0x80 | ((u >> 12) & 0x3F));
                            value->string->push_back(0x80 | ((u >> 6) & 0x3F));
                            value->string->push_back(0x80 | (u & 0x3F));
                        }
                    };

                    if (!parse_hex4(u))
                        return failed(error_t::invalid_unicode_hex);
                    if (u >= 0xD800 && u <= 0xDBFF) { /* surrogate pair */
                        if (*p++ != '\\')
                            return failed(error_t::invalid_unicode_surrogate);
                        if (*p++ != 'u')
                            return failed(error_t::invalid_unicode_surrogate);
                        if (!parse_hex4(u2))
                            return failed(error_t::invalid_unicode_hex);
                        if (u2 < 0xDC00 || u2 > 0xDFFF)
                            return failed(error_t::invalid_unicode_surrogate);
                        u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
                    }
                    encode_utf8(u);
                    p--; // 与后面 p++ 抵消
                    break;
                }
                default:
                    delete value->string;
                    return failed(error_t::invalid_string_escape);
                }
                p++;
                break;

            default:
                if ((unsigned char)*p < 0x20) {
                    delete value->string;
                    return failed(error_t::invalid_string_char);
                }
                value->string->push_back(*p);
                p++;
                break;
            }
        }
    }

    // 解析一个数组
    void parse_array(value_s* value) {
        assert(*m_cur == '[');

        m_cur++;
        value->type = type_t::array;
        value->array = new std::vector<value_s*>();

        auto array = value->array;

        parse_whitespace();
        if (*m_cur == ']') {
            m_cur++;
            return;
        }
        while (true) {
            value_s* e = new value_s();
            auto func_failed = [&](const error_t& error) {
                delete e;
                delete array;
                value->type = type_t::null;
                return failed(error);
            };

            parse_value(e);
            if (m_error != error_t::ok)
                return func_failed(m_error);

            value->array->push_back(e);
            parse_whitespace();
            if (*m_cur == ',') {
                m_cur++;
                parse_whitespace();
            } else if (*m_cur == ']') {
                m_cur++;
                return;
            } else
                return func_failed(error_t::miss_comma_or_square_bracket);
        }
    }

    void parse_object(value_s* value) {
        assert(*m_cur == '{');

        m_cur++;
        value->type = type_t::object;
        value->object = new value_s::object_t();

        auto object = value->object;

        parse_whitespace();
        if (*m_cur == '}') {
            m_cur++;
            return;
        }

        while (true) {
            value_s *key = new value_s(), *val = new value_s();
            auto func_failed = [&](const error_t& error) {
                delete key;
                delete val;
                delete object;
                value->type = type_t::null;
                return failed(error);
            };

            // parse key
            if (*m_cur != '"')
                return func_failed(error_t::miss_key);
            parse_value(key);
            if (m_error != error_t::ok)
                return func_failed(m_error);

            // parse colon
            parse_whitespace();
            if (*m_cur != ':')
                return func_failed(error_t::miss_colon);
            m_cur++;
            parse_whitespace();

            // parse value
            parse_value(val);
            if (m_error != error_t::ok)
                return func_failed(m_error);
            assert(key->type == type_t::string);
            object->insert(value_s::object_t::value_type(*key->string, val));
            parse_whitespace();
            if (*m_cur == ',') {
                m_cur++;
                parse_whitespace();
            } else if (*m_cur == '}') {
                m_cur++;
                return;
            } else
                return func_failed(error_t::miss_comma_or_curly_bracket);
        }
    }
};
} // namespace detail
} // namespace microlife
