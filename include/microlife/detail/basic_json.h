#pragma once

#include "value_t.h" // value_t

#include <map>    // object_t
#include <string> // string_t
#include <vector> // vector_t

namespace microlife {
namespace detail {
/***
 * @brief Basic JSON class
 * @details This class is used to store JSON data.
 * @author qingl
 * @date 2022_04_09
 */
class basic_json {
public:
    using boolean_t = bool;
    using number_t = double;
    using string_t = std::string;
    using array_t = std::vector<basic_json*>;
    using object_t = std::map<std::string, basic_json*>;

    using value_t = detail::value_t;
};
} // namespace detail
} // namespace microlife
