#pragma once
#include <cstdint>  // uint8_t
#include <iostream> // os << value_t

namespace microlife {
namespace detail {
/***
 * @brief token types for the lexer
 * @author qingl
 * @date 2022_04_19
 */
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

/***
 * @brief cout token_t
 * @author qingl
 * @date 2022_04_19
 */
inline std::ostream& operator<<(std::ostream& os,
                                const token_t& token) noexcept {
    switch (token) {
    case token_t::literal_true:
        return os << "literal_true";

    case token_t::literal_false:
        return os << "literal_false";

    case token_t::literal_null:
        return os << "literal_null";

    case token_t::value_string:
        return os << "value_string";

    case token_t::value_number:
        return os << "value_number";

    case token_t::begin_array:
        return os << "begin_array";

    case token_t::begin_object:
        return os << "begin_object";

    case token_t::end_array:
        return os << "end_array";

    case token_t::end_object:
        return os << "end_object";

    case token_t::name_separator:
        return os << "name_separator";

    case token_t::value_separator:
        return os << "value_separator";

    case token_t::parse_error:
        return os << "parse_error";

    case token_t::end_of_input:
        return os << "end_of_input";

    default:
        return os << "unknown token_t";
    }
}
} // namespace detail
} // namespace microlife
