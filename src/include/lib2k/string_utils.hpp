#pragma once

#include <algorithm>
#include <cctype>
#include <concepts>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
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
        [[maybe_unused]] std::string_view const separator,
        std::convertible_to<std::string> auto&& first,
        std::convertible_to<std::string> auto&&... rest
    ) { // clang-format on
        auto result = std::string{ std::forward<decltype(first)>(first) };
        (
                [&] {
                    result += separator;
                    result += std::string{ std::forward<decltype(rest)>(rest) };
                }(),
                ...
        );
        return result;
    }

    [[nodiscard]] inline std::string repeated(std::string_view const to_repeat, std::size_t const count) {
        auto result = std::string{};
        result.reserve(count * to_repeat.length());
        for (auto i = decltype(count){ 0 }; i < count; ++i) {
            result += to_repeat;
        }
        return result;
    }

    enum class StartPosition : std::string::size_type {};
    enum class MaxReplacementCount : std::string::size_type {};

    // clang-format off
    [[nodiscard]] std::string replace(
        std::string original,
        std::string_view to_replace,
        std::string_view replacement,
        StartPosition start_pos,
        MaxReplacementCount max_num_replacements
    ); // clang-format on

    // clang-format off
    [[nodiscard]] inline std::string replace(
        std::string original,
        std::string_view const to_replace,
        std::string_view const replacement
    ) { // clang-format on
        return replace(
                std::move(original),
                to_replace,
                replacement,
                StartPosition{ 0 },
                MaxReplacementCount{ std::numeric_limits<std::underlying_type_t<MaxReplacementCount>>::max() }
        );
    }

    // clang-format off
    [[nodiscard]] inline std::string replace(
        std::string original,
        std::string_view const to_replace,
        std::string_view const replacement,
        StartPosition const start_pos
    ) { // clang-format on
        return replace(
                std::move(original),
                to_replace,
                replacement,
                start_pos,
                MaxReplacementCount{ std::numeric_limits<std::underlying_type_t<MaxReplacementCount>>::max() }
        );
    }

    // clang-format off
    [[nodiscard]] inline std::string replace(
        std::string original,
        std::string_view const to_replace,
        std::string_view const replacement,
        MaxReplacementCount const max_num_replacements
    ) { // clang-format on
        return replace(std::move(original), to_replace, replacement, StartPosition{ 0 }, max_num_replacements);
    }

    inline void left_pad(std::string& string, std::size_t const target_length, char const padding_char = ' ') {
        if (target_length <= string.length()) {
            return;
        }
        string.insert(0, target_length - string.length(), padding_char);
    }

    inline void right_pad(std::string& string, std::size_t const target_length, char const padding_char = ' ') {
        if (target_length <= string.length()) {
            return;
        }
        string.insert(string.length(), target_length - string.length(), padding_char);
    }

    // clang-format off
    [[nodiscard]] inline std::string left_padded(
        std::string&& string,
        std::size_t const target_length,
        char const padding_char = ' '
    ) { // clang-format on
        left_pad(string, target_length, padding_char);
        return string;
    }

    // clang-format off
    [[nodiscard]] inline std::string right_padded(
        std::string&& string,
        std::size_t const target_length,
        char const padding_char = ' '
    ) { // clang-format on
        right_pad(string, target_length, padding_char);
        return string;
    }
} // namespace c2k
