#pragma once
#include "core.hpp"
#include "fassert.hpp"
#include <optional>
#include <type_traits>

namespace shrink {

namespace _impl_int {

template<class T>
requires std::is_integral_v<T>
struct Shrinker {
    struct Iter {
        using value_type = T;
        using difference_type = std::ptrdiff_t;

        Iter(const Iter&) noexcept = default;
        Iter& operator=(const Iter&) noexcept = default;
        Iter(Iter&&) noexcept = default;
        Iter& operator=(Iter&&) noexcept = default;

        Iter() noexcept
        :   _state(std::nullopt)
        {}

        explicit Iter(T v) noexcept
        :   _state(_State(v))
        {
            if (_state->is_exhausted()) {
                _state.reset();
            }
        }

        T operator*() const noexcept {
            FASSERT(_state);
            return **_state;
        }

        Iter& operator++() noexcept {
            FASSERT(_state);
            bool has_next = _state->next();
            if (!has_next) {
                _state.reset();
            }
            return *this;
        }

        Iter operator++(int) noexcept {
            Iter copied(*this);
            ++(*this);
            return copied;
        }

        bool operator==(const Iter& ano) const noexcept {
            return _state == ano._state;
        }

        bool operator!=(const Iter&) const noexcept = default;

    private:
        struct _State {
            T init;
            T complement;

            explicit _State(T v) noexcept
            :   init(v),
                complement(v)
            {}

            T operator*() const noexcept {
                return init - complement;
            }

            bool next() noexcept {
                complement /= 2;
                return !is_exhausted();
            }

            bool is_exhausted() const noexcept {
                return complement == 0;
            }

            bool operator==(const _State& ano) const noexcept {
                return init == ano.init && complement == ano.complement;
            }

            bool operator!=(const _State& ano) const noexcept = default;
        };

        std::optional<_State> _state;
    };

    using iterator = Iter;
    using const_iterator = Iter;
    using value_type = Iter::value_type;
    using difference_type = Iter::difference_type;

    Iter begin() const noexcept {
        return Iter(_v);
    }

    Iter end() const noexcept {
        return Iter();
    }

    Iter cbegin() const noexcept {
        return begin();
    }

    Iter cend() const noexcept {
        return end();
    }

    explicit Shrinker(T v) noexcept
    :   _v(v)
    {}

private:
    T _v;
};

}

template<class T>
requires std::is_integral_v<T>
struct Shrinker<T> {
    explicit Shrinker(T v) noexcept
    :   _v(v)
    {}

    _impl_int::Shrinker<T> shrink() && noexcept {
        _impl_int::Shrinker<T> x(_v);
        return x;
    }

private:
    T _v;
};

}
