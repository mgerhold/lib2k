#include <lib2k/utf8/literals.hpp>
#include <lib2k/utf8/char.hpp>
#include <lib2k/utf8/string.hpp>
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
}