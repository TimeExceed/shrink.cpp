#include "shrink/core.hpp"
#include "shrink/span.hpp"
#include "test_util.hpp"
#include "testa.hpp"
#include <string>
#include <ranges>
#include <iterator>
#include <algorithm>
#include <format>

using namespace std;

namespace {
string format_span(span<uint8_t> xs) {
    return format("[{},{}]", xs.front(), xs.back());
}

void shrink_span(const string&) {
    vector<uint8_t> xs = {0, 1, 2, 3, 4, 5, 6, 7};
    auto trial = shrink::shrink(span<uint8_t>(xs));
    string trial_str = [&]() {
        vector<string> res;
        ranges::copy(
            trial | views::transform([](auto x) { return format_span(x); }),
            back_inserter(res));
        return join(res, ", "sv);
    }();
    auto oracle_str = "[0,3], [1,4], [2,5], [3,6], [4,7], "
        "[0,5], [1,6], [2,7], [0,6], [1,7]"sv;
    TESTA_ASSERT(trial_str == oracle_str)
        .hint("trial: {}", trial_str)
        .hint("oracle: {}", oracle_str)
        .issue();
}
}
TESTA_DEF_JUNIT_LIKE1(shrink_span);
