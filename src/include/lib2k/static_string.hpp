#pragma once

#include <algorithm>
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>

template<std::size_t literal_size>
struct StaticString final {
    template<std::size_t other_literal_size>
    friend struct StaticString;

    std::array<char, literal_size> buffer{};

private:
    consteval explicit StaticString(std::array<char, literal_size> buffer) {
        std::copy(buffer.cbegin(), buffer.cend(), this->buffer.data());
    }

public:
    consteval StaticString(char const (&c_string)[literal_size]) { // NOLINT (implicit converting constructor)
        if (c_string[literal_size - 1] != '\0') {
            throw; // string must be null-terminated
        }
        std::copy(std::begin(c_string), std::end(c_string), buffer.begin());
    }

    [[nodiscard]] constexpr char const* c_str() const {
        return buffer.data();
    }

    [[nodiscard]] constexpr std::string str() const {
        return std::string{ c_str() };
    }

    [[nodiscard]] constexpr std::string_view view() const {
        return std::string_view{ c_str() };
    }

    [[nodiscard]] constexpr bool operator==(char const* const other) const {
        for (auto i = std::size_t{ 0 }; i < buffer.size(); ++i) {
            auto const lhs = buffer.at(i);
            auto const rhs = other[i];
            if (lhs != rhs) {
                return false;
            }
        }
        return other[buffer.size() - 1] == '\0';
    }

    [[nodiscard]] constexpr bool operator==(std::string const& other) const {
        return other == c_str();
    }

    [[nodiscard]] constexpr bool operator==(std::string_view const other) const {
        return other == c_str();
    }

    template<std::size_t other_literal_size>
    [[nodiscard]] constexpr bool operator==(StaticString<other_literal_size> const& other) const {
        return literal_size == other_literal_size and *this == other.c_str();
    }

    template<std::size_t other_literal_size>
    [[nodiscard]] consteval auto operator+(char const (&other_literal)[other_literal_size]) const {
        return *this + StaticString<other_literal_size>{ other_literal };
    }

    template<std::size_t other_literal_size>
    [[nodiscard]] consteval auto operator+(StaticString<other_literal_size> const& other) const {
        constexpr auto new_size = literal_size + other_literal_size - 1;
        auto new_buffer = std::array<char, new_size>{};
        std::copy(buffer.cbegin(), buffer.cend() - 1, new_buffer.data());
        std::copy(other.buffer.cbegin(), other.buffer.cend(), new_buffer.data() + literal_size - 1);
        return StaticString<new_size>{ new_buffer };
    }

    // clang-format off
    template<std::size_t first_literal_size, std::size_t... literal_sizes>
    [[nodiscard]] consteval auto join(
        StaticString<first_literal_size> const& first,
        StaticString<literal_sizes> const&... strings
    ) const { // clang-format on
        if constexpr (sizeof...(strings) == 0) {
            return first;
        } else {
            return first + ((*this + strings) + ...);
        }
    }

    template<std::size_t... literal_sizes>
    [[nodiscard]] consteval auto join(char const (&... strings)[literal_sizes]) const {
        return join(StaticString<literal_sizes>{ strings }...);
    }

    friend std::ostream& operator<<(std::ostream& stream, StaticString const& string) {
        return stream << string.c_str();
    }
};

template<StaticString string>
[[nodiscard]] consteval auto operator""_static() {
    return string;
}

// clang-format off
template<std::size_t lhs_literal_size, std::size_t rhs_literal_size>
[[nodiscard]] consteval auto operator+(
    char const (&lhs)[lhs_literal_size],
    StaticString<rhs_literal_size> const& rhs
) { // clang-format on
    return StaticString<lhs_literal_size>{ lhs } + rhs;
}
