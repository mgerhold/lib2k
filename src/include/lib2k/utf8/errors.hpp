#pragma once

#include <stdexcept>

namespace c2k {
    enum class Utf8Error {
        InvalidUtf8String,
        InvalidUtf8Char,
        InvalidBytesRange,
    };

    class InvalidUtf8Char final : public std::runtime_error {
    public:
        InvalidUtf8Char() : std::runtime_error{ "Invalid UTF-8 char" } { }
    };

    class InvalidUtf8String final : public std::runtime_error {
    public:
        InvalidUtf8String() : std::runtime_error{ "Invalid UTF-8 string" } { }
    };
} // namespace c2k
