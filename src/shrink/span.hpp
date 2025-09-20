#pragma once

#include "core.hpp"
#include "fassert.hpp"
#include <span>

namespace shrink {

namespace _impl_span {

template<class T, size_t Extent>
struct ContiguousShrinker {
    struct Iter {
        using value_type = std::span<T, Extent>;
        using difference_type = std::ptrdiff_t;

        Iter() noexcept
        :   _state(std::nullopt)
        {}

        explicit Iter(std::span<T, Extent> xs) noexcept
        :   _state(_State(xs))
        {
            ++(*this);
        }

        Iter(const Iter&) noexcept = default;
        Iter& operator=(const Iter&) noexcept = default;
        Iter(Iter&&) noexcept = default;
        Iter& operator=(Iter&&) noexcept = default;

        value_type operator*() const noexcept {
            FASSERT(_state);
            return _state->span();
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
            std::span<T, Extent> elems;
            size_t offset = 0;
            size_t count = 0;
            size_t count_complement = 0;

            explicit _State(std::span<T, Extent> xs) noexcept
            :   elems(std::move(xs))
            {
                count_complement = elems.size();
                count = elems.size() - count_complement;
                offset = elems.size();
            }

            bool operator==(const _State& ano) const noexcept {
                if (elems.data() != ano.elems.data()) {
                    return false;
                }
                if (elems.size() != ano.elems.size()) {
                    return false;
                }
                if (offset != ano.offset) {
                    return false;
                }
                if (count != ano.count) {
                    return false;
                }
                return true;
            }
            bool operator!=(const _State&) const noexcept = default;

            std::span<T> span() const noexcept {
                return elems.subspan(offset, count);
            }

            bool next() noexcept {
                ++offset;
                if (offset + count <= elems.size()) {
                    return true;
                }
                offset = 0;
                count_complement /= 2;
                count = elems.size() - count_complement;
                return count_complement > 0;
            }
        };

        std::optional<_State> _state;
    };

    using iterator = Iter;
    using const_iterator = Iter;
    using value_type = Iter::value_type;
    using difference_type = Iter::difference_type;

    Iter begin() const noexcept {
        return Iter(_xs);
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

    explicit ContiguousShrinker(std::span<T, Extent> xs) noexcept
    :   _xs(xs)
    {}

private:
    std::span<T> _xs;
};

}

template<class T, size_t Extent>
struct Shrinker<std::span<T, Extent>> {
    explicit Shrinker(std::span<T, Extent> xs) noexcept
    :   _xs(xs)
    {}

    _impl_span::ContiguousShrinker<T, Extent> shrink() && noexcept {
        return _impl_span::ContiguousShrinker<T, Extent>(std::move(_xs));
    }

private:
    std::span<T, Extent> _xs;
};

}

