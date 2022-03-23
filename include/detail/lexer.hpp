#pragma once

#include "json_t.hpp"

#include <assert.h>
#include <errno.h> // errno(strtod)
#include <math.h>  // HUGE_VAL(strtod)

namespace microlife {
namespace detail {
class lexer_base {
protected:
    using boolean_t = json_t::boolean_t;
    using number_t = json_t::number_t;
    using string_t = json_t::string_t;
    using array_t = json_t::array_t;
    using object_t = json_t::object_t;

public:
    /// token types for the parser
    enum class token_t
    {
        literal_true,    ///< the `true` literal
        literal_false,   ///< the `false` literal
        literal_null,    ///< the `null` literal
        value_string,    ///< a string -- use get_string() for actual value
        value_number,    ///< a number -- use get_number() for actual value
        begin_array,     ///< the character for array begin `[`
        begin_object,    ///< the character for object begin `{`
        end_array,       ///< the character for array end `]`
        end_object,      ///< the character for object end `}`
        name_separator,  ///< the name separator `:`
        value_separator, ///< the value separator `,`
        parse_error,     ///< indicating a parse error
        end_of_input     ///< indicating the end of the input buffer
    };

    /// return name of values of type token_type (only used for errors)
    static const char* token_type_name(const token_t t) noexcept {
        switch (t) {
        case token_t::literal_true:
            return "true literal";
        case token_t::literal_false:
            return "false literal";
        case token_t::literal_null:
            return "null literal";
        case token_t::value_string:
            return "string literal";
        case token_t::value_number:
            return "number literal";
        case token_t::begin_array:
            return "'['";
        case token_t::begin_object:
            return "'{'";
        case token_t::end_array:
            return "']'";
        case token_t::end_object:
            return "'}'";
        case token_t::name_separator:
            return "':'";
        case token_t::value_separator:
            return "','";
        case token_t::parse_error:
            return "<parse error>";
        case token_t::end_of_input:
            return "end of input";
            // LCOV_EXCL_START
        default: // catch non-enum values
            return "unknown token";
            // LCOV_EXCL_STOP
        }
    }
};

class lexer : public lexer_base {
private:
    using char_t = char;

    string_t::const_iterator m_cur; // 指向 需要解析的下一个字符 的指针
    string_t m_buffer;              // 解析出来的字符串的缓冲区
    // number values
    number_t m_value_number;

public:
    void init(string_t::const_iterator begin, string_t::const_iterator end) {
        m_cur = begin;
        assert(*end == '\0');
    }

    token_t scan() {
        skip_whitespace();

        switch (*m_cur) {
            // structural characters
        case '[':
            m_cur++;
            return token_t::begin_array;
        case ']':
            m_cur++;
            return token_t::end_array;
        case '{':
            m_cur++;
            return token_t::begin_object;
        case '}':
            m_cur++;
            return token_t::end_object;
        case ':':
            m_cur++;
            return token_t::name_separator;
        case ',':
            m_cur++;
            return token_t::value_separator;

            // literals
        case 't':
            return scan_literal("rue", token_t::literal_true);
        case 'f':
            return scan_literal("alse", token_t::literal_false);
        case 'n':
            return scan_literal("ull", token_t::literal_null);

        // string
        case '\"':
            return scan_string();

            // number
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return scan_number();

            // end of input (the null byte is needed when parsing from
            // string literals)
        case '\0':
            return token_t::end_of_input;

            // error
        default:
            return token_t::parse_error;
        }
    }

    number_t get_number() const { return m_value_number; }

    string_t&& get_string() { return std::move(m_buffer); }

private:
    // 解析所有空格
    void skip_whitespace() {
        while (is_whitespace())
            m_cur++;
    }

    token_t scan_literal(const char_t* literal_text, token_t return_type) {
        m_cur++;
        while (*literal_text != '\0') {
            if (*m_cur != *literal_text)
                return token_t::parse_error;
            m_cur++;
            literal_text++;
        }

        return return_type;
    }

    // 解析 string
    token_t scan_string() {
        assert(*m_cur == '\"');
        m_buffer.clear();

        while (true) {
            m_cur++;
            switch (*m_cur) {
            case '\0':
                return token_t::parse_error; // 缺失右引号 '"'

            case '\"':
                m_cur++;
                return token_t::value_string;

            case '\\': {
                switch (*++m_cur) {
                case '\"':
                case '\\':
                    m_buffer.push_back(*m_cur);
                    break;
                case 'b':
                    m_buffer.push_back('\b');
                    break;
                case 'f':
                    m_buffer.push_back('\f');
                    break;
                case 'n':
                    m_buffer.push_back('\n');
                    break;
                case 'r':
                    m_buffer.push_back('\r');
                    break;
                case 't':
                    m_buffer.push_back('\t');
                    break;
                case 'u': {
                    // 解析 utf-8
                    // 解析 4 位的 16 进制数
                    auto parse_hex4 = [&](unsigned& u) {
                        u = 0;
                        for (int i = 0; i < 4; i++) {
                            m_cur++;
                            char ch = *m_cur;
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

                    unsigned u, u2;
                    if (!parse_hex4(u))
                        return token_t::parse_error;
                    // surrogate pair
                    if (u >= 0xD800 && u <= 0xDBFF) {
                        if (*++m_cur != '\\' || *++m_cur != 'u' ||
                            !parse_hex4(u2) || (u2 < 0xDC00 || u2 > 0xDFFF))
                            return token_t::parse_error;
                        u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
                    }
                    // 为什么要做 x & 0xFF 这种操作呢？
                    // 这是因为 u 是 unsigned
                    // 类型，一些编译器可能会警告这个转型可能会截断数据。
                    // 但实际上，配合了范围的检测然后右移之后，可以保证写入的是
                    // 0~255 内的值。 为了避免一些编译器的警告误判，我们加上 x &
                    // 0xFF。
                    // 一般来说，编译器在优化之后，这与操作是会被消去的，不会影响性能。
                    if (u <= 0x7F)
                        m_buffer.push_back(u & 0xFF);
                    else if (u <= 0x7FF) {
                        m_buffer.push_back(0xC0 | ((u >> 6) & 0xFF));
                        m_buffer.push_back(0x80 | (u & 0x3F));
                    } else if (u <= 0xFFFF) {
                        m_buffer.push_back(0xE0 | ((u >> 12) & 0xFF));
                        m_buffer.push_back(0x80 | ((u >> 6) & 0x3F));
                        m_buffer.push_back(0x80 | (u & 0x3F));
                    } else {
                        assert(u <= 0x10FFFF);
                        m_buffer.push_back(0xF0 | ((u >> 18) & 0xFF));
                        m_buffer.push_back(0x80 | ((u >> 12) & 0x3F));
                        m_buffer.push_back(0x80 | ((u >> 6) & 0x3F));
                        m_buffer.push_back(0x80 | (u & 0x3F));
                    }
                    break;
                }
                default:
                    return token_t::parse_error;
                }
                break;
            }

            default:
                if ((unsigned char)*m_cur < 0x20) {
                    return token_t::parse_error;
                }
                m_buffer.push_back(*m_cur);
                break;
            }
        }
    }

    // 解析数字
    // 使用标准库的 strtod() 来进行转换
    // strtod() 可转换 JSON 所要求的格式，但问题是，一些 JSON 不容许的格式
    // strtod() 也可转换，所以我们需要自行做格式校验。
    // strtod() https://en.cppreference.com/w/c/string/byte/strtof
    token_t scan_number() {
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
            return token_t::parse_error;

        if (*p == '.') {
            p++;
            // 至少要有一个数字
            // 移动指针到下一个非数字的字符
            if (!isDigital(*p))
                return token_t::parse_error;
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
                return token_t::parse_error;
            do {
                p++;
            } while (isDigital(*p));
        }

        // 数字格式校验正确
        // strtod
        errno = 0;
        m_value_number = strtod(string_t(m_cur, p).c_str(), nullptr);
        // 数字过大
        if (errno == ERANGE &&
            (m_value_number == HUGE_VAL || m_value_number == -HUGE_VAL)) {
            return token_t::parse_error;
        }

        m_cur = p;
        return token_t::value_number;
    }

private:
    bool is_whitespace() {
        return *m_cur == ' ' || *m_cur == '\t' || *m_cur == '\n' ||
               *m_cur == '\r';
    }
};
} // namespace detail
} // namespace microlife