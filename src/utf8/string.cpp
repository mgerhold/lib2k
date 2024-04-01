#include <lib2k/utf8/char.hpp>
#include <lib2k/utf8/string.hpp>
#include <utf8proc.h>

namespace c2k {
    [[nodiscard]] Utf8String operator+(Utf8Char const c, Utf8String const& string) {
        auto result = Utf8String{};
        result += c;
        result += string;
        return result;
    }

    [[nodiscard]] Utf8String operator+(Utf8String const& lhs, Utf8String const& rhs) {
        auto result = lhs;
        result += rhs;
        return result;
    }

    Utf8String::ConstIterator::ConstIterator(std::byte const* const start) : m_next_char_start{ start } {
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

    [[nodiscard]] Utf8Char const& Utf8String::ConstIterator::operator*() const {
        return m_next;
    }

    [[nodiscard]] Utf8Char const* Utf8String::ConstIterator::operator->() const {
        return &m_next;
    }

    Utf8String::ConstIterator& Utf8String::ConstIterator::operator++() {
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

    Utf8String::ConstIterator Utf8String::ConstIterator::operator++(int) {
        auto const result = *this;
        ++(*this);
        return result;
    }

    [[nodiscard]] Utf8String::ConstIterator Utf8String::ConstIterator::operator+(difference_type const offset) const {
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
    [[nodiscard]] Utf8String::ConstIterator::difference_type Utf8String::ConstIterator::operator-(
        ConstIterator const& other
    ) const { // clang-format on
        auto distance = difference_type{ 0 };
        for (auto it = other; it != *this; ++it) {
            ++distance;
        }
        return distance;
    }

    [[nodiscard]] bool Utf8String::ConstIterator::operator==(ConstIterator const& other) const {
        return m_next_char_start == other.m_next_char_start and m_next_char_num_bytes == other.m_next_char_num_bytes;
    }

    [[nodiscard]] Utf8String Utf8String::from_string_unchecked(std::string data) {
        auto result = Utf8String{};
        result.m_data = std::move(data);
        return result;
    }

    Utf8String::Utf8String(std::string string) {
        if (not is_valid_utf8(string)) {
            throw InvalidUtf8String{};
        }
        m_data = std::move(string);
    }

    [[nodiscard]] tl::expected<Utf8String, Utf8Error> Utf8String::from_chars(std::string chars) {
        if (not is_valid_utf8(chars)) {
            return tl::unexpected{ Utf8Error::InvalidUtf8String };
        }
        return from_string_unchecked(std::move(chars));
    }

    [[nodiscard]] bool Utf8String::is_valid_utf8(std::string_view const string) {
        auto current = string.data();
        auto const end = current + string.length();
        while (current < end) {
            auto const bytes_remaining = static_cast<utf8proc_ssize_t>(end - current);
            auto codepoint = utf8proc_int32_t{};
            auto const result = utf8proc_iterate(
                    static_cast<utf8proc_uint8_t const*>(static_cast<void const*>(current)),
                    bytes_remaining,
                    &codepoint
            );
            if (result < 0) {
                return false;
            }
            current += result;
        }
        return true;
    }

    [[nodiscard]] std::size_t Utf8String::calculate_char_width() const {
        auto width = std::size_t{ 0 };
        auto current = reinterpret_cast<utf8proc_uint8_t const*>(m_data.data());

        while (current < reinterpret_cast<utf8proc_uint8_t const*>(m_data.data() + m_data.length())) {
            auto codepoint = utf8proc_int32_t{};
            auto const bytes_read = utf8proc_iterate(current, -1, &codepoint);
            // clang-format off
            assert(
                codepoint != -1
                and utf8proc_codepoint_valid(codepoint)
                and "Utf8String objects must always contain valid UTF8"
            );
            // clang-format on
            width += static_cast<std::size_t>(utf8proc_charwidth(codepoint));
            current += bytes_read;
        }
        // clang-format off
        assert(
            current == reinterpret_cast<utf8proc_uint8_t const*>(m_data.data() + m_data.length())
            and "Utf8String objects must always contain valid UTF8"
        );
        // clang-format on
        return width;
    }


    [[nodiscard]] bool Utf8String::operator==(Utf8String const& other) const {
        return m_data == other.m_data;
    }

    void Utf8String::append(Utf8Char const c) {
        for (auto const byte : c.m_codepoint) {
            m_data.push_back(static_cast<char>(byte));
        }
    }

    void Utf8String::append(Utf8String const& string) {
        std::copy(string.m_data.cbegin(), string.m_data.cend(), std::back_inserter(m_data));
    }

    [[nodiscard]] Utf8String Utf8String::operator+(Utf8Char const c) const {
        auto copy = *this;
        copy += c;
        return copy;
    }

    [[nodiscard]] Utf8String Utf8String::operator+(Utf8String const& other) const {
        auto copy = *this;
        copy += other;
        return copy;
    }

    void Utf8String::reserve(std::size_t const new_capacity_in_bytes) {
        m_data.reserve(new_capacity_in_bytes);
    }

    [[nodiscard]] Utf8String::ConstIterator Utf8String::find(Utf8Char const needle) const {
        return find(needle, cbegin());
    }


    [[nodiscard]] Utf8String::ConstIterator Utf8String::find(Utf8Char const needle, ConstIterator const& start) const {
        for (auto it = start; it != cend(); ++it) {
            if (*it == needle) {
                return it;
            }
        }
        return cend();
    }

    // clang-format off
    [[nodiscard]] Utf8String::ConstIterator Utf8String::find(
        Utf8Char const needle,
        ConstIterator::difference_type const start_position
    ) const { // clang-format on
        return find(needle, cbegin() + start_position);
    }

    [[nodiscard]] Utf8String::ConstIterator Utf8String::find(Utf8String const& needle) const {
        return find(needle, cbegin());
    }

    // clang-format off
    [[nodiscard]] Utf8String::ConstIterator Utf8String::find(
        Utf8String const& needle,
        ConstIterator const& start
    ) const { // clang-format on
        for (auto it = start; it != cend(); ++it) {
            auto other = needle.cbegin();
            for (auto copy = it; copy != cend() and other != needle.cend(); ++copy, ++other) {
                if (*copy != *other) {
                    break;
                }
            }
            if (other == needle.cend()) {
                return it;
            }
        }
        return cend();
    }

    // clang-format off
    [[nodiscard]] Utf8String::ConstIterator Utf8String::find(
        Utf8String const& needle,
        ConstIterator::difference_type const start_position
    ) const { // clang-format on
        return find(needle, cbegin() + start_position);
    }

    Utf8String::ConstIterator Utf8String::erase(ConstIterator const& position) {
        return erase(position, position + 1);
    }

    Utf8String::ConstIterator Utf8String::erase(ConstIterator const& first, ConstIterator const& last) {
        auto const start_byte_offset = static_cast<std::size_t>(
                static_cast<char const*>(static_cast<void const*>(first.m_next_char_start)) - m_data.data()
        );
        auto const end_byte_offset = static_cast<std::size_t>(
                static_cast<char const*>(static_cast<void const*>(last.m_next_char_start)) - m_data.data()
        );

        m_data.erase(start_byte_offset, end_byte_offset - start_byte_offset);

        if (last == cend()) {
            return cend();
        }

        return ConstIterator{ first.m_next_char_start };
    }

    void Utf8String::reverse() {
        auto new_string = Utf8String{};
        new_string.reserve(m_data.size());
        for (auto const c : *this) {
            new_string = c + new_string;
        }
        *this = std::move(new_string);
    }

    [[nodiscard]] Utf8String Utf8String::to_uppercase() const {
        return transform([](Utf8Char const c) { return c.to_uppercase(); });
    }

    [[nodiscard]] Utf8String Utf8String::to_lowercase() const {
        return transform([](Utf8Char const c) { return c.to_lowercase(); });
    }
} // namespace c2k
