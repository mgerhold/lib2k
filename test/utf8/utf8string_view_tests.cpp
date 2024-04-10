#include <gtest/gtest.h>
#include <lib2k/utf8.hpp>

using c2k::Utf8StringView;
using namespace c2k::Utf8Literals;

TEST(Utf8StringViewTests, Construction) {
    static constexpr auto text = "Hello, 🌍!";
    EXPECT_NO_THROW(std::ignore = Utf8StringView{ text });
    EXPECT_NO_THROW(std::ignore = Utf8StringView{ std::string_view{ text } });
    auto const string = "Hello, 🌍!"_utf8;
    EXPECT_NO_THROW(std::ignore = Utf8StringView{ string });
    EXPECT_NO_THROW((std::ignore = Utf8StringView{ string.find('H'), string.find(',') }));

    EXPECT_THROW(std::ignore = Utf8StringView{ "Hello, \xff!" }, c2k::InvalidUtf8String);
    EXPECT_THROW(std::ignore = Utf8StringView{ "\x80" }, c2k::InvalidUtf8String);
    EXPECT_THROW(std::ignore = Utf8StringView{ "\xE0\xA0" }, c2k::InvalidUtf8String);
    EXPECT_THROW(std::ignore = Utf8StringView{ "\xF4\x90\x80\x80" }, c2k::InvalidUtf8String);
    EXPECT_THROW(std::ignore = Utf8StringView{ "\xED\xA0\x80" }, c2k::InvalidUtf8String);
    EXPECT_THROW(std::ignore = Utf8StringView{ "\xE0\xA0\x41" }, c2k::InvalidUtf8String);
}

TEST(Utf8StringViewTests, UserDefinedLiteral) {
    EXPECT_NO_THROW(std::ignore = "Hello, 🌍!"_utf8view);
    EXPECT_NO_THROW(std::ignore = ""_utf8view);
    EXPECT_NO_THROW(std::ignore = "test"_utf8view);
    EXPECT_THROW(std::ignore = "Hello, \xff!"_utf8view, c2k::InvalidUtf8String);
    EXPECT_THROW(std::ignore = "\x80"_utf8view, c2k::InvalidUtf8String);
    EXPECT_THROW(std::ignore = "\xE0\xA0"_utf8view, c2k::InvalidUtf8String);
    EXPECT_THROW(std::ignore = "\xF4\x90\x80\x80"_utf8view, c2k::InvalidUtf8String);
    EXPECT_THROW(std::ignore = "\xED\xA0\x80"_utf8view, c2k::InvalidUtf8String);
    EXPECT_THROW(std::ignore = "\xE0\xA0\x41"_utf8view, c2k::InvalidUtf8String);
}

// todo: check what happens if you try to iterate over empty strings or string_views
// todo: also check iterators for non null-terminated strings (when using views)
