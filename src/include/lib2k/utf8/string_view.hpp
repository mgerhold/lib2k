#pragma once

#include "../concepts.hpp"
#include "../string_utils.hpp"
#include "const_iterator.hpp"
#include "const_reverse_iterator.hpp"
#include "string.hpp"
#include <string_view>

namespace c2k {
    class Utf8String;

    namespace detail {
        class Utf8ConstIterator;
    }

    class Utf8StringView final {
        friend class Utf8String;

    private:
        std::string_view m_view;

    public:
        using ConstIterator = detail::Utf8ConstIterator;
        using ReverseIterator = detail::Utf8ConstReverseIterator;

        constexpr Utf8StringView() = default;
        Utf8StringView(Utf8String const& string);  // NOLINT (implicit converting constructor)
        Utf8StringView(std::string const& string); // NOLINT (implicit converting constructor)
        Utf8StringView(detail::Utf8ConstIterator const& begin, detail::Utf8ConstIterator const& end);
        Utf8StringView(std::string_view view); // NOLINT (implicit converting constructor)
        Utf8StringView(char const* chars);     // NOLINT (implicit converting constructor)

        [[nodiscard]] static Utf8StringView from_string_view_unchecked(std::string_view view);

        [[nodiscard]] constexpr bool is_empty() const {
            return m_view.empty();
        }

        [[nodiscard]] std::size_t num_bytes() const {
            return m_view.length();
        }

        [[nodiscard]] std::size_t calculate_char_count() const {
            return static_cast<std::size_t>(cend() - cbegin());
        }

        [[nodiscard]] std::size_t calculate_char_width() const;

        [[nodiscard]] Utf8StringView substring(ConstIterator begin, ConstIterator end) const;
        [[nodiscard]] Utf8StringView substring(ConstIterator begin) const;
        [[nodiscard]] Utf8StringView substring(ConstIterator begin, std::size_t num_chars) const;
        [[nodiscard]] Utf8StringView substring(std::size_t start, std::size_t num_chars) const;
        [[nodiscard]] Utf8StringView substring(std::size_t start) const;

        [[nodiscard]] constexpr bool operator==(Utf8StringView const other) const {
            return m_view == other.m_view;
        }

        [[nodiscard]] Utf8Char front() const;
        [[nodiscard]] Utf8Char back() const;

        [[nodiscard]] detail::Utf8ConstIterator begin() const {
            return detail::Utf8ConstIterator{ reinterpret_cast<std::byte const*>(m_view.data()) };
        }

        [[nodiscard]] detail::Utf8ConstIterator cbegin() const {
            return begin();
        }

        [[nodiscard]] detail::Utf8ConstIterator end() const {
            return detail::Utf8ConstIterator{
                reinterpret_cast<std::byte const*>(m_view.data() + m_view.length()),
            };
        }

        [[nodiscard]] detail::Utf8ConstIterator cend() const {
            return end();
        }

        [[nodiscard]] ReverseIterator rbegin() const {
            return ReverseIterator{
                reinterpret_cast<std::byte const*>(m_view.data()),
                reinterpret_cast<std::byte const*>(m_view.data() + m_view.size()),
            };
        }

        [[nodiscard]] ReverseIterator crbegin() const {
            return rbegin();
        }

        [[nodiscard]] ReverseIterator rend() const {
            return ReverseIterator{
                reinterpret_cast<std::byte const*>(m_view.data()),
                reinterpret_cast<std::byte const*>(m_view.data()),
            };
        }

        [[nodiscard]] ReverseIterator crend() const {
            return rend();
        }

        [[nodiscard]] ConstIterator find(Utf8Char needle) const;
        [[nodiscard]] ConstIterator find(Utf8Char needle, ConstIterator const& start) const;
        [[nodiscard]] ConstIterator find(Utf8Char needle, ConstIterator::difference_type start_position) const;
        [[nodiscard]] ConstIterator find(Utf8StringView needle) const;
        [[nodiscard]] ConstIterator find(Utf8StringView needle, ConstIterator const& start) const;
        [[nodiscard]] ConstIterator find(Utf8StringView needle, ConstIterator::difference_type start_position) const;

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

        [[nodiscard]] std::vector<Utf8StringView> split(Utf8StringView delimiter);

        // clang-format off
        [[nodiscard]] Utf8String replace(
            Utf8StringView to_replace,
            Utf8StringView replacement,
            ConstIterator const& start,
            MaxReplacementCount max_num_replacements
        ) const; // clang-format on

        // clang-format off
        [[nodiscard]] Utf8String replace(
            Utf8StringView const to_replace,
            Utf8StringView const replacement
        ) const { // clang-format on
            return replace(
                    to_replace,
                    replacement,
                    cbegin(),
                    MaxReplacementCount{ std::numeric_limits<std::underlying_type_t<MaxReplacementCount>>::max() }
            );
        }

        // clang-format off
        [[nodiscard]] Utf8String replace(
            Utf8StringView const to_replace,
            Utf8StringView const replacement,
            ConstIterator const& start
        ) const { // clang-format on
            return replace(
                    to_replace,
                    replacement,
                    start,
                    MaxReplacementCount{ std::numeric_limits<std::underlying_type_t<MaxReplacementCount>>::max() }
            );
        }

        // clang-format off
        [[nodiscard]] Utf8String replace(
            Utf8StringView const to_replace,
            Utf8StringView const replacement,
            MaxReplacementCount const max_num_replacements
        ) const { // clang-format on
            return replace(to_replace, replacement, cbegin(), max_num_replacements);
        }
    };

    namespace Utf8Literals {
        [[nodiscard]] Utf8StringView operator""_utf8view(char const* str, std::size_t length);
    }
} // namespace c2k
