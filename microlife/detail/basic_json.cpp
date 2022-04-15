#include "microlife/detail/basic_json.h"

#include <assert.h>

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
            j.array->push_back(new basic_json(*i));
        }
        return j;
    }
    // object
    else if (m_type == value_t::object) {
        auto j = json_value(value_t::object);
        for (const auto& i : *this->m_value.object) {
            j.object->insert(
                std::make_pair(i.first, new basic_json(*i.second)));
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

        // default:
        // 在此之前，应当遍历 value_t 中所有值，不应执行带此处
        // assert(false);
        // break;
    }
}

// TODO: 改为非递归版本
void basic_json::json_value::destroy(const value_t t) {
    if (t == value_t::array) {
        for (auto i : *array)
            delete i;
        delete array;
    } else if (t == value_t::object) {
        for (auto i : *object) {
            assert(i.second != nullptr);
            i.second->m_value.destroy(i.second->m_type);
        }
        delete object;
    } else if (t == value_t::string) {
        delete string;
    }
}
