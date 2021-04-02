#pragma once

#include <cassert>

#include <algorithm>
#include <sstream>
#include <string>
#include <string_view>


#define ToLambda(F) [](auto&&... args)                              \
    noexcept(noexcept(F(std::forward<decltype(args)>(args)...)))    \
    -> decltype(F(std::forward<decltype(args)>(args)...)) {         \
        return F(std::forward<decltype(args)>(args)...);            \
    }


namespace psqlxx {

[[nodiscard]]
static inline bool StartsWith(const std::string &str, const char *prefix) {
    assert(prefix);

    return str.rfind(prefix, 0) == 0;
}


[[nodiscard]]
static inline constexpr std::string_view
Trim(const std::string_view original) {
    const auto left = std::find_if_not(original.cbegin(), original.cend(),
                                       ToLambda(std::isspace));
    if (left == original.cend())
        return {};

    const auto right = std::find_if_not(original.crbegin(), original.crend(),
                                        ToLambda(std::isspace)).base();

    return std::string_view(left, std::distance(left, right) + 1);
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
