#pragma once

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

        [[nodiscard]] constexpr bool empty() const {
            return m_view.empty();
        }

        [[nodiscard]] constexpr bool operator==(Utf8StringView const other) const {
            return m_view == other.m_view;
        }

        [[nodiscard]] detail::Utf8ConstIterator begin() const {
            return detail::Utf8ConstIterator{
                static_cast<std::byte const*>(static_cast<void const*>(&m_view.front())),
            };
        }
    };

    namespace Utf8Literals {
        [[nodiscard]] Utf8StringView operator""_utf8view(char const* str, std::size_t length);
    }
} // namespace c2k
