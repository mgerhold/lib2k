#include <lib2k/utf8/const_iterator.hpp>
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

    [[nodiscard]] Utf8ConstIterator Utf8ConstIterator::operator+(difference_type const offset) const {
        if (offset < 0) {
            throw std::invalid_argument{ "cannot add negative value to Utf8String::ConstIterator" };
        }
        auto copy = *this;
        for (auto i = difference_type{ 0 }; i < offset; ++i) {
            ++copy;
        }
        return copy;
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

    [[nodiscard]] bool Utf8ConstIterator::operator==(Utf8ConstIterator const& other) const {
        return m_next_char_start == other.m_next_char_start and m_next_char_num_bytes == other.m_next_char_num_bytes;
    }
} // namespace c2k::detail
