#pragma once

#include <concepts>
#include <iterator>
#include <utility>

namespace c2k {
    template<typename T, typename... Args>
    concept IsOneOf = (std::same_as<T, Args> || ...);

    template<typename F, typename Result, typename... Args>
    concept Invocable = requires(F&& f, Result&& result, Args&&... args) {
        { f(std::forward<Args>(args)...) } -> std::convertible_to<Result>;
    };

    template<typename T, typename Element>
    concept Iterable = requires(T&& iterable) {
        { *std::begin(iterable) } -> std::convertible_to<const Element&>;
        { *std::begin(iterable) } -> std::convertible_to<const Element&>;
        { *std::end(iterable) } -> std::convertible_to<const Element&>;
        { *(std::end(iterable) - 1) } -> std::convertible_to<const Element&>;
    };
} // namespace c2k
