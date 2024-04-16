#pragma once

#include "../concepts.hpp"
#include "char.hpp"
#include "const_iterator.hpp"
#include <cstdint>
#include <string>
#include <tl/expected.hpp>

namespace c2k {
    class Utf8String;

    namespace Utf8Literals {
        [[nodiscard]] Utf8String operator""_utf8(char const* str, std::size_t length);
    }

    class Utf8String final {
        friend class Utf8StringView;
        friend Utf8String Utf8Literals::operator""_utf8(char const* str, std::size_t length);

    private:
        std::string m_data;

    public:
        using ConstIterator = detail::Utf8ConstIterator;

        Utf8String() = default;
        Utf8String(std::string string);      // NOLINT (implicit converting constructor)
        Utf8String(char const* const string) // NOLINT (implicit converting constructor)
            : Utf8String{ std::string{ string } } { }
        [[nodiscard]] static Utf8String from_string_unchecked(std::string data);
        [[nodiscard]] static tl::expected<Utf8String, Utf8Error> from_chars(std::string chars);
        [[nodiscard]] static bool is_valid_utf8(std::string_view string);

        [[nodiscard]] char const* c_str() const {
            return m_data.data();
        }

        [[nodiscard]] std::size_t calculate_char_count() const {
            return static_cast<std::size_t>(cend() - cbegin());
        }

        [[nodiscard]] std::size_t calculate_char_width() const;

        [[nodiscard]] bool operator==(Utf8String const& other) const;

        [[nodiscard]] ConstIterator begin() const {
            return ConstIterator{ reinterpret_cast<std::byte const*>(m_data.data()) };
        }

        [[nodiscard]] ConstIterator cbegin() const {
            return begin();
        }

        [[nodiscard]] ConstIterator end() const {
            return ConstIterator{ reinterpret_cast<std::byte const*>(m_data.data() + m_data.size()) };
        }

        [[nodiscard]] ConstIterator cend() const {
            return end();
        }

        void append(Utf8Char c);

        void append(Utf8String const& string);

        Utf8String& operator+=(Utf8Char const c) {
            append(c);
            return *this;
        }

        Utf8String& operator+=(Utf8String const& other) {
            append(other);
            return *this;
        }

        [[nodiscard]] Utf8String operator+(Utf8Char c) const;
        [[nodiscard]] Utf8String operator+(Utf8String const& other) const;
        friend Utf8String operator+(Utf8Char c, Utf8String const& string);
        friend Utf8String operator+(Utf8String const& lhs, Utf8String const& rhs);

        [[nodiscard]] bool is_empty() const {
            return m_data.empty();
        }

        void clear() {
            m_data.clear();
        }

        void reserve(std::size_t new_capacity_in_bytes);

        [[nodiscard]] ConstIterator find(Utf8Char needle) const;
        [[nodiscard]] ConstIterator find(Utf8Char needle, ConstIterator const& start) const;
        [[nodiscard]] ConstIterator find(Utf8Char needle, ConstIterator::difference_type start_position) const;
        [[nodiscard]] ConstIterator find(Utf8String const& needle) const;
        [[nodiscard]] ConstIterator find(Utf8String const& needle, ConstIterator const& start) const;
        [[nodiscard]] ConstIterator find(Utf8String const& needle, ConstIterator::difference_type start_position) const;
        ConstIterator erase(ConstIterator const& position);
        ConstIterator erase(ConstIterator const& first, ConstIterator const& last);
        void reverse();
        [[nodiscard]] Utf8String to_uppercase() const;
        [[nodiscard]] Utf8String to_lowercase() const;

        [[nodiscard]] Utf8String transform(Invocable<Utf8Char, Utf8Char> auto&& transformer) const {
            auto result = Utf8String{};
            for (auto const c : *this) {
                result += transformer(c);
            }
            return result;
        }

        friend std::ostream& operator<<(std::ostream& os, Utf8String const& string) {
            return os << string.m_data;
        }
    };
} // namespace c2k
