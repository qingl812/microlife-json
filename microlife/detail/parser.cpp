#include "microlife/detail/parser.h"

using namespace microlife;
using namespace microlife::detail;

bool parser::parse(const string_t& str, basic_json& json) {
    m_lexer.init(str.begin(), str.end());

    assert(m_stack_token.empty());
    assert(m_stack_json.empty());

    auto parse_json = basic_parse();
    // clean stacks
    while (!m_stack_token.empty())
        m_stack_token.pop();

    while (!m_stack_json.empty())
        m_stack_json.pop();

    if (parse_json != nullptr) {
        json = std::move(*parse_json);
        delete parse_json;
        parse_json = nullptr;
        return true;
    } else
        return false;
}

basic_json* parser::basic_parse() {
    // literal_null
    // 所有的值类型都是 literal_null

    while (true) {
        token_t token = m_lexer.scan();

        switch (token) {
        case token_t::parse_error:
            return nullptr;

        case token_t::end_of_input: {
            if (m_stack_token.size() == 1 &&
                m_stack_token.top() == token_t::literal_null &&
                m_stack_json.size() == 1) {
                return new basic_json(std::move(m_stack_json.top()));
            }
            return nullptr;
        }

        case token_t::end_array: {
            token = token_t::literal_null;

            auto json_array = parse_end_array();
            if (json_array == nullptr)
                return nullptr;
            else {
                m_stack_json.push(std::move(*json_array));
                delete json_array;
            }
            break;
        }

        case token_t::end_object: {
            token = token_t::literal_null;

            auto json_object = parse_end_object();
            if (json_object == nullptr)
                return nullptr;
            else {
                m_stack_json.push(std::move(*json_object));
                delete json_object;
            }
            break;
        }

        case token_t::literal_null:
            token = token_t::literal_null;
            m_stack_json.push(std::move(basic_json(nullptr)));
            break;

        case token_t::literal_true:
            token = token_t::literal_null;
            m_stack_json.push(std::move(basic_json(true)));
            break;

        case token_t::literal_false:
            token = token_t::literal_null;
            m_stack_json.push(std::move(basic_json(false)));
            break;

        case token_t::value_number:
            token = token_t::literal_null;
            m_stack_json.push(std::move(basic_json(m_lexer.get_number())));
            break;

        case token_t::value_string:
            token = token_t::literal_null;
            m_stack_json.push(std::move(basic_json(m_lexer.get_string())));
            break;

        case token_t::begin_array:
        case token_t::begin_object:
        case token_t::value_separator:
        case token_t::name_separator:
            break;

        default:
            assert(false);
            break;
        }

        m_stack_token.push(token);
    }
}

// token 为值类型时，插入到 array 末尾
#define PUSH_VALUE_TO_ARRAY()                                                  \
    do {                                                                       \
        m_stack_token.pop();                                                   \
        if (m_stack_json.empty())                                              \
            return nullptr;                                                    \
        array.push_back(std::move(m_stack_json.top()));                        \
        m_stack_json.pop();                                                    \
    } while (0)

basic_json* parser::parse_end_array() {
    if (m_stack_token.empty())
        return nullptr;

    // 如果第一个 value
    // 剩下的一定是 ,' + value
    // 最后则是 '['
    array_t array;
    if (m_stack_token.top() == token_t::literal_null) {
        PUSH_VALUE_TO_ARRAY();

        while (m_stack_token.size() >= 2) {
            if (m_stack_token.top() == token_t::begin_array)
                break;

            // ,' + value
            if (m_stack_token.top() != token_t::value_separator)
                return nullptr;
            m_stack_token.pop();
            if (m_stack_token.top() != token_t::literal_null)
                return nullptr;

            PUSH_VALUE_TO_ARRAY();
        }
    }

    // '['
    if (m_stack_token.empty() || m_stack_token.top() != token_t::begin_array)
        return nullptr;
    m_stack_token.pop();
    return new basic_json(std::move(array));
}

// token 为值类型时，插入到 object 末尾
#define PUSH_VALUE_TO_OBJECT()                                                 \
    do {                                                                       \
        m_stack_token.pop();                                                   \
        if (m_stack_token.top() != token_t::name_separator)                    \
            return nullptr;                                                    \
        m_stack_token.pop();                                                   \
        if (m_stack_token.top() != token_t::literal_null)                      \
            return nullptr;                                                    \
        m_stack_token.pop();                                                   \
        /* value */                                                            \
        if (m_stack_json.empty())                                              \
            return nullptr;                                                    \
        auto value = std::move(m_stack_json.top());                            \
        m_stack_json.pop();                                                    \
        /* key */                                                              \
        if (m_stack_json.empty())                                              \
            return nullptr;                                                    \
        auto key = std::move(m_stack_json.top());                              \
        m_stack_json.pop();                                                    \
        /* key.type == string */                                               \
        if (!key.is_string())                                                  \
            return nullptr;                                                    \
        object.emplace(std::move(key.get<string_t&>()), std::move(value));     \
    } while (0)

basic_json* parser::parse_end_object() {
    if (m_stack_token.empty())
        return nullptr;

    // 如果第一个 value : key
    // 剩下的一定是 ,' + value : key
    // 最后则是 '{'
    object_t object;
    if (m_stack_token.size() >= 4 &&
        m_stack_token.top() == token_t::literal_null) {
        PUSH_VALUE_TO_OBJECT();

        while (m_stack_token.size() >= 4) {
            if (m_stack_token.top() == token_t::begin_object)
                break;

            // ,' + value : key
            if (m_stack_token.top() != token_t::value_separator)
                return nullptr;
            m_stack_token.pop();
            if (m_stack_token.top() != token_t::literal_null)
                return nullptr;
            PUSH_VALUE_TO_OBJECT();
        }
    }

    // '{'
    if (m_stack_token.empty() || m_stack_token.top() != token_t::begin_object)
        return nullptr;
    m_stack_token.pop();
    return new basic_json(std::move(object));
}
