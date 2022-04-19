#pragma once

#include "basic_json.h"

#include <assert.h> // assert()
#include <errno.h>  // errno(strtod)
#include <iostream> // os
#include <math.h>   // HUGE_VAL(strtod)

namespace microlife {
namespace detail {
/***
 * @brief JSON format lexer
 * @details The field type used by this class to parse strings.
 * @author qingl
 * @date 2022_04_09
 */
class lexer {
private:
    using boolean_t = basic_json::boolean_t;
    using number_t = basic_json::number_t;
    using string_t = basic_json::string_t;
    using array_t = basic_json::array_t;
    using object_t = basic_json::object_t;

    using char_t = char;

public:
    /// token types for the parser
    enum class token_t {
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
        end_of_input,    ///< indicating the end of the input buffer
    };

private:
    string_t::const_iterator m_it_cur, m_it_end; // the next character to parse
    char_t m_cur;                                // current character

    string_t m_buffer;       // parsed string value
    number_t m_value_number; // parsed number value

public:
    // init the lexer with the given input buffer
    void init(string_t::const_iterator begin, string_t::const_iterator end) {
        m_it_cur = begin;
        m_it_end = end;
        next_char();
    }

    // scan the next field, return token
    token_t scan();

    // returns the value parsed by scan, assert(token == value_number)
    number_t get_number() const { return m_value_number; }

    // returns the value parsed by scan, assert(token == value_string)
    string_t&& get_string() { return std::move(m_buffer); }

private:
    // scan string
    token_t scan_string();

    // scan number
    token_t scan_number();

private:
    // next character
    inline void next_char() {
        m_cur = (m_it_cur == m_it_end) ? '\0' : *m_it_cur++;
    }

    // skip all whitespaces
    inline void skip_whitespace() {
        while (is_whitespace())
            next_char();
    }

    // whether m_cur is a space
    inline bool is_whitespace() const {
        return m_cur == ' ' || m_cur == '\t' || m_cur == '\n' || m_cur == '\r';
    }

    // scan a literal, true/false/null
    token_t scan_literal(const char_t* literal_text, token_t return_type);
};

/***
 * @brief cout token_t
 * @author qingl
 * @date 2022_04_19
 */
inline std::ostream& operator<<(std::ostream& os, const lexer::token_t& token) {
    switch (token) {
    case lexer::token_t::literal_true:
        return os << "literal_true";

    case lexer::token_t::literal_false:
        return os << "literal_false";

    case lexer::token_t::literal_null:
        return os << "literal_null";

    case lexer::token_t::value_string:
        return os << "value_string";

    case lexer::token_t::value_number:
        return os << "value_number";

    case lexer::token_t::begin_array:
        return os << "begin_array";

    case lexer::token_t::begin_object:
        return os << "begin_object";

    case lexer::token_t::end_array:
        return os << "end_array";

    case lexer::token_t::end_object:
        return os << "end_object";

    case lexer::token_t::name_separator:
        return os << "name_separator";

    case lexer::token_t::value_separator:
        return os << "value_separator";

    case lexer::token_t::parse_error:
        return os << "parse_error";

    case lexer::token_t::end_of_input:
        return os << "end_of_input";

    default:
        return os << "unknown token_t";
    }
}
} // namespace detail
} // namespace microlife
