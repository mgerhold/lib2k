#pragma once

#include "../static_vector.hpp"
#include "errors.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <span>
#include <tl/expected.hpp>

namespace c2k {

    namespace detail {
        class Utf8ConstIterator;
    }

    class Utf8Char final {
        friend class detail::Utf8ConstIterator;
        friend class Utf8String;
        friend class Utf8StringView;

    private:
        using Codepoint = StaticVector<std::byte, 4>;

        Codepoint m_codepoint;

        explicit constexpr Utf8Char(Codepoint const codepoint) : m_codepoint{ codepoint } { }

    public:
        constexpr Utf8Char() : m_codepoint{ std::byte{ 0 } } { }

        Utf8Char(char c); // NOLINT (implicit converting constructor)
        Utf8Char(Utf8Char const&) = default;
        Utf8Char(Utf8Char&&) = default;
        Utf8Char& operator=(Utf8Char const&) & = default;
        Utf8Char& operator=(Utf8Char const&) && = delete;
        Utf8Char& operator=(Utf8Char&&) & = default;
        Utf8Char& operator=(Utf8Char&&) && = delete;
        ~Utf8Char() noexcept = default;

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

        [[nodiscard]] static tl::expected<Utf8Char, Utf8Error> from_codepoint(std::int32_t codepoint);

        [[nodiscard]] std::string_view as_string_view() const {
            return std::string_view{
                reinterpret_cast<char const*>(m_codepoint.data()),
                reinterpret_cast<char const*>(m_codepoint.data() + m_codepoint.size()),
            };
        }

        [[nodiscard]] std::int32_t codepoint() const;

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

    namespace Utf8Literals {
        [[nodiscard]] Utf8Char operator""_utf8(char c);
    };

} // namespace c2k
