#pragma once

#include <algorithm>
#include <cctype>
#include <concepts>
#include <stdexcept>
#include <string>
#include <vector>

namespace c2k {
    template<typename T, typename Result, typename... Args>
    concept Predicate = requires(T predicate) {
        { predicate(std::declval<Args>()...) } -> std::same_as<Result>;
    };

    template<typename T>
    concept StringIterable = std::convertible_to<decltype(*std::cbegin(std::declval<T>())), std::string>;

    void left_trim(std::string& s, Predicate<bool, char> auto&& predicate) {
        auto const it = std::find_if(std::cbegin(s), std::cend(s), [&](auto const c) { return not predicate(c); });
        if (it == std::cend(s)) {
            s.clear();
            return;
        }
        s.erase(std::cbegin(s), it);
    }

    inline void left_trim(std::string& s) {
        left_trim(s, [](char const c) { return static_cast<bool>(std::isspace(static_cast<unsigned char>(c))); });
    }

    inline void left_trim(std::string& s, std::string_view const characters_to_erase) {
        left_trim(s, [characters_to_erase](char const c) {
            return characters_to_erase.find(c) != std::string_view::npos;
        });
    }

    [[nodiscard]] std::string left_trimmed(std::string&& s, Predicate<bool, char> auto&& predicate) {
        left_trim(s, std::forward<decltype(predicate)>(predicate));
        return s;
    }

    [[nodiscard]] inline std::string left_trimmed(std::string&& s) {
        return left_trimmed(std::move(s), [](char const c) {
            return static_cast<bool>(std::isspace(static_cast<unsigned char>(c)));
        });
    }

    [[nodiscard]] inline std::string left_trimmed(std::string&& s, std::string_view const characters_to_erase) {
        return left_trimmed(std::move(s), [characters_to_erase](char const c) {
            return characters_to_erase.find(c) != std::string_view::npos;
        });
    }

    void right_trim(std::string& s, Predicate<bool, char> auto&& predicate) {
        auto const rit = std::find_if(s.crbegin(), s.crend(), [&](auto const c) { return not predicate(c); });
        if (rit == s.crend()) {
            s.clear();
            return;
        }
        s.erase(rit.base(), s.cend());
    }

    inline void right_trim(std::string& s) {
        right_trim(s, [](char const c) { return static_cast<bool>(std::isspace(static_cast<unsigned char>(c))); });
    }

    inline void right_trim(std::string& s, std::string_view const characters_to_erase) {
        right_trim(s, [characters_to_erase](char const c) {
            return characters_to_erase.find(c) != std::string_view::npos;
        });
    }

    [[nodiscard]] std::string right_trimmed(std::string&& s, Predicate<bool, char> auto&& predicate) {
        right_trim(s, std::forward<decltype(predicate)>(predicate));
        return s;
    }

    [[nodiscard]] inline std::string right_trimmed(std::string&& s) {
        return right_trimmed(std::move(s), [](char const c) {
            return static_cast<bool>(std::isspace(static_cast<unsigned char>(c)));
        });
    }

    [[nodiscard]] inline std::string right_trimmed(std::string&& s, std::string_view const characters_to_erase) {
        return right_trimmed(std::move(s), [characters_to_erase](char const c) {
            return characters_to_erase.find(c) != std::string_view::npos;
        });
    }

    void trim(std::string& s, Predicate<bool, char> auto&& predicate) {
        left_trim(s, std::forward<decltype(predicate)>(predicate));
        right_trim(s, std::forward<decltype(predicate)>(predicate));
    }

    inline void trim(std::string& s) {
        trim(s, [](char const c) { return static_cast<bool>(std::isspace(static_cast<unsigned char>(c))); });
    }

    inline void trim(std::string& s, std::string_view const characters_to_erase) {
        trim(s, [characters_to_erase](char const c) { return characters_to_erase.find(c) != std::string_view::npos; });
    }

    [[nodiscard]] std::string trimmed(std::string&& s, Predicate<bool, char> auto&& predicate) {
        trim(s, std::forward<decltype(predicate)>(predicate));
        return s;
    }

    [[nodiscard]] inline std::string trimmed(std::string&& s) {
        return trimmed(std::move(s), [](char const c) {
            return static_cast<bool>(std::isspace(static_cast<unsigned char>(c)));
        });
    }

    [[nodiscard]] inline std::string trimmed(std::string&& s, std::string_view const characters_to_erase) {
        return trimmed(std::move(s), [characters_to_erase](char const c) {
            return characters_to_erase.find(c) != std::string_view::npos;
        });
    }

    [[nodiscard]] std::vector<std::string> split(std::string const& s, std::string_view delimiter);

    [[nodiscard]] std::string join(StringIterable auto&& iterable, std::string_view const separator) {
        auto result = std::string{};
        for (auto it = std::cbegin(iterable); it != std::cend(iterable); ++it) {
            if constexpr (std::same_as<std::decay_t<decltype(*std::cbegin(iterable))>, char const*>) {
                if (*it == nullptr) {
                    throw std::invalid_argument{ "cannot join nullptr c-strings" };
                }
            }
            if (it != std::cbegin(iterable)) {
                result += separator;
            }
            result += *it;
        }
        return result;
    }

    // clang-format off
    [[nodiscard]] std::string join(
        std::string_view const separator,
        std::convertible_to<std::string> auto&& first,
        std::convertible_to<std::string> auto&&... rest
    ) { // clang-format on
        auto result = std::string{ std::forward<decltype(first)>(first) };
        ([&]() {
            result += separator;
            result += std::string{ std::forward<decltype(rest)>(rest) };
        }(), ...);
        return result;
    }
} // namespace c2k
