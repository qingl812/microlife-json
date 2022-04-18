#include "microlife/detail/parser.h"

using namespace microlife;
using namespace microlife::detail;

bool parser::parse(const string_t& str, basic_json& value) {
    static parser par;
    auto json = par.basic_parse(str);
    if (json) {
        value = std::move(*json);
        delete json;
        return true;
    } else {
        value = basic_json();
        par.clear();
        return false;
    }
}

void parser::clear() {
    while (!m_tokens.empty()) {
        m_tokens.pop();
    }
}

void parser::assert_token_is_value(const token_t& token_is_value) {
    assert(token_is_value == token_t::literal_null ||
           token_is_value == token_t::literal_true ||
           token_is_value == token_t::literal_false ||
           token_is_value == token_t::value_number ||
           token_is_value == token_t::value_string);

    assert(token_is_value != token_t::begin_array &&
           token_is_value != token_t::begin_object &&
           token_is_value != token_t::end_array &&
           token_is_value != token_t::end_object &&
           token_is_value != token_t::value_separator &&
           token_is_value != token_t::name_separator &&
           token_is_value != token_t::end_of_input &&
           token_is_value != token_t::parse_error);
}

basic_json* parser::basic_parse(const string_t& str) {
    // init
    m_lexer.init(str.begin(), str.end());
    clear();

    // parse
    while (true) {
        token_t token;
        basic_json* json;
        token = m_lexer.scan();

        switch (token) {
        case token_t::parse_error:
            return nullptr;

        case token_t::end_of_input: {
            if (m_tokens.size() == 1) {
                std::tie(token, json) = m_tokens.top();
                m_tokens.pop();
                if (json) {
                    assert_token_is_value(token);
                    return json;
                }
            }
            // error
            return nullptr;
        }

        case token_t::end_array: {
            if (m_tokens.empty())
                return nullptr;
            token = token_t::literal_null;
            array_t array;

            // m_tokens 里面的可能的合法值
            // m_tokens: '['
            // m_tokens: '[' or ',' + value + ,' + value
            if (m_tokens.top().first == token_t::begin_array) {
                m_tokens.pop();
                json = new basic_json(std::move(array));
                break; // 跳出 switch
            }

            // '[' or ',' + value
            while (true) {
                if (m_tokens.size() >= 2) {
                    auto temp_a = m_tokens.top(); // value
                    m_tokens.pop();
                    auto temp_b = m_tokens.top(); // ',' | '['
                    m_tokens.pop();

                    if (temp_a.second) {
                        assert_token_is_value(temp_a.first);
                        array.push_back(std::move(*temp_a.second));

                        if (temp_b.first == token_t::begin_array)
                            break; // 解析成功
                        else if (temp_b.first == token_t::value_separator)
                            continue; // 解析下一个值
                    }
                    // 解析失败
                    if (temp_b.second)
                        delete temp_b.second;
                }
                return nullptr;
            }
            // 解析成功
            json = new basic_json(std::move(array));
            break;
        }

        case token_t::end_object: {
            if (m_tokens.empty())
                return nullptr;
            token = token_t::literal_null;
            object_t object;

            // m_tokens 里面的可能的合法值
            // m_tokens: '{'
            // m_tokens: '{' or ',' + key + ':' + value
            if (m_tokens.top().first == token_t::begin_object) {
                m_tokens.pop();
                json = new basic_json(std::move(object));
                break; // 跳出 switch
            }

            // '{' or ',' + key + ':' + value
            while (true) {
                if (m_tokens.size() >= 4) {
                    auto temp_a = m_tokens.top(); // value
                    m_tokens.pop();
                    auto temp_b = m_tokens.top(); // ':'
                    m_tokens.pop();
                    auto temp_c = m_tokens.top(); // key
                    m_tokens.pop();
                    auto temp_d = m_tokens.top(); // '{' or ','
                    m_tokens.pop();

                    bool sign = temp_c.first == token_t::value_string &&
                                temp_b.first == token_t::name_separator &&
                                temp_a.second;
                    if (sign) {
                        assert_token_is_value(temp_a.first);
                        assert(temp_c.second->is_string());

                        object.insert(
                            std::make_pair(temp_c.second->get<string_t&>(),
                                           std::move(*temp_a.second)));
                        delete temp_c.second;

                        if (temp_d.first == token_t::begin_object)
                            break; // 解析成功
                        else if (temp_d.first == token_t::value_separator)
                            continue; // 解析下一个值
                    }
                    // 解析失败
                    if (temp_d.second)
                        delete temp_d.second;
                    // if sign, key and value 已经插入到 object 中
                    if (!sign) {
                        if (temp_a.second)
                            delete temp_a.second;
                        if (temp_b.second)
                            delete temp_b.second;
                        if (temp_c.second)
                            delete temp_c.second;
                    }
                }
                return nullptr;
            }

            // 解析成功
            json = new basic_json(std::move(object));
            break;
        }

        case token_t::literal_null:
            json = new basic_json();
            break;

        case token_t::literal_true:
            json = new basic_json(true);
            break;

        case token_t::literal_false:
            json = new basic_json(false);
            break;

        case token_t::value_number:
            json = new basic_json(m_lexer.get_number());
            break;

        case token_t::value_string:
            json = new basic_json(std::move(m_lexer.get_string()));
            break;

        case token_t::begin_array:
        case token_t::begin_object:
        case token_t::value_separator:
        case token_t::name_separator:
            json = nullptr;
            break;

        default:
            assert(false);
            break;
        }

        if (token != token_t::begin_array && token != token_t::begin_object &&
            token != token_t::value_separator &&
            token != token_t::name_separator) {
            assert_token_is_value(token);
            assert(json != nullptr);
        }

        m_tokens.push(std::make_pair(token, json));
    }
}
