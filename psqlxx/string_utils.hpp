#pragma once

#include <cassert>

#include <string>


[[nodiscard]]
static inline bool StartsWith(const std::string &str, const char *prefix) {
    assert(prefix);

    return str.rfind(prefix, 0) == 0;
}
