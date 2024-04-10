#include <lib2k/utf8/char.hpp>
#include <lib2k/utf8/literals.hpp>
#include <lib2k/utf8/string.hpp>
#include <lib2k/utf8/string_view.hpp>
#include <string>

namespace c2k {
    [[nodiscard]] Utf8Char Utf8Literals::operator""_utf8(char const c) {
        return Utf8Char{ c };
    }

    [[nodiscard]] Utf8String Utf8Literals::operator""_utf8(char const* const str, std::size_t const length) {
        return Utf8String{
            std::string{ str, length }
        };
    }

    [[nodiscard]] Utf8StringView Utf8Literals::operator""_utf8view(char const* const str, std::size_t const length) {
        auto const view = std::string_view{ str, length };
        if (not Utf8String::is_valid_utf8(view)) {
            throw InvalidUtf8String{};
        }
        return Utf8StringView::from_string_view_unchecked(view);
    }
} // namespace c2k
