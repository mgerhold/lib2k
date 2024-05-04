#pragma once

#include "../concepts.hpp"
#include "../string_utils.hpp"
#include "char.hpp"
#include "const_iterator.hpp"
#include "const_reverse_iterator.hpp"
#include <cstdint>
#include <string>
#include <tl/expected.hpp>

namespace c2k {
    namespace detail {
        class Utf8ConstReverseIterator;
    }
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
        using ReverseIterator = detail::Utf8ConstReverseIterator;

        Utf8String() = default;
        Utf8String(std::string string);  // NOLINT (implicit converting constructor)
        Utf8String(char const* string);  // NOLINT (implicit converting constructor)
        Utf8String(Utf8StringView view); // NOLINT (implicit converting constructor)

        Utf8String(ConstIterator const& begin, ConstIterator const& end);

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

        [[nodiscard]] Utf8String substring(ConstIterator const& begin, ConstIterator const& end) const;
        [[nodiscard]] Utf8String substring(ConstIterator const& begin) const;
        [[nodiscard]] Utf8String substring(ConstIterator const& begin, std::size_t num_chars) const;
        [[nodiscard]] Utf8String substring(std::size_t start, std::size_t num_chars) const;
        [[nodiscard]] Utf8String substring(std::size_t start) const;

        [[nodiscard]] bool operator==(Utf8String const& other) const;
        [[nodiscard]] bool operator==(Utf8StringView other) const;
        [[nodiscard]] bool operator==(char const* other) const;

        [[nodiscard]] Utf8Char front() const;
        [[nodiscard]] Utf8Char back() const;

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

        [[nodiscard]] ReverseIterator rbegin() const {
            return ReverseIterator{
                reinterpret_cast<std::byte const*>(m_data.data()),
                reinterpret_cast<std::byte const*>(m_data.data() + m_data.size()),
            };
        }

        [[nodiscard]] ReverseIterator crbegin() const {
            return rbegin();
        }

        [[nodiscard]] ReverseIterator rend() const {
            return ReverseIterator{
                reinterpret_cast<std::byte const*>(m_data.data()),
                reinterpret_cast<std::byte const*>(m_data.data()),
            };
        }

        [[nodiscard]] ReverseIterator crend() const {
            return rend();
        }

        void append(Utf8Char c);

        void append(char const* c_string);

        void append(std::string const& string);

        void append(Utf8String const& string);

        void append(Utf8StringView view);

        Utf8String& operator+=(Utf8Char const c) {
            append(c);
            return *this;
        }

        Utf8String& operator+=(char const* c_string);

        Utf8String& operator+=(std::string const& string);

        Utf8String& operator+=(Utf8String const& other) {
            append(other);
            return *this;
        }

        Utf8String& operator+=(Utf8StringView other);

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

        [[nodiscard]] Utf8String join(Iterable<Utf8StringView> auto const& iterable) const {
            if (std::cbegin(iterable) == std::cend(iterable)) {
                return {};
            }
            auto it = std::cbegin(iterable);
            auto result = Utf8String{ *it };
            ++it;
            for (; it != std::cend(iterable); ++it) {
                result += *this;
                result += *it;
            }
            return result;
        }

        [[nodiscard]] std::vector<Utf8String> split(Utf8StringView delimiter) const;

        // clang-format off
        [[nodiscard]] Utf8String replace(
            Utf8StringView to_replace,
            Utf8StringView replacement,
            ConstIterator const& start,
            MaxReplacementCount max_num_replacements
        ) const; // clang-format on

        [[nodiscard]] Utf8String replace(Utf8StringView to_replace, Utf8StringView replacement) const;

        // clang-format off
        [[nodiscard]] Utf8String replace(
            Utf8StringView to_replace,
            Utf8StringView replacement,
            ConstIterator const& start
        ) const;

        [[nodiscard]] Utf8String replace(
            Utf8StringView to_replace,
            Utf8StringView replacement,
            MaxReplacementCount max_num_replacements
        ) const;
        // clang-format on

        friend std::ostream& operator<<(std::ostream& os, Utf8String const& string) {
            return os << string.m_data;
        }
    };
} // namespace c2k
