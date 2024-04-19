#include <lib2k/utf8/const_reverse_iterator.hpp>

namespace c2k::detail {

    Utf8ConstReverseIterator::Utf8ConstReverseIterator(
            std::byte const* string_start,
            std::byte const* forward_iterator_start
    )
        : m_string_start{ string_start } {
        auto it = Utf8ConstIterator{ forward_iterator_start };
        m_is_sentinel = (it.m_next_char_start == string_start);
        if (not m_is_sentinel) {
            --it;
        }
        m_current_char_start = it.m_next_char_start;
        m_current_char_num_bytes = it.m_next_char_num_bytes;
        m_current = *it;
    }

    Utf8ConstReverseIterator& Utf8ConstReverseIterator::operator++() {
        if (m_current_char_start == m_string_start) {
            m_is_sentinel = true;
        }
        if (m_is_sentinel) {
            return *this;
        }
        auto it = Utf8ConstIterator{ m_current_char_start, m_current_char_num_bytes, m_current };
        --it;
        m_current_char_start = it.m_next_char_start;
        m_current_char_num_bytes = it.m_next_char_num_bytes;
        m_current = *it;
        return *this;
    }

    Utf8ConstReverseIterator& Utf8ConstReverseIterator::operator--() {
        if (m_is_sentinel) {
            auto const begin = Utf8ConstIterator{ m_string_start };
            m_current_char_start = begin.m_next_char_start;
            m_current_char_num_bytes = begin.m_next_char_num_bytes;
            m_current = *begin;
            m_is_sentinel = false;
            return *this;
        }
        auto it = Utf8ConstIterator{ m_current_char_start, m_current_char_num_bytes, m_current };
        ++it;
        m_current_char_start = it.m_next_char_start;
        m_current_char_num_bytes = it.m_next_char_num_bytes;
        m_current = *it;
        return *this;
    }
} // namespace c2k::detail
