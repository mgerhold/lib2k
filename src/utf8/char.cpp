#include <lib2k/utf8/char.hpp>
#include <utf8proc.h>

namespace c2k {
    [[nodiscard]] static utf8proc_int32_t to_utf8proc_codepoint(StaticVector<std::byte, 4> const codepoint) {
        auto utf8proc_codepoint = utf8proc_int32_t{};
        auto const result = utf8proc_iterate(
                reinterpret_cast<utf8proc_uint8_t const*>(&codepoint.front()),
                static_cast<utf8proc_ssize_t>(codepoint.size()),
                &utf8proc_codepoint
        );
        assert(result >= 0);
        return utf8proc_codepoint;
    }

    [[nodiscard]] static Utf8Char to_utf8char(utf8proc_int32_t const codepoint) {
        auto buffer = std::array<utf8proc_uint8_t, 4>{};
        auto const num_bytes = utf8proc_encode_char(codepoint, buffer.data());
        auto result = StaticVector<std::byte, 4>{};
        for (auto i = decltype(num_bytes){ 0 }; i < num_bytes; ++i) {
            result.push_back(static_cast<std::byte>(buffer.at(i)));
        }
        return Utf8Char::from_bytes_unchecked(std::span{ result.begin(), result.end() });
    }

    Utf8Char::Utf8Char(char const c) {
        auto utf8proc_codepoint = utf8proc_int32_t{};
        auto const result = utf8proc_iterate(
                reinterpret_cast<utf8proc_uint8_t const*>(&c),
                static_cast<utf8proc_ssize_t>(sizeof(c)),
                &utf8proc_codepoint
        );
        if (result < 0) {
            throw InvalidUtf8Char{};
        }
        m_codepoint = Codepoint{ static_cast<std::byte>(c) };
    }

    [[nodiscard]] tl::expected<Utf8Char, Utf8Error> Utf8Char::from_bytes(std::span<std::byte const> const bytes) {
        if (bytes.empty()) {
            return tl::unexpected{ Utf8Error::InvalidBytesRange };
        }
        auto utf8proc_codepoint = utf8proc_int32_t{};
        auto const result = utf8proc_iterate(
                reinterpret_cast<utf8proc_uint8_t const*>(bytes.data()),
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

    [[nodiscard]] tl::expected<Utf8Char, Utf8Error> Utf8Char::from_codepoint(std::int32_t const codepoint) {
        static_assert(std::same_as<std::remove_const_t<decltype(codepoint)>, utf8proc_int32_t>);
        auto const result = to_utf8char(codepoint);
        if (result.m_codepoint.empty()) {
            return tl::unexpected{ Utf8Error::InvalidUtf8Char };
        }
        return result;
    }

    [[nodiscard]] bool Utf8Char::is_uppercase() const {
        return utf8proc_isupper(to_utf8proc_codepoint(m_codepoint)) == 1;
    }

    [[nodiscard]] bool Utf8Char::is_lowercase() const {
        return utf8proc_islower(to_utf8proc_codepoint(m_codepoint)) == 1;
    }

    [[nodiscard]] Utf8Char Utf8Char::to_uppercase() const {
        return to_utf8char(utf8proc_toupper(to_utf8proc_codepoint(m_codepoint)));
    }

    [[nodiscard]] Utf8Char Utf8Char::to_lowercase() const {
        return to_utf8char(utf8proc_tolower(to_utf8proc_codepoint(m_codepoint)));
    }
} // namespace c2k
