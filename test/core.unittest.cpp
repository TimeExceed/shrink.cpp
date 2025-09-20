#include "shrink/core.hpp"
#include "test_util.hpp"
#include "testa.hpp"
#include <ranges>
#include <iterator>
#include <algorithm>

using namespace std;

namespace {
void shrink_unshrink(const string&) {
    uint8_t x = 10;
    auto trial = shrink::shrink(shrink::unshrink(x));
    vector<uint8_t> trial_res;
    ranges::copy(
        trial | views::transform([](auto x) { return x.v; }),
        std::back_inserter(trial_res));
    vector<uint8_t> oracle = {};
    TESTA_ASSERT(trial_res == oracle)
        .hint("trial: {}", join(trial_res, ", "sv))
        .hint("oracle: {}", join(oracle, ", "sv))
        .issue();
}
}
TESTA_DEF_JUNIT_LIKE1(shrink_unshrink);
