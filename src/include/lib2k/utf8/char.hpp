#pragma once

#include "../static_vector.hpp"
#include "errors.hpp"
#include <cassert>
#include <iostream>
#include <span>
#include <tl/expected.hpp>

namespace c2k {

    class Utf8Char final {
        friend class Utf8String;

    private:
        using Codepoint = StaticVector<std::byte, 4>;

        Codepoint m_codepoint;

        explicit constexpr Utf8Char(Codepoint const codepoint) : m_codepoint{ codepoint } { }

    public:
        constexpr Utf8Char() : m_codepoint{ std::byte{ 0 } } { }

        Utf8Char(char c);

        [[nodiscard]] static constexpr Utf8Char from_bytes_unchecked(std::span<std::byte const> const bytes) {
            assert(not bytes.empty());
            assert(bytes.size_bytes() <= Codepoint::capacity());
            auto codepoint = Codepoint{};
            for (auto const byte : bytes) {
                codepoint.push_back(byte);
            }
            return Utf8Char{ codepoint };
        }

        [[nodiscard]] static tl::expected<Utf8Char, Utf8Error> from_bytes(std::span<std::byte const> bytes);

        [[nodiscard]] bool constexpr operator==(Utf8Char const& other) const = default;

        [[nodiscard]] bool is_uppercase() const;
        [[nodiscard]] bool is_lowercase() const;
        [[nodiscard]] Utf8Char to_uppercase() const;
        [[nodiscard]] Utf8Char to_lowercase() const;

        friend std::ostream& operator<<(std::ostream& os, Utf8Char const c) {
            for (auto const byte : c.m_codepoint) {
                os << static_cast<unsigned char>(byte);
            }
            return os;
        }
    };

} // namespace c2k
