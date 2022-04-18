#pragma once

#include "lexer.h"

#include <assert.h>
#include <memory> // std::unique_ptr
#include <stack>  // stack for parse

namespace microlife {
namespace detail {
/***
 * @brief JSON parser
 * @details Parses JSON string and returns a tree of nodes
 * @author qingl
 * @date 2022_04_09
 */
class parser {
private:
    using boolean_t = basic_json::boolean_t;
    using number_t = basic_json::number_t;
    using string_t = basic_json::string_t;
    using array_t = basic_json::array_t;
    using object_t = basic_json::object_t;
    using value_t = basic_json::value_t;

    using token_t = lexer::token_t;

    using stack_token_t = std::stack<std::pair<token_t, basic_json*>>;

private:
    lexer m_lexer;                       // lexer
    std::stack<token_t> m_stack_token;   // stack for tokens
    std::stack<basic_json> m_stack_json; // stack for json values

public:
    parser() {}
    ~parser() { }

    bool parse(const string_t& str, basic_json& json);

private:
    basic_json* basic_parse();
    // if return nullptr, then error
    basic_json* parse_end_array();
    basic_json* parse_end_object();
};
} // namespace detail
} // namespace microlife
