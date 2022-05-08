#include "microlife/json.hpp"

#include <iostream>

using json = microlife::json;

int main() {
    // 解析json
    {
        json j;
        j.parse("{\"a\":1,\"b\":2}");

        std::cout << (j.is_object() ? "is object" : "not object") << std::endl;

        auto a = j.get<json::object_t&>().find("a");
        auto b = j.get<json::object_t&>().find("b");

        std::cout << a->first << ": " << a->second << std::endl;
        std::cout << b->first << ": " << b->second << std::endl;
    }
    std::cout << std::endl;
    // 序列化 json 数据
    {
        json::object_t object;
        object["a"] = 1;
        object["b"] = 2;

        json j = object;
        std::cout << j.dump() << std::endl;
    }
    std::cout << std::endl;
    // json 数据的操作
    {
        json json_null = nullptr;
        std::cout << json_null.type() << ": ";

        json json_bool = true;
        std::cout << json_bool.type() << ": ";
        std::cout << json_bool.get<bool>() << std::endl;

        json json_num = 3.14;
        std::cout << json_num.type() << ": ";
        std::cout << json_num.get<double>() << std::endl;

        json json_str = "hello json";
        std::cout << json_str.type() << ": ";
        std::cout << json_str.get<std::string>() << std::endl;

        json json_array = json::array_t({1, 2, 3});
        std::cout << json_array.type() << ": ";
        std::cout << json_array.get<json::array_t&>()[0] << std::endl;

        json json_object = json::object_t({{"a", 1}, {"b", 2}});
        std::cout << json_object.type() << ": ";
        std::cout << json_object.get<json::object_t&>().at("a") << std::endl;

        std::cout << std::endl;
        json json_cnt(3, true);
        std::cout << json_cnt.type() << ": ";
        std::cout << json_cnt.get<json::array_t&>().size() << std::endl;
    }
}