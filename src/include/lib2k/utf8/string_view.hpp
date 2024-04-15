#pragma once

#include "const_iterator.hpp"
#include <string_view>

namespace c2k {
    class Utf8String;

    namespace detail {
        class Utf8ConstIterator;
    }

    class Utf8StringView final {
    private:
        std::string_view m_view;

    public:
        constexpr Utf8StringView() = default;
        Utf8StringView(Utf8String const& string); // NOLINT (implicit converting constructor)
        Utf8StringView(detail::Utf8ConstIterator const& begin, detail::Utf8ConstIterator const& end);
        Utf8StringView(std::string_view view); // NOLINT (implicit converting constructor)
        Utf8StringView(char const* chars);     // NOLINT (implicit converting constructor)

        [[nodiscard]] static Utf8StringView from_string_view_unchecked(std::string_view view);

        [[nodiscard]] constexpr bool is_empty() const {
            return m_view.empty();
        }

        [[nodiscard]] std::size_t calculate_char_count() const {
            return static_cast<std::size_t>(cend() - cbegin());
        }

        [[nodiscard]] std::size_t calculate_char_width() const;

        [[nodiscard]] constexpr bool operator==(Utf8StringView const other) const {
            return m_view == other.m_view;
        }

        [[nodiscard]] detail::Utf8ConstIterator begin() const {
            return detail::Utf8ConstIterator{
                reinterpret_cast<std::byte const*>(m_view.data()),
            };
        }

        [[nodiscard]] detail::Utf8ConstIterator cbegin() const {
            return begin();
        }

        [[nodiscard]] detail::Utf8ConstIterator end() const {
            return detail::Utf8ConstIterator{ reinterpret_cast<std::byte const*>(m_view.data() + m_view.length()) };
        }

        [[nodiscard]] detail::Utf8ConstIterator cend() const {
            return end();
        }
    };

    namespace Utf8Literals {
        [[nodiscard]] Utf8StringView operator""_utf8view(char const* str, std::size_t length);
    }
} // namespace c2k
