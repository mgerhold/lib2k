#pragma once

#include "char.hpp"
#include <cstddef>
#include <cstdint>
#include <iterator>

namespace c2k {
    class Utf8String;
    class Utf8StringView;

    namespace detail {
        class Utf8ConstIterator final {
            friend class ::c2k::Utf8String;
            friend class ::c2k::Utf8StringView;

        private:
            std::byte const* m_next_char_start{ nullptr };
            std::uint8_t m_next_char_num_bytes{ 0 };
            // We have to keep a copy of the Utf8Char around to make operator->() work. Otherwise, we'd not
            // have an address that we could return there.
            Utf8Char m_next;

            explicit Utf8ConstIterator(std::byte const* start);

        public:
            using difference_type = std::ptrdiff_t;
            using value_type = Utf8Char;

            constexpr Utf8ConstIterator() = default;

            [[nodiscard]] Utf8Char const& operator*() const;
            [[nodiscard]] Utf8Char const* operator->() const;
            Utf8ConstIterator& operator++();
            Utf8ConstIterator operator++(int);
            Utf8ConstIterator& operator--();
            Utf8ConstIterator operator--(int);
            [[nodiscard]] Utf8ConstIterator operator+(difference_type offset) const;
            [[nodiscard]] difference_type operator-(Utf8ConstIterator const& other) const;
            [[nodiscard]] bool operator==(Utf8ConstIterator const& other) const;
        };

        static_assert(std::input_or_output_iterator<Utf8ConstIterator>);
        static_assert(std::input_iterator<Utf8ConstIterator>);
        static_assert(std::forward_iterator<Utf8ConstIterator>);
        static_assert(std::bidirectional_iterator<Utf8ConstIterator>);
    } // namespace detail
} // namespace c2k
