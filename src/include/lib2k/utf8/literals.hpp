#pragma once

#include <cstdlib>

namespace c2k {
    class Utf8Char;
    class Utf8String;
    class Utf8StringView;

    namespace Utf8Literals {
        [[nodiscard]] Utf8Char operator""_utf8(char c);

        [[nodiscard]] Utf8String operator""_utf8(char const* str, std::size_t length);

        [[nodiscard]] Utf8StringView operator""_utf8view(char const* str, std::size_t length);
    } // namespace Utf8Literals
} // namespace c2k
