#include <gtest/gtest.h>
#include <lib2k/utf8/char.hpp>
#include <lib2k/utf8/literals.hpp>
#include <lib2k/utf8/string.hpp>

using c2k::Utf8Char;
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

TEST(Utf8CharTests, IsUppercase) {
    EXPECT_TRUE('A'_utf8.is_uppercase());
    EXPECT_TRUE("Ä"_utf8.cbegin()->is_uppercase());
    EXPECT_TRUE("Φ"_utf8.cbegin()->is_uppercase());
    EXPECT_FALSE('a'_utf8.is_uppercase());
    EXPECT_FALSE("ä"_utf8.cbegin()->is_uppercase());
    EXPECT_FALSE("🌍"_utf8.cbegin()->is_uppercase());
}

TEST(Utf8CharTests, IsLowercase) {
    EXPECT_TRUE('a'_utf8.is_lowercase());
    EXPECT_TRUE("ä"_utf8.cbegin()->is_lowercase());
    EXPECT_TRUE("ѭ"_utf8.cbegin()->is_lowercase());
    EXPECT_FALSE('A'_utf8.is_lowercase());
    EXPECT_FALSE("Ä"_utf8.cbegin()->is_lowercase());
    EXPECT_FALSE("🌍"_utf8.cbegin()->is_lowercase());
}

TEST(Utf8CharTests, ToUppercase) {
    EXPECT_EQ("a"_utf8.cbegin()->to_uppercase(), *("A"_utf8.cbegin()));
    EXPECT_EQ("ä"_utf8.cbegin()->to_uppercase(), *("Ä"_utf8.cbegin()));
    EXPECT_EQ("φ"_utf8.cbegin()->to_uppercase(), *("Φ"_utf8.cbegin()));
    EXPECT_EQ("🌍"_utf8.cbegin()->to_uppercase(), *("🌍"_utf8.cbegin()));
}

TEST(Utf8CharTests, ToLowercase) {
    EXPECT_EQ("A"_utf8.cbegin()->to_lowercase(), *("a"_utf8.cbegin()));
    EXPECT_EQ("Ä"_utf8.cbegin()->to_lowercase(), *("ä"_utf8.cbegin()));
    EXPECT_EQ("Φ"_utf8.cbegin()->to_lowercase(), *("φ"_utf8.cbegin()));
    EXPECT_EQ("🌍"_utf8.cbegin()->to_lowercase(), *("🌍"_utf8.cbegin()));
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
