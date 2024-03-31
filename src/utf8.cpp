#include <cassert>
#include <cstring>
#include <lib2k/utf8.hpp>
#include <utf8proc.h>

namespace c2k {
    [[nodiscard]] tl::expected<Utf8Char, Utf8Error> Utf8Char::from_bytes(std::span<std::byte const> const bytes) {
        if (bytes.empty()) {
            return tl::unexpected{ Utf8Error::InvalidBytesRange };
        }
        auto utf8proc_codepoint = utf8proc_int32_t{};
        auto const result = utf8proc_iterate(
                static_cast<utf8proc_uint8_t const*>(static_cast<void const*>(bytes.data())),
                static_cast<utf8proc_ssize_t>(bytes.size_bytes()),
                &utf8proc_codepoint
        );
        if (result < 0) {
            return tl::unexpected{ Utf8Error::InvalidUtf8Char };
        }
        auto codepoint = Codepoint{};
        for (auto i = decltype(result){ 0 }; i < result; ++i) {
            codepoint.push_back(bytes[i]);
        }
        return Utf8Char{ codepoint };
    }

    [[nodiscard]] Utf8String Utf8Literals::operator""_utf8(char const* const str, std::size_t const length) {
        auto const result = Utf8String::from_chars(std::string{ str, str + length });
        if (not result.has_value()) {
            throw InvalidUtf8String{};
        }
        return result.value();
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
    }

    [[nodiscard]] Utf8Char Utf8String::ConstIterator::operator*() const {
        auto codepoint = Utf8Char::Codepoint{};
        for (auto i = decltype(m_next_char_num_bytes){ 0 }; i < m_next_char_num_bytes; ++i) {
            codepoint.push_back(*(m_next_char_start + i));
        }
        return Utf8Char{ codepoint };
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
        return *this;
    }

    Utf8String::ConstIterator Utf8String::ConstIterator::operator++(int) {
        auto const result = *this;
        ;
        ++*this;
        return result;
    }

    [[nodiscard]] bool Utf8String::ConstIterator::operator==(ConstIterator const& other) const {
        return m_next_char_start == other.m_next_char_start and m_next_char_num_bytes == other.m_next_char_num_bytes;
    }

    [[nodiscard]] tl::expected<Utf8String, Utf8Error> Utf8String::from_chars(std::string chars) {
        auto const result = try_get_utf8_num_chars(chars);
        if (not result.has_value()) {
            return tl::unexpected{ Utf8Error::InvalidUtf8String };
        }
        return Utf8String{ std::move(chars), result.value() };
    }

    [[nodiscard]] tl::expected<std::size_t, Utf8Error> Utf8String::try_get_utf8_num_chars(std::string_view const buffer
    ) {
        auto current = buffer.data();
        auto const end = current + buffer.length();
        auto num_chars = std::size_t{ 0 };
        while (current < end) {
            auto const bytes_remaining = static_cast<utf8proc_ssize_t>(end - current);
            auto codepoint = utf8proc_int32_t{};
            auto const result = utf8proc_iterate(
                    static_cast<utf8proc_uint8_t const*>(static_cast<void const*>(current)),
                    bytes_remaining,
                    &codepoint
            );
            if (result < 0) {
                return tl::unexpected{ Utf8Error::InvalidUtf8String };
            }
            current += result;
            ++num_chars;
        }
        return num_chars;
    }

    [[nodiscard]] std::size_t Utf8String::char_width() const {
        auto width = std::size_t{ 0 };
        auto current = reinterpret_cast<utf8proc_uint8_t const*>(m_data.data());

        while (current < reinterpret_cast<utf8proc_uint8_t const*>(m_data.data() + m_data.length())) {
            auto codepoint = utf8proc_int32_t{};
            auto const bytes_read = utf8proc_iterate(current, -1, &codepoint);
            // clang-format off
            assert(
                codepoint != -1 and utf8proc_codepoint_valid(codepoint)
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
} // namespace c2k
