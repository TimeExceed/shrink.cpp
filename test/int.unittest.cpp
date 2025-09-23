#include "shrink/core.hpp"
#include "shrink/int.hpp"
#include "test_util.hpp"
#include "testa.hpp"
#include <iterator>
#include <algorithm>
#include <cstddef>

using namespace std;

namespace {
void shrink_uint8(const string&) {
    uint8_t x = 10;
    auto trial = shrink::shrink(x);
    vector<uint8_t> trial_res;
    ranges::copy(std::move(trial), std::back_inserter(trial_res));
    vector<uint8_t> oracle = {0, 5, 8, 9};
    TESTA_ASSERT(trial_res == oracle)
        .hint("trial: {}", join(trial_res, ", "sv))
        .hint("oracle: {}", join(oracle, ", "sv))
        .issue();
}
}
TESTA_DEF_JUNIT_LIKE1(shrink_uint8);

namespace {
void shrink_uint8_zero(const string&) {
    uint8_t x = 0;
    auto trial = shrink::shrink(x);
    vector<uint8_t> trial_res;
    ranges::copy(std::move(trial), std::back_inserter(trial_res));
    vector<uint8_t> oracle = {};
    TESTA_ASSERT(trial_res == oracle)
        .hint("trial: {}", join(trial_res, ", "sv))
        .hint("oracle: {}", join(oracle, ", "sv))
        .issue();
}
}
TESTA_DEF_JUNIT_LIKE1(shrink_uint8_zero);

namespace {
void shrink_int8_pos(const string&) {
    int8_t x = 10;
    auto trial = shrink::shrink(x);
    vector<int8_t> trial_res;
    ranges::copy(std::move(trial), std::back_inserter(trial_res));
    vector<int8_t> oracle = {0, 5, 8, 9};
    TESTA_ASSERT(trial_res == oracle)
        .hint("trial: {}", join(trial_res, ", "sv))
        .hint("oracle: {}", join(oracle, ", "sv))
        .issue();
}
}
TESTA_DEF_JUNIT_LIKE1(shrink_int8_pos);

namespace {
void shrink_int8_pos_limit(const string&) {
    int8_t x = numeric_limits<int8_t>::max();
    auto trial = shrink::shrink(x);
    vector<int8_t> trial_res;
    ranges::copy(std::move(trial), std::back_inserter(trial_res));
    vector<int8_t> oracle = {0, 64, 96, 112, 120, 124, 126};
    TESTA_ASSERT(trial_res == oracle)
        .hint("trial: {}", join(trial_res, ", "sv))
        .hint("oracle: {}", join(oracle, ", "sv))
        .issue();
}
}
TESTA_DEF_JUNIT_LIKE1(shrink_int8_pos_limit);

namespace {
void shrink_int8_neg(const string&) {
    int8_t x = -10;
    auto trial = shrink::shrink(x);
    vector<int8_t> trial_res;
    ranges::copy(std::move(trial), std::back_inserter(trial_res));
    vector<int8_t> oracle = {0, -5, -8, -9};
    TESTA_ASSERT(trial_res == oracle)
        .hint("trial: {}", join(trial_res, ", "sv))
        .hint("oracle: {}", join(oracle, ", "sv))
        .issue();
}
}
TESTA_DEF_JUNIT_LIKE1(shrink_int8_neg);

namespace {
void shrink_int8_neg_limit(const string&) {
    int8_t x = numeric_limits<int8_t>::min();
    auto trial = shrink::shrink(x);
    vector<int8_t> trial_res;
    ranges::copy(std::move(trial), back_inserter(trial_res));
    vector<int8_t> oracle = {0, -64, -96, -112, -120, -124, -126, -127};
    TESTA_ASSERT(trial_res == oracle)
        .hint("trial: {}", join(trial_res, ", "sv))
        .hint("oracle: {}", join(oracle, ", "sv))
        .issue();
}
}
TESTA_DEF_JUNIT_LIKE1(shrink_int8_neg_limit);

namespace {
void shrink_byte(const string&) {
    byte x {10};
    auto trial = shrink::shrink(x);
    vector<string> trial_res;
    ranges::copy(
        std::move(trial)
        | views::transform([](byte b) {
            return format("{:02x}", to_integer<int>(b));
        }),
        back_inserter(trial_res));
    vector<string> oracle = {};
    TESTA_ASSERT(trial_res == oracle)
        .hint("trial: {}", join(trial_res, ", "sv))
        .hint("oracle: {}", join(oracle, ", "sv))
        .issue();
}
}
TESTA_DEF_JUNIT_LIKE1(shrink_byte);
