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
    lexer m_lexer;
    stack_token_t m_tokens;

public:
    parser() = default;
    ~parser() = default;

    static bool parse(const string_t& str, basic_json& value);

private:
    void clear();

    // token 是一个有效的 json 值种类
    // 而不是 ‘:' ',' 这种
    // 如果是 array or object
    // token == literal_null
    inline void assert_token_is_value(const token_t& token_is_value);

    basic_json* basic_parse(const string_t& str);
};
} // namespace detail
} // namespace microlife
