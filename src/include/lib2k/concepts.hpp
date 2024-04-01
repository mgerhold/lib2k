#pragma once

#include <concepts>

namespace c2k {
    template<typename T, typename... Args>
    concept IsOneOf = (std::same_as<T, Args> || ...);

    template<typename F, typename Result, typename... Args>
    concept Invocable = requires(F&& f, Result&& result, Args&&... args) {
        { f(std::forward<Args>(args)...) } -> std::convertible_to<Result>;
    };
} // namespace c2k
