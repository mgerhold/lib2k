#pragma once

#include <cstdint>
#include <iostream>
#include <lib2k/static_vector.hpp>
#include <span>
#include <stdexcept>
#include <string>
#include <tl/expected.hpp>

namespace c2k {
    enum class Utf8Error {
        InvalidUtf8String,
        InvalidUtf8Char,
        InvalidBytesRange,
    };

    class InvalidUtf8Char final : public std::runtime_error {
    public:
        InvalidUtf8Char() : std::runtime_error{ "Invalid UTF-8 char" } { }
    };

    class InvalidUtf8String final : public std::runtime_error {
    public:
        InvalidUtf8String() : std::runtime_error{ "Invalid UTF-8 string" } { }
    };

    class Utf8Char final {
        friend class Utf8String;

    private:
        using Codepoint = StaticVector<std::byte, 4>;

        Codepoint m_codepoint;

        explicit constexpr Utf8Char(Codepoint const codepoint) : m_codepoint{ codepoint } { }

    public:
        constexpr explicit Utf8Char(char const c) {
            auto const as_unsigned = static_cast<unsigned char>(c);
            if (as_unsigned > 127) {
                throw InvalidUtf8Char{};
            }
            m_codepoint = Codepoint{ std::byte{ as_unsigned } };
        }

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

        [[nodiscard]] bool constexpr operator==(Utf8Char const other) const {
            return m_codepoint == other.m_codepoint;
        }

        friend std::ostream& operator<<(std::ostream& os, Utf8Char const c) {
            for (auto const byte : c.m_codepoint) {
                os << static_cast<unsigned char>(byte);
            }
            return os;
        }
    };

    class Utf8String;

    namespace Utf8Literals {
        [[nodiscard]] constexpr Utf8Char operator""_utf8(char const c) {
            return Utf8Char{ c };
        }

        [[nodiscard]] Utf8String operator""_utf8(char const* str, std::size_t length);
    } // namespace Utf8Literals

    class Utf8String final {
        friend Utf8String Utf8Literals::operator""_utf8(char const* str, std::size_t length);

    private:
        std::string m_data;
        std::size_t m_num_chars;

        class ConstIterator {
            friend class Utf8String;

        private:
            std::byte const* m_next_char_start{ nullptr };
            std::uint8_t m_next_char_num_bytes{ 0 };

            explicit ConstIterator(std::byte const* start);

        public:
            using difference_type = std::ptrdiff_t;
            using value_type = Utf8Char;

            constexpr ConstIterator() = default;
            ConstIterator(ConstIterator const&) = default;
            ConstIterator(ConstIterator&&) = default;
            ConstIterator& operator=(ConstIterator const&) = default;
            ConstIterator& operator=(ConstIterator&&) = default;

            [[nodiscard]] Utf8Char operator*() const;
            ConstIterator& operator++();
            ConstIterator operator++(int);
            [[nodiscard]] bool operator==(ConstIterator const& other) const;
        };

        static_assert(std::input_or_output_iterator<ConstIterator>);
        static_assert(std::input_iterator<ConstIterator>);
        // todo: check why the following assert fails with GCC and clang
        // static_assert(std::forward_iterator<ConstIterator>);

        explicit Utf8String(std::string data, std::size_t const num_chars)
            : m_data{ std::move(data) },
              m_num_chars{ num_chars } { }

    public:
        [[nodiscard]] static tl::expected<Utf8String, Utf8Error> from_chars(std::string chars);
        [[nodiscard]] static tl::expected<std::size_t, Utf8Error> try_get_utf8_num_chars(std::string_view buffer);

        [[nodiscard]] char const* c_str() const {
            return m_data.data();
        }

        [[nodiscard]] std::size_t num_chars() const {
            return m_num_chars;
        }

        [[nodiscard]] std::size_t char_width() const;

        [[nodiscard]] bool operator==(Utf8String const& other) const;

        [[nodiscard]] ConstIterator begin() const {
            return ConstIterator{
                static_cast<std::byte const*>(static_cast<void const*>(m_data.data())),
            };
        }

        [[nodiscard]] ConstIterator end() const {
            return ConstIterator{
                static_cast<std::byte const*>(static_cast<void const*>(m_data.data() + m_data.size())),
            };
        }
    };
} // namespace c2k
