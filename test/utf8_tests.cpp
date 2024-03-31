#include <gtest/gtest.h>
#include <lib2k/utf8.hpp>
#include <sstream>

using c2k::Utf8Char;
using c2k::Utf8Error;
using c2k::Utf8String;
using namespace c2k::Utf8Literals;

TEST(Utf8CharTests, Construction) {
    EXPECT_NO_THROW(std::ignore = Utf8Char{ 'a' });
    EXPECT_NO_THROW(std::ignore = Utf8Char{ ' ' });
    EXPECT_NO_THROW(std::ignore = Utf8Char{ '\0' });
    EXPECT_NO_THROW(std::ignore = Utf8Char{ '"' });
    EXPECT_NO_THROW(std::ignore = Utf8Char{ '\n' });
}

TEST(Utf8CharTests, UserDefinedLiteral) {
    EXPECT_EQ(Utf8Char{ 'a' }, 'a'_utf8);
    EXPECT_EQ(Utf8Char{ ' ' }, ' '_utf8);
    EXPECT_EQ(Utf8Char{ '\0' }, '\0'_utf8);
    EXPECT_EQ(Utf8Char{ '"' }, '"'_utf8);
    EXPECT_EQ(Utf8Char{ '\n' }, '\n'_utf8);
}

TEST(Utf8CharTests, FromBytes) {
    // clang-format off
    EXPECT_NO_THROW(
        std::ignore = Utf8Char::from_bytes(std::array{
            std::byte{static_cast<unsigned char>('\xF0')},
            std::byte{static_cast<unsigned char>('\x9F')},
            std::byte{static_cast<unsigned char>('\xA6')},
            std::byte{static_cast<unsigned char>('\x80')},
        }).value()
    );
    // clang-format on
}

TEST(Utf8CharTests, Printing) {
    auto stream = std::ostringstream{};
    stream << Utf8Char{ 'a' };
    EXPECT_EQ(stream.str(), "a");

    stream = {};
    stream << Utf8Char{ '\n' };
    EXPECT_EQ(stream.str(), "\n");

    stream = {};
    stream << Utf8Char::from_bytes(std::array{
                                           std::byte{ static_cast<unsigned char>('\xF0') },
                                           std::byte{ static_cast<unsigned char>('\x9F') },
                                           std::byte{ static_cast<unsigned char>('\xA6') },
                                           std::byte{ static_cast<unsigned char>('\x80') },
                                   })
                      .value();
    EXPECT_EQ(stream.str(), "🦀");
}

TEST(Utf8StringTests, TryGetUtf8NumChars) {
    EXPECT_EQ(Utf8String::try_get_utf8_num_chars("abc").value(), 3);
    EXPECT_EQ(Utf8String::try_get_utf8_num_chars("Hello, 🌍!").value(), 9);
    auto const result = Utf8String::try_get_utf8_num_chars("Hello, \xff!");
    EXPECT_EQ(result.error(), Utf8Error::InvalidUtf8String);
    EXPECT_EQ(Utf8String::try_get_utf8_num_chars("").value(), 0);
    EXPECT_EQ(Utf8String::try_get_utf8_num_chars("Hey, Ferris! 🦀").value(), 14);
}

TEST(Utf8StringTests, Construction) {
    EXPECT_TRUE(Utf8String::from_chars("").has_value());
    EXPECT_TRUE(Utf8String::from_chars("abc").has_value());
    EXPECT_TRUE(Utf8String::from_chars("Hello, 🌍!").has_value());
    EXPECT_EQ(Utf8String::from_chars("Hello, \xff!").error(), Utf8Error::InvalidUtf8String);
}

TEST(Utf8StringTests, CString) {
    EXPECT_STREQ(Utf8String::from_chars("").value().c_str(), "");
    EXPECT_STREQ(Utf8String::from_chars("abc").value().c_str(), "abc");
    EXPECT_STREQ(Utf8String::from_chars("Hello, 🌍!").value().c_str(), "Hello, 🌍!");
}

TEST(Utf8StringTests, UserDefinedLiteral) {
    EXPECT_EQ(Utf8String::from_chars("abc").value(), "abc"_utf8);
    EXPECT_EQ(Utf8String::from_chars("").value(), ""_utf8);
    EXPECT_EQ(Utf8String::from_chars("Hello, 🌍!").value(), "Hello, 🌍!"_utf8);
    EXPECT_THROW(std::ignore = "Hello, \xff!"_utf8, c2k::InvalidUtf8String);
    EXPECT_THROW(std::ignore = "\x80"_utf8, c2k::InvalidUtf8String);
    EXPECT_THROW(std::ignore = "\xE0\xA0"_utf8, c2k::InvalidUtf8String);
    EXPECT_THROW(std::ignore = "\xF4\x90\x80\x80"_utf8, c2k::InvalidUtf8String);
    EXPECT_THROW(std::ignore = "\xED\xA0\x80"_utf8, c2k::InvalidUtf8String);
    EXPECT_THROW(std::ignore = "\xE0\xA0\x41"_utf8, c2k::InvalidUtf8String);
}

TEST(Utf8StringTests, NumChars) {
    EXPECT_EQ(Utf8String::from_chars("abc").value().num_chars(), 3);
    EXPECT_EQ(Utf8String::from_chars("a🦀c").value().num_chars(), 3);
    EXPECT_EQ(Utf8String::from_chars("").value().num_chars(), 0);
    EXPECT_EQ(Utf8String::from_chars("🦀🌍😊").value().num_chars(), 3);
    EXPECT_EQ(Utf8String::from_chars("Hello, 🌍!").value().num_chars(), 9);
    EXPECT_EQ(Utf8String::from_chars("C++ Programming 🚀").value().num_chars(), 17);
}

TEST(Utf8StringTests, CharWidth) {
    EXPECT_EQ(Utf8String::from_chars("abc").value().char_width(), 3);
    EXPECT_EQ(Utf8String::from_chars("").value().char_width(), 0);
    EXPECT_EQ(Utf8String::from_chars("a🦀c").value().char_width(), 4);
    EXPECT_EQ(Utf8String::from_chars("🦀🌍😊").value().char_width(), 6);
    EXPECT_EQ(Utf8String::from_chars("C++ Programming 🚀").value().num_chars(), 17);
}

TEST(Utf8StringTests, Iterating) {
    auto const utf8_string = "Hello, 🌍!"_utf8;
    auto iterator = utf8_string.begin();
    EXPECT_EQ(*iterator, 'H'_utf8);
    ++iterator;
    EXPECT_EQ(*iterator, 'e'_utf8);
    ++iterator;
    EXPECT_EQ(*iterator, 'l'_utf8);
    ++iterator;
    EXPECT_EQ(*iterator, 'l'_utf8);
    ++iterator;
    EXPECT_EQ(*iterator, 'o'_utf8);
    ++iterator;
    EXPECT_EQ(*iterator, ','_utf8);
    ++iterator;
    EXPECT_EQ(*iterator, ' '_utf8);
    ++iterator;
    EXPECT_EQ(
            *iterator,
            Utf8Char::from_bytes(std::array{
                    std::byte{ static_cast<unsigned char>('\xf0') },
                    std::byte{ static_cast<unsigned char>('\x9f') },
                    std::byte{ static_cast<unsigned char>('\x8c') },
                    std::byte{ static_cast<unsigned char>('\x8d') },
            }) // == 🌍
    );
    ++iterator;
    EXPECT_EQ(*iterator, '!'_utf8);
    EXPECT_EQ(utf8_string.num_chars(), 9);

    auto stream = std::ostringstream{};
    for (auto const c : utf8_string) {
        stream << c << '\n';
    }
    EXPECT_EQ(stream.str(), "H\ne\nl\nl\no\n,\n \n🌍\n!\n");
}

TEST(Utf8StringTests, OutputOperator) {
    auto stream = std::ostringstream{};
    stream << "Hello, 🌍!"_utf8;
    EXPECT_EQ(stream.str(), "Hello, 🌍!");
}
