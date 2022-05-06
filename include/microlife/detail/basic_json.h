#pragma once
#include "lexer.hpp"
#include "macro_scope.hpp"
#include "macro_scope.hpp" // json_assert()
#include "parser.hpp"
#include "value_t.hpp"

#include <algorithm> // sort
#include <map>       // object_t
#include <sstream>   // ostringstream
#include <string.h>  // strcmp
#include <string>    // string_t
#include <vector>    // vector_t

namespace microlife {
namespace detail {
/***
 * @brief Basic JSON class
 * @details This class is used to store JSON data.
 * @author qingl
 * @date 2022_04_14
 */
class basic_json {
public:
    using boolean_t = bool;
    using number_t = double;
    using string_t = std::string;
    using array_t = std::vector<basic_json>;
    using object_t = std::map<string_t, basic_json>;

    using value_t = detail::value_t;

private:
    using parser =
        ::microlife::detail::parser<::microlife::detail::lexer, basic_json>;

    // private
    JSON_PRIVATE_UNLESS_TESTED

    /***
     * @brief json value union
     * @details The real place to store the json value
     * @author qingl
     * @date 2022_04_14
     */
    union json_value {
        boolean_t boolean;
        number_t number;
        string_t* string;
        array_t* array;
        object_t* object;

        // 请不要使用 char* 初始化 json_value
        // 会被自动转换为 boolean_t
        json_value() = default;
        json_value(boolean_t v) noexcept : boolean(v) {}
        json_value(number_t v) noexcept : number(v) {}
        json_value(int v) noexcept : number(v) {}

        json_value(const string_t& value) : string(new string_t(value)) {}
        json_value(const object_t& value) : object(new object_t(value)) {}
        json_value(const array_t& value) : array(new array_t(value)) {}

        json_value(string_t&& value) : string(new string_t(std::move(value))) {}
        json_value(object_t&& value) : object(new object_t(std::move(value))) {}
        json_value(array_t&& value) : array(new array_t(std::move(value))) {}

        json_value(value_t t) {
            switch (t) {
            case value_t::object:
                object = new object_t();
                break;

            case value_t::array:
                array = new array_t();
                break;

            case value_t::string:
                string = new string_t();
                break;

            case value_t::boolean:
                boolean = false;
                break;

            case value_t::number:
                number = number_t(0);
                break;

            default:
            case value_t::null:
                object = nullptr;
                break;
            }
        }

        // TODO: 改为非递归版本
        void destroy(const value_t t) {
            if (t == value_t::array) {
                delete array;
            } else if (t == value_t::object) {
                delete object;
            } else if (t == value_t::string) {
                delete string;
            }
        }
    };

private:
    value_t m_type;
    json_value m_value;

public:
    // 基本构造函数
    basic_json(std::nullptr_t = nullptr) : m_type(value_t::null), m_value() {}
    basic_json(boolean_t v) : m_type(value_t::boolean), m_value(v) {}
    basic_json(number_t v) : m_type(value_t::number), m_value(v) {}
    basic_json(int v) : m_type(value_t::number), m_value(v) {}
    basic_json(const char* v)
        : m_type(value_t::string), m_value(std::move(string_t(v))) {}

    // 拷贝构造函数
    basic_json(const string_t& v) : m_type(value_t::string), m_value(v) {}
    basic_json(const array_t& v) : m_type(value_t::array), m_value(v) {}
    basic_json(const object_t& v) : m_type(value_t::object), m_value(v) {}

    // 移动构造函数
    basic_json(string_t&& v) : m_type(value_t::string), m_value(std::move(v)) {}
    basic_json(array_t&& v) : m_type(value_t::array), m_value(std::move(v)) {}
    basic_json(object_t&& v) : m_type(value_t::object), m_value(std::move(v)) {}

    // 按类型构造 basic_json
    basic_json(value_t v) : m_type(v), m_value(v) {}

    // 构造函数
    basic_json(const basic_json& v)
        : m_type(v.m_type), m_value(v.deep_copy()) {}

    basic_json(basic_json&& other) noexcept {
        m_type = other.m_type;
        m_value = other.m_value;

        other.m_type = value_t::null;
    }

    basic_json(size_t cnt, const basic_json& val)
        : m_type(value_t::array), m_value(value_t::array) {
        while (cnt--) {
            m_value.array->push_back(val);
        }
    }

    // 析构函数
    virtual ~basic_json() { m_value.destroy(m_type); }

public:
    value_t type() const { return m_type; }

    // is_type() ?
    bool is_null() const { return m_type == value_t::null; }
    bool is_boolean() const { return m_type == value_t::boolean; }
    bool is_number() const { return m_type == value_t::number; }
    bool is_string() const { return m_type == value_t::string; }
    bool is_array() const { return m_type == value_t::array; }
    bool is_object() const { return m_type == value_t::object; }

    // T get<T>()
    template <typename T>
    T get() const {
        // bool
        if constexpr (std::is_same_v<T, bool>) {
            json_assert(is_boolean());
            return m_value.boolean;
        }
        // int
        else if constexpr (std::is_same_v<T, int>) {
            json_assert(is_number());
            return static_cast<int>(m_value.number);
        }
        // double
        else if constexpr (std::is_same_v<T, double>) {
            json_assert(is_number());
            return m_value.number;
        }

        // string
        else if constexpr (std::is_same_v<T, std::string>) {
            json_assert(is_string());
            return *m_value.string;
        }
        // string&
        else if constexpr (std::is_same_v<T, std::string&>) {
            json_assert(is_string());
            return *m_value.string;
        }
        // const string&
        else if constexpr (std::is_same_v<T, const std::string&>) {
            json_assert(is_string());
            return *m_value.string;
        }

        // vector<basic_json>
        else if constexpr (std::is_same_v<T, std::vector<basic_json>>) {
            json_assert(is_array());
            return *m_value.array;
        }
        // vector<basic_json>&
        else if constexpr (std::is_same_v<T, std::vector<basic_json>&>) {
            json_assert(is_array());
            return *m_value.array;
        }
        // const vector<basic_json>&
        else if constexpr (std::is_same_v<T, const std::vector<basic_json>&>) {
            json_assert(is_array());
            return *m_value.array;
        }

        // map<string, basic_json>
        else if constexpr (std::is_same_v<T,
                                          std::map<std::string, basic_json>>) {
            json_assert(is_object());
            return *m_value.object;
        }
        // map<string, basic_json>&
        else if constexpr (std::is_same_v<T,
                                          std::map<std::string, basic_json>&>) {
            json_assert(is_object());
            return *m_value.object;
        }
        // const map<string, basic_json>&
        else if constexpr (std::is_same_v<
                               T, const std::map<std::string, basic_json>&>) {
            asjson_assertsert(is_object());
            return *m_value.object;
        }

        // else
        else {
            static_assert(std::is_same_v<T, std::string>,
                          "basic_json::get<T>() : T is not supported");
        }
    }

    // TODO: 改为非递归版本
    // get a string representation of a JSON value (serialize)
    string_t dump() const {
        switch ((m_type)) {
        default:
        case value_t::null:
            return "null";

        case value_t::boolean:
            if (m_value.boolean)
                return "true";
            else
                return "false";

        case value_t::number:
            return number_to_dump(m_value.number);

        case value_t::string:
            return string_to_dump(*m_value.string);

        case value_t::array: {
            string_t ret;
            ret.push_back('[');
            if (m_value.array->size() > 0) {
                for (auto& i : *m_value.array) {
                    ret += i.dump();
                    ret.push_back(',');
                }
                ret.pop_back();
            }
            ret.push_back(']');
            return ret;
        }

        case value_t::object: {
            string_t ret;
            ret.push_back('{');
            if (m_value.object->size() > 0) {
                for (const auto& i : *m_value.object) {
                    ret += '\"' + i.first + '\"' + ':';
                    ret += i.second.dump();
                    ret.push_back(',');
                }
                ret.pop_back();
            }
            ret.push_back('}');
            return ret;
        }
        }
    }

    // parse a string into a JSON value (deserialize)
    bool parse(const string_t& str) {
        static parser p;
        return p.parse(str, *this);
    }

public:
    // 赋值函数
    basic_json& operator=(const basic_json& other) {
        m_value.destroy(m_type);

        m_type = other.m_type;
        m_value = other.deep_copy();

        return *this;
    }

    basic_json& operator=(basic_json&& other) noexcept {
        m_value.destroy(m_type);

        m_type = other.m_type;
        m_value = other.m_value;

        other.m_type = value_t::null;
        other.m_value = value_t(value_t::null);
        return *this;
    }

    bool operator==(const basic_json& other) const {
        return compare(*this, other) == 0;
    }

    /***
     * @brief compare baisc_json
     * @details
     * left == right, return 0
     * left < right, return -1
     * left > right, return 1
     * @author qingl
     * @date 2022_04_18
     */
    static int8_t compare(const basic_json& left, const basic_json& right) {
        if (left.m_type != right.m_type) {
            return left.m_type < right.m_type ? -1 : 1;
        }

        const auto& left_v = left.m_value;
        const auto& right_v = right.m_value;
        switch (left.m_type) {
        default:
        case value_t::null:
            return 0;

        case value_t::boolean:
            return left_v.boolean == right_v.boolean
                       ? 0
                       : (left_v.boolean ? 1 : -1);

        case value_t::number:
            return left_v.number == right_v.number
                       ? 0
                       : (left_v.number > right_v.number ? 1 : -1);

        case value_t::string:
            return int8_t(
                strcmp(left_v.string->c_str(), right_v.string->c_str()));

        case value_t::array: {
            int diff = left_v.array->size() - right_v.array->size();
            if (diff != 0) {
                return diff > 0 ? 1 : -1;
            }

            auto a = *left_v.array;
            auto b = *right_v.array;
            auto cmp = [](const basic_json& a, const basic_json& b) {
                return compare(a, b) <= 0;
            };
            std::sort(a.begin(), a.end(), cmp);
            std::sort(b.begin(), b.end(), cmp);
            // compare
            for (auto it_a = a.begin(), it_b = b.begin(); it_a != a.end();
                 it_a++, it_b++) {
                auto diff = compare(*it_a, *it_b);
                if (diff != 0) {
                    return diff;
                }
            }
            return 0;
        }

        case value_t::object: {
            int diff = left_v.object->size() - right_v.object->size();
            if (diff != 0) {
                return diff > 0 ? 1 : -1;
            }
            // compare
            for (auto it_a = left_v.object->begin(),
                      it_b = right_v.object->begin();
                 it_a != left_v.object->end(); it_a++, it_b++) {
                auto diff = compare(it_a->second, it_b->second);
                if (diff != 0) {
                    return diff;
                }
            }
            return 0;
        }
        }
    }

private:
    // 用于深拷贝一个 basic_json 值
    json_value deep_copy() const {
        // string
        if (m_type == value_t::string) {
            return json_value(*m_value.string);
        }
        // array
        else if (m_type == value_t::array) {
            auto j = json_value(value_t::array);
            for (const auto& i : *this->m_value.array) {
                j.array->push_back(std::move(basic_json(i)));
            }
            return j;
        }
        // object
        else if (m_type == value_t::object) {
            auto j = json_value(value_t::object);
            for (const auto& i : *this->m_value.object) {
                j.object->insert(
                    std::make_pair(i.first, std::move(basic_json(i.second))));
            }
            return j;
        }
        // else
        else {
            return m_value;
        }
    }

    // number_t to string_t
    inline static string_t number_to_dump(number_t number) {
        char buffer[32];
        sprintf(buffer, "%.17g", number);
        return buffer;
    }

    // string_t to dump
    inline static string_t string_to_dump(const string_t& string) {
        static const char hex_digits[] = {'0', '1', '2', '3', '4', '5',
                                          '6', '7', '8', '9', 'A', 'B',
                                          'C', 'D', 'E', 'F'};
        string_t ret;

        ret.push_back('\"');
        auto it = string.begin();
        while (it != string.end()) {
            unsigned char ch = (unsigned char)*it;

            switch (ch) {
            case '\"':
                ret.push_back('\\');
                ret.push_back('\"');
                break;

            case '\\':
                ret.push_back('\\');
                ret.push_back('\\');
                break;

            case '\b':
                ret.push_back('\\');
                ret.push_back('b');
                break;

            case '\f':
                ret.push_back('\\');
                ret.push_back('f');
                break;

            case '\n':
                ret.push_back('\\');
                ret.push_back('n');
                break;

            case '\r':
                ret.push_back('\\');
                ret.push_back('r');
                break;

            case '\t':
                ret.push_back('\\');
                ret.push_back('t');
                break;

            default:
                if (ch < 0x20) {
                    ret.push_back('\\');
                    ret.push_back('u');
                    ret.push_back('0');
                    ret.push_back('0');
                    ret.push_back(hex_digits[ch >> 4]);
                    ret.push_back(hex_digits[ch & 15]);
                } else
                    ret.push_back(*it);
            }

            it++;
        }
        ret.push_back('\"');
        return ret;
    }
};

// cout baisc_json
inline std::ostream& operator<<(std::ostream& os, const basic_json& type) {
    return os << type.dump();
}
} // namespace detail
} // namespace microlife
