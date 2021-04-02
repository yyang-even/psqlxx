#pragma once

#include <sstream>
#include <string>
#include <string_view>


namespace psqlxx {

[[nodiscard]]
static inline auto StartsWith(const std::string &str, const std::string_view prefix) {
    return str.rfind(prefix, 0) == 0;
}


class Joiner {
    char m_delimiter{};

    template <typename T>
    void outWithDelimiter(std::stringstream &out, T &&arg) const {
        out << m_delimiter << arg;
    }

public:
    explicit constexpr Joiner(const char d = ',') : m_delimiter(d) {}

    template <typename First, typename... Args>
    [[nodiscard]]
    const std::string operator()(First &&first, Args &&... args) const {
        std::stringstream out;

        out << first;
        (..., outWithDelimiter(out, args));

        return out.str();
    }
};

static inline constexpr Joiner SpaceJoiner{' '};

}//namespace psqlxx
