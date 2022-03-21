#pragma once

#include "value_t.hpp"

#include <assert.h>
#include <map>    // object_t
#include <string> // string_t
#include <vector> // vector_t

namespace microlife {
namespace detail {
class json_t {
public:
    using boolean_t = bool;
    using number_t = double;
    using string_t = std::string;
    using array_t = std::vector<json_t*>;
    using object_t = std::map<std::string, json_t*>;

    using value_t = detail::value_t;

protected:
    union json_value {
        boolean_t boolean;
        number_t number;
        string_t* string;
        array_t* array;
        object_t* object;

        json_value() = default;
        json_value(boolean_t v) : boolean(v) {}
        json_value(number_t v) : number(v) {}

        json_value(string_t&& value) : string(new string_t(std::move(value))) {}
        json_value(object_t&& value) : object(new object_t(std::move(value))) {}
        json_value(array_t&& value) : array(new array_t(std::move(value))) {}

        void destroy(const value_t t) {
            if (t == value_t::array) {
                for (auto i : *array)
                    delete i;
                delete array;
            } else if (t == value_t::object) {
                for (auto i : *object)
                    i.second->m_value.destroy(i.second->m_type);
                delete object;
            } else if (t == value_t::string) {
                delete object;
            }
        }
    };

protected:
    value_t m_type;
    json_value m_value;

public:
    json_t() noexcept : m_type(value_t::null), m_value() {}

    json_t(const boolean_t v) : m_type(value_t::boolean), m_value(v) {}

    json_t(const number_t v) : m_type(value_t::number), m_value(v) {}

    json_t(string_t&& v) : m_type(value_t::string), m_value(std::move(v)) {}

    json_t(array_t&& v) : m_type(value_t::array), m_value(std::move(v)) {}

    json_t(object_t&& v) : m_type(value_t::object), m_value(std::move(v)) {}

    json_t(json_t&& other) noexcept
        : m_type(std::move(other.m_type)), m_value(std::move(other.m_value)) {}

    virtual ~json_t() { m_value.destroy(m_type); }

    json_t& operator=(json_t&& other) {
        m_type = std::move(other.m_type);
        m_value = std::move(other.m_value);
        return *this;
    }

public:
    constexpr value_t type() const noexcept { return m_type; }

    // is_type() ?
    constexpr bool is_null() const noexcept { return m_type == value_t::null; }

    constexpr bool is_boolean() const noexcept {
        return m_type == value_t::boolean;
    }

    constexpr bool is_number() const noexcept {
        return m_type == value_t::number;
    }

    constexpr bool is_string() const noexcept {
        return m_type == value_t::string;
    }

    constexpr bool is_array() const noexcept {
        return m_type == value_t::array;
    }

    constexpr bool is_object() const noexcept {
        return m_type == value_t::object;
    }

    // T get<T>()
    boolean_t get_boolean() {
        assert(is_boolean());

        return m_value.boolean;
    }

    number_t get_number() {
        assert(m_type == value_t::number);

        return m_value.number;
    }

    string_t* get_string() {
        assert(m_type == value_t::string);

        return m_value.string;
    }

    array_t* get_array() {
        assert(m_type == value_t::array);

        return m_value.array;
    }

    object_t* get_object() {
        assert(m_type == value_t::object);

        return m_value.object;
    }
};
} // namespace detail
} // namespace microlife