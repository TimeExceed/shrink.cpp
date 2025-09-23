#pragma once
#include <ranges>
#include <type_traits>
#include <cstdlib>

namespace shrink {

template<class T>
struct Shrinker {
    static_assert(false, "Shrinker does not implement for type T.");
};

template<class T, class U>
concept _shrinker_range = std::ranges::forward_range<T>
    && std::is_same_v<std::remove_cvref_t<std::ranges::range_value_t<T>>, U>;

template<class T>
concept Shrinkable =
    std::is_copy_constructible_v<T> && std::is_move_constructible_v<T>
    && requires (T v) {
        { Shrinker<T>(std::move(v)) } noexcept;
    }
    && requires(Shrinker<T> v) {
        { std::move(v).shrink() } noexcept -> _shrinker_range<T>;
    };

template<Shrinkable T>
auto _shrink(T t) noexcept {
    Shrinker<T> x(std::move(t));
    return std::move(x).shrink();
}

template<class T>
auto shrink(T t) noexcept {
    return _shrink<std::remove_cvref_t<T>>(std::move(t));
}

template<class T>
struct Unshrink {
    T v;
};

template<class T>
struct _EmptyShrinkerImpl {
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using iterator = value_type const*;
    using const_iterator = iterator;

    iterator begin() const noexcept {
        return nullptr;
    }

    iterator end() const noexcept {
        return nullptr;
    }

    iterator cbegin() const noexcept {
        return begin();
    }

    iterator cend() const noexcept {
        return end();
    }
};


template<class T>
requires std::is_move_constructible_v<T>
struct Shrinker<Unshrink<T>> {
    explicit Shrinker(Unshrink<T> v) noexcept
    {}

    _EmptyShrinkerImpl<Unshrink<T>> shrink() && noexcept {
        return _EmptyShrinkerImpl<Unshrink<T>>();
    }
};

template<class T>
requires std::is_move_constructible_v<T>
auto unshrink(T v) noexcept {
    return Unshrink<T> {
        .v = std::move(v),
    };
}

}

