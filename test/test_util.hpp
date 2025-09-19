#pragma once
#include <string>
#include <ranges>
#include <string_view>

template<std::ranges::range R>
std::string join(const R& range, std::string_view sep) noexcept {
    std::string res;
    auto res_end = back_inserter(res);
    auto it = range.begin();
    auto end = range.end();
    if (it != end) {
        format_to(res_end, "{}", *it);
        ++it;
    }
    for(; it != end; ++it) {
        format_to(res_end, "{}{}", sep, *it);
    }
    return res;
}

