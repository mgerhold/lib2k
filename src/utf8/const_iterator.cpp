#include <lib2k/utf8/const_iterator.hpp>
#include <lib2k/utf8/string.hpp>
#include <lib2k/utf8/string_view.hpp>
#include <utf8proc.h>

namespace c2k::detail {
    Utf8ConstIterator::Utf8ConstIterator(std::byte const* const start) : m_next_char_start{ start } {
        auto utf8proc_codepoint = utf8proc_int32_t{};
        auto const result = utf8proc_iterate(
                static_cast<utf8proc_uint8_t const*>(static_cast<void const*>(start)),
                -1,
                &utf8proc_codepoint
        );
        assert(result >= 0);
        m_next_char_num_bytes = static_cast<decltype(m_next_char_num_bytes)>(result);

        auto codepoint = Utf8Char::Codepoint{};
        for (auto i = decltype(m_next_char_num_bytes){ 0 }; i < m_next_char_num_bytes; ++i) {
            codepoint.push_back(*(m_next_char_start + i));
        }
        m_next = Utf8Char{ codepoint };
    }

    Utf8ConstIterator::Utf8ConstIterator(
            std::byte const* const next_char_start,
            std::uint8_t const next_char_num_bytes,
            Utf8Char const next
    )
        : m_next_char_start{ next_char_start },
          m_next_char_num_bytes{ next_char_num_bytes },
          m_next{ next } { }

    [[nodiscard]] Utf8Char const& Utf8ConstIterator::operator*() const {
        return m_next;
    }

    [[nodiscard]] Utf8Char const* Utf8ConstIterator::operator->() const {
        return &m_next;
    }

    Utf8ConstIterator& Utf8ConstIterator::operator++() {
        if (*m_next_char_start == std::byte{ 0 }) {
            return *this;
        }
        m_next_char_start += m_next_char_num_bytes;
        auto utf8proc_codepoint = utf8proc_int32_t{};
        auto const result = utf8proc_iterate(
                static_cast<utf8proc_uint8_t const*>(static_cast<void const*>(m_next_char_start)),
                -1,
                &utf8proc_codepoint
        );
        assert(result >= 0);
        m_next_char_num_bytes = static_cast<decltype(m_next_char_num_bytes)>(result);

        auto codepoint = Utf8Char::Codepoint{};
        for (auto i = decltype(m_next_char_num_bytes){ 0 }; i < m_next_char_num_bytes; ++i) {
            codepoint.push_back(*(m_next_char_start + i));
        }
        m_next = Utf8Char{ codepoint };

        return *this;
    }

    Utf8ConstIterator Utf8ConstIterator::operator++(int) {
        auto const result = *this;
        ++(*this);
        return result;
    }

    Utf8ConstIterator& Utf8ConstIterator::operator--() {
        static constexpr auto is_continuation_byte = [](char const byte) {
            // see (for example): https://stackoverflow.com/a/49701599/7540548
            return (byte & 0b1100'0000) == 0b1000'0000;
        };
        auto pointer = reinterpret_cast<char const*>(m_next_char_start);
        auto num_bytes = std::size_t{ 0 };
        do {
            --pointer;
            ++num_bytes;
        } while (is_continuation_byte(*pointer));
        m_next_char_start = reinterpret_cast<std::byte const*>(pointer);
        m_next_char_num_bytes = num_bytes;
        auto codepoint = Utf8Char::Codepoint{};
        for (auto i = std::size_t{ 0 }; i < num_bytes; ++i) {
            codepoint.push_back(*(m_next_char_start + i));
        }
        m_next = Utf8Char{ codepoint };
        return *this;
    }

    Utf8ConstIterator Utf8ConstIterator::operator--(int) {
        auto const result = *this;
        --(*this);
        return result;
    }

    [[nodiscard]] Utf8ConstIterator Utf8ConstIterator::operator+(difference_type const offset) const {
        auto copy = *this;
        copy += offset;
        return copy;
    }

    Utf8ConstIterator& Utf8ConstIterator::operator+=(difference_type const offset) {
        if (offset == 0) {
            return *this;
        }
        if (offset > 0) {
            for (auto i = difference_type{ 0 }; i < offset; ++i) {
                ++(*this);
            }
            return *this;
        }
        for (auto i = difference_type{ 0 }; i < -offset; ++i) {
            --(*this);
        }
        return *this;
    }

    Utf8ConstIterator& Utf8ConstIterator::operator-=(difference_type const offset) {
        return (*this) += -offset;
    }

    // clang-format off
    [[nodiscard]] Utf8ConstIterator::difference_type Utf8ConstIterator::operator-(
        Utf8ConstIterator const& other
    ) const { // clang-format on
        auto distance = difference_type{ 0 };
        for (auto it = other; it != *this; ++it) {
            ++distance;
        }
        return distance;
    }

    [[nodiscard]] Utf8ConstIterator Utf8ConstIterator::operator-(difference_type const offset) const {
        return *this + (-offset);
    }
} // namespace c2k::detail
