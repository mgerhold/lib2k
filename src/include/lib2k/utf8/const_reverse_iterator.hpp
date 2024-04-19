#pragma once

#include "char.hpp"
#include "const_iterator.hpp"

namespace c2k::detail {
    class Utf8ConstReverseIterator {
        // we cannot simply use std::reverse_iterator since we work with a proxy object
        friend class ::c2k::Utf8String;
        friend class ::c2k::Utf8StringView;

    public:
        using difference_type = Utf8ConstIterator::difference_type;
        using value_type = Utf8ConstIterator::value_type;

    private:
        std::byte const* m_string_start{ nullptr };
        std::byte const* m_current_char_start{ nullptr };
        std::uint8_t m_current_char_num_bytes{ 0 };
        Utf8Char m_current;
        bool m_is_sentinel{ false };

        explicit Utf8ConstReverseIterator(std::byte const* string_start, std::byte const* forward_iterator_start);

    public:
        constexpr Utf8ConstReverseIterator() = default;

        [[nodiscard]] Utf8ConstIterator base() const;

        [[nodiscard]] Utf8Char const& operator*() const {
            return m_current;
        }

        [[nodiscard]] Utf8Char const* operator->() const {
            return &m_current;
        }

        Utf8ConstReverseIterator& operator++();

        [[nodiscard]] Utf8ConstReverseIterator operator++(int) {
            auto const result = *this;
            ++(*this);
            return result;
        }

        Utf8ConstReverseIterator& operator+=(difference_type offset);

        [[nodiscard]] Utf8ConstReverseIterator operator+(difference_type const offset) const {
            auto copy = *this;
            copy += offset;
            return copy;
        }

        // clang-format off
        [[nodiscard]] friend Utf8ConstReverseIterator operator+(
            difference_type const offset,
            Utf8ConstReverseIterator const& iterator
        ) { // clang-format on
            return iterator + offset;
        }

        Utf8ConstReverseIterator& operator--();

        Utf8ConstReverseIterator& operator-=(difference_type const offset) {
            return (*this) += -offset;
        }

        [[nodiscard]] Utf8ConstReverseIterator operator--(int) {
            auto const result = *this;
            --(*this);
            return result;
        }

        [[nodiscard]] difference_type operator-(Utf8ConstReverseIterator const& other) const;

        [[nodiscard]] Utf8ConstReverseIterator operator-(difference_type const offset) const {
            return *this + (-offset);
        }

        [[nodiscard]] constexpr bool operator==(Utf8ConstReverseIterator const& other) const {
            assert(m_string_start == other.m_string_start and "iterators must stem from the same source string");
            if (not m_is_sentinel and not other.m_is_sentinel) {
                return m_current_char_start == other.m_current_char_start;
            }
            return m_is_sentinel == other.m_is_sentinel;
        }
    };

    static_assert(std::input_or_output_iterator<Utf8ConstReverseIterator>);
    static_assert(std::input_iterator<Utf8ConstReverseIterator>);
    static_assert(std::forward_iterator<Utf8ConstReverseIterator>);
    static_assert(std::bidirectional_iterator<Utf8ConstReverseIterator>);
} // namespace c2k::detail
