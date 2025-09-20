#include "shrink/core.hpp"
#include "shrink/int.hpp"
#include "shrink/vec.hpp"
#include "test_util.hpp"
#include "testa.hpp"
#include <string>
#include <ranges>
#include <iterator>
#include <algorithm>
#include <format>

using namespace std;

namespace {
void shrink_vec(const string&) {
    vector<uint8_t> xs = {1, 1};
    auto trial = shrink::shrink(xs);
    string trial_str = [&]() {
        vector<string> res;
        ranges::copy(
            trial
            | views::transform([](auto const& xs) {
                return format("[{}]", join(xs, ", "sv));
            }),
            back_inserter(res));
        return join(res, ", "sv);
    }();
    auto oracle_str = "[1], [1], [0, 1], [1, 0]"sv;
    TESTA_ASSERT(trial_str == oracle_str)
        .hint("trial: {}", trial_str)
        .hint("oracle: {}", oracle_str)
        .issue();
}
}
TESTA_DEF_JUNIT_LIKE1(shrink_vec);

namespace {
void shrink_elem_shrinker_empty(const string&) {
    vector<uint8_t> xs = {0, 0, 0};
    shrink::_impl_vec::ElemShrinker<uint8_t> trial(std::move(xs));
    string trial_str = [&]() {
        vector<string> res;
        ranges::copy(
            trial | views::transform([](auto const& xs) {
                return format("[{}]", join(xs, ", "sv));
            }),
            back_inserter(res));
        return join(res, ", "sv);
    }();
    auto oracle_str = ""sv;
    TESTA_ASSERT(trial_str == oracle_str)
        .hint("trial: {}", trial_str)
        .hint("oracle: {}", oracle_str)
        .issue();
}
}
TESTA_DEF_JUNIT_LIKE1(shrink_elem_shrinker_empty);

namespace {
void shrink_elem_shrinker_skip(const string&) {
    vector<uint8_t> xs = {1, 0, 1};
    shrink::_impl_vec::ElemShrinker<uint8_t> trial(std::move(xs));
    string trial_str = [&]() {
        vector<string> res;
        ranges::copy(
            trial | views::transform([](auto const& xs) {
                return format("[{}]", join(xs, ", "sv));
            }),
            back_inserter(res));
        return join(res, ", "sv);
    }();
    auto oracle_str = "[0, 0, 1], [1, 0, 0]"sv;
    TESTA_ASSERT(trial_str == oracle_str)
        .hint("trial: {}", trial_str)
        .hint("oracle: {}", oracle_str)
        .issue();
}
}
TESTA_DEF_JUNIT_LIKE1(shrink_elem_shrinker_skip);

namespace {
void shrink_len_shrinker(const string&) {
    vector<uint8_t> xs = {0, 1, 2, 3};
    shrink::_impl_vec::LenShrinker<uint8_t> trial(std::move(xs));
    string trial_str = [&]() {
        vector<string> res;
        ranges::copy(
            trial | views::transform([](auto const& xs) {
                return format("[{}]", join(xs, ", "sv));
            }),
            back_inserter(res));
        return join(res, " "sv);
    }();
    auto oracle_str = "[2, 3] [0, 1] "
        "[1, 2, 3] [0, 2, 3] [0, 1, 3] [0, 1, 2]"sv;
    TESTA_ASSERT(trial_str == oracle_str)
        .hint("trial: {}", trial_str)
        .hint("oracle: {}", oracle_str)
        .issue();
}
}
TESTA_DEF_JUNIT_LIKE1(shrink_len_shrinker);

namespace {
struct A {
    int v = 0;
};

struct AShrinker {
    struct Iter {
        using value_type = A;
        using difference_type = std::ptrdiff_t;

        Iter(const Iter&) noexcept = default;
        Iter& operator=(const Iter&) noexcept = default;
        Iter(Iter&&) noexcept = default;
        Iter& operator=(Iter&&) noexcept = default;

        explicit Iter() noexcept
        {}

        explicit Iter(const A& a, size_t cnt_down) noexcept
        :   _a(&a),
            _cnt_down(cnt_down)
        {}

        value_type operator*() const noexcept {
            A r(*_a);
            return r;
        }

        Iter& operator++() noexcept {
            --_cnt_down;
            return *this;
        }

        Iter operator++(int) noexcept {
            Iter copied(*this);
            ++(*this);
            return copied;
        }

        bool operator==(const Iter& ano) const noexcept {
            return _a == ano._a && _cnt_down == ano._cnt_down;
        }

        bool operator!=(const Iter&) const noexcept = default;

    private:
        const A* _a = nullptr;
        size_t _cnt_down = 0;
    };

    struct Iter1 {
        using value_type = A;
        using difference_type = std::ptrdiff_t;

        A operator*() const noexcept {
            abort();
        }

        Iter1& operator++() noexcept {
            abort();
        }

        Iter1 operator++(int) noexcept {
            abort();
        }

        bool operator==(const Iter1& ano) const noexcept {
            return true;
        }

        bool operator!=(const Iter1&) const noexcept = default;
    };

    using iterator = Iter;
    using const_iterator = Iter;
    using value_type = Iter::value_type;
    using difference_type = std::ptrdiff_t;

    explicit AShrinker(A a) noexcept
    :   _a(a)
    {}

    iterator begin() const noexcept {
        return Iter(_a, 2);
        // return Iter1 {};
    }

    iterator end() const noexcept {
        return Iter(_a, 0);
        // return Iter1 {};
    }

private:
    A _a;
};
}

template<>
struct shrink::Shrinker<A> {
    explicit Shrinker(A a) noexcept
    :   _v(a)
    {}

    AShrinker shrink() && noexcept {
        AShrinker x(_v);
        return x;
    }

private:
    A _v;
};

namespace {
void shrink_dangling_elem_shrinker(const string&) {
    vector<A> xs = {
        A {.v = 0},
        A {.v = 1},
    };
    shrink::_impl_vec::ElemShrinker<A> trial(xs);
    string trial_str = [&]() {
        vector<string> res;
        ranges::copy(
            trial
            | views::transform([](auto const& xs) {
                vector<string> as;
                ranges::copy(
                    xs | views::transform([](auto x) {
                        return format("{}", x.v);
                    }),
                    back_inserter(as));
                return format("[{}]", join(as, ","sv));
            }),
            back_inserter(res));
        return join(res, " "sv);
    }();
}
}
TESTA_DEF_JUNIT_LIKE1(shrink_dangling_elem_shrinker);
