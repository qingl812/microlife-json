#include "microlife/detail/lexer.h"

using namespace microlife;
using namespace microlife::detail;

void lexer::init(string_t::const_iterator begin, string_t::const_iterator end) {
    assert(*end == '\0');
    m_cur = begin;
}

bool lexer::is_whitespace() const {
    return *m_cur == ' ' || *m_cur == '\t' || *m_cur == '\n' || *m_cur == '\r';
}

void lexer::skip_whitespace() {
    while (is_whitespace())
        m_cur++;
}

lexer::token_t lexer::scan() {
    skip_whitespace();

    switch (*m_cur) {
    // structural characters
    case '[':
        m_cur++;
        m_token = token_t::begin_array;
        break;

    case ']':
        m_cur++;
        m_token = token_t::end_array;
        break;

    case '{':
        m_cur++;
        m_token = token_t::begin_object;
        break;

    case '}':
        m_cur++;
        m_token = token_t::end_object;
        break;

    case ':':
        m_cur++;
        m_token = token_t::name_separator;
        break;

    case ',':
        m_cur++;
        m_token = token_t::value_separator;
        break;

    // literals
    case 't':
        m_token = scan_literal("rue", token_t::literal_true);
        break;

    case 'f':
        m_token = scan_literal("alse", token_t::literal_false);
        break;

    case 'n':
        m_token = scan_literal("ull", token_t::literal_null);
        break;

    // string
    case '\"':
        m_token = scan_string();
        break;

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
        m_token = scan_number();
        break;

        // end of input (the null byte is needed when parsing from
        // string literals)
    case '\0':
        m_token = token_t::end_of_input;
        break;

        // error
    default:
        m_token = token_t::parse_error;
        break;
    }

    return m_token;
}

lexer::token_t lexer::scan_string() {
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

// 使用标准库的 strtod() 来进行转换
// strtod() 可转换 JSON 所要求的格式，但问题是，一些 JSON 不容许的格式
// strtod() 也可转换，所以我们需要自行做格式校验。
// strtod() https://en.cppreference.com/w/c/string/byte/strtof
lexer::token_t lexer::scan_number() {
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

lexer::token_t lexer::scan_literal(const char_t* literal_text,
                                   token_t return_type) {
    m_cur++;
    while (*literal_text != '\0') {
        if (*m_cur != *literal_text)
            return token_t::parse_error;
        m_cur++;
        literal_text++;
    }

    return return_type;
}

lexer::number_t lexer::get_number() const {
    assert(m_token == token_t::value_number);
    return m_value_number;
}

lexer::string_t&& lexer::get_string() {
    assert(m_token == token_t::value_string);
    return std::move(m_buffer);
}
