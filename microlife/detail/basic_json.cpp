#include "microlife/detail/basic_json.h"
#include "microlife/detail/parser.hpp"

using namespace microlife;
using namespace microlife::detail;

bool basic_json::parse(const string_t& str) {
    static parser p;
    return p.parse(str, *this);
}
