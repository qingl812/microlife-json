#include "microlife/detail/basic_json.h"
#include "microlife/detail/parser.h"

#include <algorithm> // sort
#include <assert.h>
#include <string.h> // strcmp

using namespace microlife;
using namespace microlife::detail;

// 用于深拷贝一个 basic_json 值
basic_json::json_value basic_json::deep_copy() const {
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

// json_value
basic_json::json_value::json_value(value_t t) {
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

    case value_t::null:
        object = nullptr;
        break;

    default:
        assert(false);
        break;
    }
}

// TODO: 改为非递归版本
void basic_json::json_value::destroy(const value_t t) {
    if (t == value_t::array) {
        delete array;
    } else if (t == value_t::object) {
        delete object;
    } else if (t == value_t::string) {
        delete string;
    }
}

// TODO: 改为非递归版本, 特殊字符处理
basic_json::string_t basic_json::dump() const {
    switch ((m_type)) {
    case value_t::null:
        return "null";

    case value_t::boolean:
        if (m_value.boolean)
            return "true";
        else
            return "false";

    case value_t::number: {
        string_t ret = std::to_string(m_value.number);
        while (ret.back() == '0')
            ret.pop_back();
        if (ret.back() == '.')
            ret.pop_back();
        return ret;
    }

    case value_t::string:
        return '\"' + *m_value.string + '\"';

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

    default:
        assert(false);
        return "";
    }
}

bool basic_json::parse(const string_t& str) {
    static parser p;
    return p.parse(str, *this);
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
int8_t basic_json::compare(const basic_json& left, const basic_json& right) {
    auto diff = int8_t(left.m_type) - int8_t(right.m_type);
    if (diff != 0) {
        return diff > 0 ? 1 : -1;
    }

    const auto& left_v = left.m_value;
    const auto& right_v = right.m_value;
    switch (left.m_type) {
    case value_t::null:
        return 0;

    case value_t::boolean:
        return left_v.boolean == right_v.boolean ? 0
                                                 : (left_v.boolean ? 1 : -1);

    case value_t::number:
        return left_v.number == right_v.number
                   ? 0
                   : (left_v.number > right_v.number ? 1 : -1);

    case value_t::string:
        return int8_t(strcmp(left_v.string->c_str(), right_v.string->c_str()));

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
        for (auto it_a = left_v.object->begin(), it_b = right_v.object->begin();
             it_a != left_v.object->end(); it_a++, it_b++) {
            auto diff = compare(it_a->second, it_b->second);
            if (diff != 0) {
                return diff;
            }
        }
        return 0;
    }

    default:
        assert(false);
        return 0;
    }
}
