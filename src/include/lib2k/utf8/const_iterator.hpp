#pragma once

#include "char.hpp"
#include <cstddef>
#include <cstdint>
#include <iterator>

namespace c2k {
    class Utf8String;
    class Utf8StringView;

    namespace detail {
        class Utf8ConstReverseIterator;

        class Utf8ConstIterator final {
            friend class ::c2k::Utf8String;
            friend class ::c2k::Utf8StringView;
            friend class ::c2k::detail::Utf8ConstReverseIterator;

        private:
            std::byte const* m_next_char_start{ nullptr };
            std::uint8_t m_next_char_num_bytes{ 0 };
            // We have to keep a copy of the Utf8Char around to make operator->() work. Otherwise, we'd not
            // have an address that we could return there.
            Utf8Char m_next;

            explicit Utf8ConstIterator(std::byte const* start);
            Utf8ConstIterator(std::byte const* next_char_start, std::uint8_t next_char_num_bytes, Utf8Char next);

        public:
            using difference_type = std::ptrdiff_t;
            using value_type = Utf8Char;

            constexpr Utf8ConstIterator() = default;

            [[nodiscard]] Utf8Char const& operator*() const;
            [[nodiscard]] Utf8Char const* operator->() const;
            Utf8ConstIterator& operator++();
            [[nodiscard]] Utf8ConstIterator operator++(int);
            Utf8ConstIterator& operator--();
            [[nodiscard]] Utf8ConstIterator operator--(int);
            [[nodiscard]] Utf8ConstIterator operator+(difference_type offset) const;

            [[nodiscard]] friend Utf8ConstIterator
            operator+(difference_type const offset, Utf8ConstIterator const& iterator) {
                return iterator + offset;
            }

            [[nodiscard]] difference_type operator-(Utf8ConstIterator const& other) const;
            [[nodiscard]] Utf8ConstIterator operator-(difference_type offset) const;
            Utf8ConstIterator& operator+=(difference_type offset);
            Utf8ConstIterator& operator-=(difference_type offset);

            [[nodiscard]] constexpr bool operator==(Utf8ConstIterator const& other) const {
                return m_next_char_start == other.m_next_char_start
                       and m_next_char_num_bytes == other.m_next_char_num_bytes;
            }
        };

        static_assert(std::input_or_output_iterator<Utf8ConstIterator>);
        static_assert(std::input_iterator<Utf8ConstIterator>);
        static_assert(std::forward_iterator<Utf8ConstIterator>);
        static_assert(std::bidirectional_iterator<Utf8ConstIterator>);
    } // namespace detail
} // namespace c2k
