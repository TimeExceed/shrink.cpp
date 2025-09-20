#pragma once
#include "core.hpp"
#include "fassert.hpp"
#include <memory>
#include <vector>
#include <type_traits>

namespace shrink {

namespace _impl_vec {

template<class T>
requires std::is_move_constructible_v<T> && std::is_copy_constructible_v<T>
struct LenShrinker {
    struct Iter {
        using value_type = std::vector<T>;
        using difference_type = std::ptrdiff_t;

        Iter(const Iter&) noexcept = default;
        Iter& operator=(const Iter&) noexcept = default;
        Iter(Iter&&) noexcept = default;
        Iter& operator=(Iter&&) noexcept = default;

        Iter() noexcept
        :   _state(std::nullopt)
        {}

        explicit Iter(const std::vector<T>& xs) noexcept
        :   _state(_State(xs))
        {
            ++(*this);
        }

        value_type operator*() const noexcept {
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
            explicit _State(const std::vector<T>& vs) noexcept
            :   _elems(&vs),
                _hole_len(vs.size())
            {}

            bool next() noexcept {
                _hole_offset += _hole_len;
                if (_hole_offset >= _elems->size()) {
                    _hole_len /= 2;
                    _hole_offset = 0;
                }
                return _hole_len > 0;
            }

            std::vector<T> operator*() const noexcept {
                std::vector<T> res;
                res.insert(
                    res.end(),
                    _elems->begin(),
                    _elems->begin() + _hole_offset);
                if (_hole_offset + _hole_len < _elems->size()) {
                    res.insert(
                        res.end(),
                        _elems->begin() + _hole_offset + _hole_len,
                        _elems->end());
                }
                return res;
            }

            bool operator==(const _State& ano) const noexcept {
                if (_elems != ano._elems) {
                    return false;
                }
                if (_hole_len != ano._hole_len) {
                    return false;
                }
                if (_hole_offset != ano._hole_offset) {
                    return false;
                }
                return true;
            }

            bool operator!=(const _State&) const noexcept = default;

        private:
            const std::vector<T>* _elems = nullptr;
            size_t _hole_len = 0;
            size_t _hole_offset = 0;
        };

        std::optional<_State> _state;
    };

    using iterator = Iter;
    using const_iterator = Iter;
    using value_type = Iter::value_type;
    using difference_type = Iter::difference_type;

    Iter begin() const noexcept {
        return Iter(*_elems);
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

    explicit LenShrinker(const std::vector<T>& vs) noexcept
    :   _elems(&vs)
    {}

private:
    // * This shrinker must own nothing so its iterators can work well without
    //   the shrinker.
    // * uses a pointer rather than a span in order to be compatible with
    //   vector<bool>.
    const std::vector<T>* _elems = nullptr;
};

template<class T>
requires
    std::is_move_constructible_v<T>
    && std::is_copy_constructible_v<T>
    && Shrinkable<T>
struct ElemShrinker {
    struct Iter {
        using value_type = std::vector<T>;
        using difference_type = std::ptrdiff_t;

        Iter(const Iter&) noexcept = default;
        Iter& operator=(const Iter&) noexcept = default;
        Iter(Iter&&) noexcept = default;
        Iter& operator=(Iter&&) noexcept = default;

        Iter() noexcept
        :   _state(std::nullopt)
        {}

        explicit Iter(const std::vector<T>& xs) noexcept
        :   _state(_State(xs))
        {
            ++(*this);
        }

        value_type operator*() const noexcept {
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
            explicit _State(const std::vector<T>& xs) noexcept
            :   _elems(&xs)
            {}

            std::vector<T> operator*() const noexcept {
                FASSERT(!_elem_iters.empty());
                std::vector<T> res;
                res.reserve(_elems->size());
                if (_index > 0) {
                    res.insert(res.end(), _elems->begin(), _elems->begin() + _index);
                }
                res.push_back(*std::get<0>(_elem_iters));
                res.insert(res.end(), _elems->begin() + _index + 1, _elems->end());
                return res;
            }

            bool next() noexcept {
                if (!_elem_iters.empty()) {
                    _elem_iters = std::move(_elem_iters).next();
                }
                if (_elem_iters.empty()) {
                    ++_index;
                    for(int64_t n = _elems->size(); _index < n; ++_index) {
                        _shrinker.reset(new ElemShrinkerType(shrink((*_elems)[_index])));
                        _elem_iters = std::move(
                            std::ranges::subrange(_shrinker->begin(), _shrinker->end()));
                        if (!_elem_iters.empty()) {
                            return true;
                        }
                    }
                    return false;
                } else {
                    return true;
                }
            }

            bool operator==(const _State& ano) const noexcept {
                if (_elems != ano._elems) {
                    return false;
                }
                if (_index != ano._index) {
                    return false;
                }
                auto const& [it, end] = _elem_iters;
                auto const& [ano_it, ano_end] = ano._elem_iters;
                if (it != ano_it) {
                    return false;
                }
                if (end != ano_end) {
                    return false;
                }
                return true;
            }

            bool operator!=(const _State&) const noexcept = default;

        private:
            using ElemShrinkerType = std::invoke_result_t<decltype(shrink<T>), T>;
            using ElemShrinkerIter = typename ElemShrinkerType::const_iterator;

            const std::vector<T>* _elems = nullptr;
            int64_t _index = -1;
            // * should own an element shrinker because its iterators may share
            //   something in the shrinker.
            // * `_shrinker` must be a shared ptr in order to make the iterators
            //   copyable.
            std::shared_ptr<ElemShrinkerType> _shrinker;
            std::ranges::subrange<ElemShrinkerIter> _elem_iters;
        };

        std::optional<_State> _state;
    };

    using iterator = Iter;
    using const_iterator = Iter;
    using value_type = Iter::value_type;
    using difference_type = Iter::difference_type;

    Iter begin() const noexcept {
        return Iter(*_elems);
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

    explicit ElemShrinker(const std::vector<T>& vs) noexcept
    :   _elems(&vs)
    {}

private:
    // * This shrinker must own nothing so its iterators can work well without
    //   the shrinker.
    // * uses a pointer rather than a span in order to be compatible with
    //   vector<bool>.
    const std::vector<T>* _elems = nullptr;
};

template<Shrinkable T>
struct ChainShrinker {
    explicit ChainShrinker(std::vector<T> vs) noexcept
    :   _elems(std::move(vs))
    {}

    struct Iter {
        using value_type = std::vector<T>;
        using difference_type = std::ptrdiff_t;

        Iter(const Iter&) noexcept = default;
        Iter& operator=(const Iter&) noexcept = default;
        Iter(Iter&&) noexcept = default;
        Iter& operator=(Iter&&) noexcept = default;

        Iter() noexcept
        {}

        explicit Iter(const std::vector<T>& xs) noexcept
        {
            LenShrinker<T> len_sh(xs);
            _len_shrinker = std::move(
                std::ranges::subrange<LenIter>(len_sh.begin(), len_sh.end()));
            ElemShrinker<T> elem_sh(xs);
            _elem_shrinker = std::move(
                std::ranges::subrange<ElemIter>(elem_sh.begin(), elem_sh.end()));
        }

        value_type operator*() const noexcept {
            if (!_len_shrinker.empty()) {
                return _len_shrinker.front();
            }
            FASSERT(!_elem_shrinker.empty());
            return _elem_shrinker.front();
        }

        Iter& operator++() noexcept {
            if (!_len_shrinker.empty()) {
                _len_shrinker = std::move(_len_shrinker).next();
                return *this;
            }
            FASSERT(!_elem_shrinker.empty());
            _elem_shrinker = std::move(_elem_shrinker).next();
            return *this;
        }

        Iter operator++(int) noexcept {
            Iter copied(*this);
            ++(*this);
            return copied;
        }

        bool operator==(const Iter& ano) const noexcept {
            if (_len_shrinker.begin() != ano._len_shrinker.begin()) {
                return false;
            }
            if (_elem_shrinker.begin() != ano._elem_shrinker.begin()) {
                return false;
            }
            return true;
        }

        bool operator!=(const Iter& ano) const noexcept = default;

    private:
        using LenIter = typename LenShrinker<T>::const_iterator;
        std::ranges::subrange<LenIter> _len_shrinker;

        using ElemIter = typename ElemShrinker<T>::const_iterator;
        std::ranges::subrange<ElemIter> _elem_shrinker;
    };

    using iterator = Iter;
    using const_iterator = Iter;
    using value_type = Iter::value_type;
    using difference_type = Iter::difference_type;

    Iter begin() const noexcept {
        return Iter(_elems);
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

private:
    const std::vector<T> _elems;
};

}

template<Shrinkable T>
struct Shrinker<std::vector<T>> {
    explicit Shrinker(std::vector<T> xs) noexcept
    :   _xs(std::move(xs))
    {}

    _impl_vec::ChainShrinker<T> shrink() && noexcept {
        return _impl_vec::ChainShrinker<T>(_xs);
    }

private:
    std::vector<T> _xs;
};

}
