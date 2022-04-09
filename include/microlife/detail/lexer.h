#pragma once

#include "basic_json.h"

#include <assert.h> // assert()
#include <errno.h>  // errno(strtod)
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
        end_of_input,    ///< indicating the end of the input buffer
    };

private:
    string_t::const_iterator m_cur; // the next character to parse
    string_t m_buffer;              // parsed string value
    number_t m_value_number;        // parsed number value
    token_t m_token;                // the last token type

public:
    // init the lexer with the given input buffer
    void init(string_t::const_iterator begin, string_t::const_iterator end);

    // scan the next field, return token
    token_t scan();

    // returns the value parsed by scan, assert(token == value_number)
    number_t get_number() const;

    // returns the value parsed by scan, assert(token == value_string)
    string_t&& get_string();

private:
    // scan string
    token_t scan_string();

    // scan number
    token_t scan_number();

private:
    // skip all whitespaces
    inline void skip_whitespace();

    // whether m_cur is a space
    inline bool is_whitespace() const;

    // scan a literal, true/false/null
    inline token_t scan_literal(const char_t* literal_text,
                                token_t return_type);
};
} // namespace detail
} // namespace microlife
