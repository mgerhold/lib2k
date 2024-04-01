#include <algorithm>
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

TEST(Utf8StringTests, ValidateUtf8) {
    EXPECT_TRUE(Utf8String::is_valid_utf8("abc"));
    EXPECT_TRUE(Utf8String::is_valid_utf8("Hello, 🌍!"));
    EXPECT_FALSE(Utf8String::is_valid_utf8("Hello, \xff!"));
    EXPECT_TRUE(Utf8String::is_valid_utf8(""));
    EXPECT_TRUE(Utf8String::is_valid_utf8("Hey, Ferris! 🦀"));
}

TEST(Utf8StringTests, Construction) {
    EXPECT_TRUE(Utf8String::from_chars("").has_value());
    EXPECT_TRUE(Utf8String::from_chars("abc").has_value());
    EXPECT_TRUE(Utf8String::from_chars("Hello, 🌍!").has_value());
    EXPECT_EQ(Utf8String::from_chars("Hello, \xff!").error(), Utf8Error::InvalidUtf8String);
    EXPECT_TRUE(Utf8String::from_chars("").has_value());
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
    EXPECT_EQ(Utf8String::from_chars("").value(), ""_utf8);
}

TEST(Utf8StringTests, NumChars) {
    EXPECT_EQ(Utf8String::from_chars("abc").value().calculate_char_count(), 3);
    EXPECT_EQ(Utf8String::from_chars("a🦀c").value().calculate_char_count(), 3);
    EXPECT_EQ(Utf8String::from_chars("").value().calculate_char_count(), 0);
    EXPECT_EQ(Utf8String::from_chars("🦀🌍😊").value().calculate_char_count(), 3);
    EXPECT_EQ(Utf8String::from_chars("Hello, 🌍!").value().calculate_char_count(), 9);
    EXPECT_EQ(Utf8String::from_chars("C++ Programming 🚀").value().calculate_char_count(), 17);
}

TEST(Utf8StringTests, CharWidth) {
    EXPECT_EQ(Utf8String::from_chars("abc").value().calculate_char_width(), 3);
    EXPECT_EQ(Utf8String::from_chars("").value().calculate_char_width(), 0);
    EXPECT_EQ(Utf8String::from_chars("a🦀c").value().calculate_char_width(), 4);
    EXPECT_EQ(Utf8String::from_chars("🦀🌍😊").value().calculate_char_width(), 6);
    EXPECT_EQ(Utf8String::from_chars("C++ Programming 🚀").value().calculate_char_width(), 18);
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
    EXPECT_EQ(utf8_string.calculate_char_count(), 9);

    auto stream = std::ostringstream{};
    for (auto const c : utf8_string) {
        stream << c << '\n';
    }
    EXPECT_EQ(stream.str(), "H\ne\nl\nl\no\n,\n \n🌍\n!\n");
}

TEST(Utf8StringTests, Append) {
    auto string = "Hello, 🌍"_utf8;
    string.append('!');
    EXPECT_EQ(string, "Hello, 🌍!");
    EXPECT_EQ(string.calculate_char_count(), 9);

    string = "Hello";
    string.append(", 🌍!");
    EXPECT_EQ(string, "Hello, 🌍!");
    EXPECT_EQ(string.calculate_char_count(), 9);

    string = "Hello, 🌍";
    string += '!';
    EXPECT_EQ(string, "Hello, 🌍!");
    EXPECT_EQ(string.calculate_char_count(), 9);

    string = "Hello";
    string += ", 🌍!";
    EXPECT_EQ(string, "Hello, 🌍!");
    EXPECT_EQ(string.calculate_char_count(), 9);

    string += "";
    EXPECT_EQ(string, "Hello, 🌍!");
    EXPECT_EQ(string.calculate_char_count(), 9);

    auto result = Utf8String{};
    for (auto const c : string) {
        (result += c) += '\n';
    }
    EXPECT_EQ(result, "H\ne\nl\nl\no\n,\n \n🌍\n!\n");
    EXPECT_EQ(result.calculate_char_count(), 18);
}

TEST(Utf8StringTests, IsEmpty) {
    EXPECT_TRUE(""_utf8.is_empty());
    EXPECT_FALSE("!"_utf8.is_empty());
}

TEST(Utf8StringTests, Clear) {
    auto string = "test"_utf8;
    string.clear();
    EXPECT_EQ(string.calculate_char_count(), 0);
    EXPECT_EQ(string.calculate_char_width(), 0);
    EXPECT_TRUE(string.is_empty());
}

TEST(Utf8StringTests, Find) {
    auto const world = *("🌍"_utf8.cbegin());
    auto const str = "test🌍"_utf8;
    EXPECT_EQ(str.find('t'), str.cbegin());
    EXPECT_EQ(str.find('e'), str.cbegin() + 1);
    EXPECT_EQ(str.find('s'), str.cbegin() + 2);
    EXPECT_EQ(str.find(world), str.cbegin() + 4);
    EXPECT_EQ(str.find('x'), str.cend());

    EXPECT_EQ(str.find("test🌍"), str.cbegin());
    EXPECT_EQ(str.find("test🌍🦀"), str.cend());
    EXPECT_EQ(str.find(""), str.cbegin());
    EXPECT_EQ(str.find("est🌍"), str.cbegin() + 1);
    EXPECT_EQ(str.find("est"), str.cbegin() + 1);
}

TEST(Utf8StringTests, FindWithDifferentPosition) {
    auto const world = *("🌍"_utf8.cbegin());
    auto const str = "test🌍test🌍"_utf8;
    EXPECT_EQ(str.find('t'), str.cbegin());
    EXPECT_EQ(str.find('e', 2), str.cbegin() + 6);
    EXPECT_EQ(str.find('s', 3), str.cbegin() + 7);
    EXPECT_EQ(str.find(world, 5), str.cbegin() + 9);
    EXPECT_EQ(str.find('x'), str.cend());

    EXPECT_EQ(str.find("test🌍"), str.cbegin());
    EXPECT_EQ(str.find("test🌍", 5), str.cbegin() + 5);
    EXPECT_EQ(str.find("test🌍🦀"), str.cend());
    EXPECT_EQ(str.find("est🌍", 2), str.cbegin() + 6);
    EXPECT_EQ(str.find("est", 3), str.cbegin() + 6);

    EXPECT_EQ(str.find('t'), str.cbegin());
    EXPECT_EQ(str.find('e', str.cbegin() + 2), str.cbegin() + 6);
    EXPECT_EQ(str.find('s', str.cbegin() + 3), str.cbegin() + 7);
    EXPECT_EQ(str.find(world, str.cbegin() + 5), str.cbegin() + 9);
    EXPECT_EQ(str.find('x'), str.cend());

    EXPECT_EQ(str.find("test🌍"), str.cbegin());
    EXPECT_EQ(str.find("test🌍", str.cbegin() + 5), str.cbegin() + 5);
    EXPECT_EQ(str.find("test🌍🦀"), str.cend());
    EXPECT_EQ(str.find("est🌍", str.cbegin() + 2), str.cbegin() + 6);
    EXPECT_EQ(str.find("est", str.cbegin() + 3), str.cbegin() + 6);
}

TEST(Utf8StringTests, LargerStringFind) {
    auto const str = "Hello, world! This is a test string used for testing utf8 string search functions."_utf8;
    EXPECT_EQ(str.find('T'), str.cbegin() + 14);
    EXPECT_EQ(str.find("test"), str.cbegin() + 24);
    EXPECT_EQ(str.find("testing utf8"), str.cbegin() + 45);
    EXPECT_EQ(str.find('x'), str.cend());
    EXPECT_EQ(str.find("nonexistent"), str.cend());
}

TEST(Utf8StringTests, EmojisTest) {
    auto const sun = *("☀"_utf8.cbegin());
    auto const water = *("💧"_utf8.cbegin());
    auto const tree = *("🌴"_utf8.cbegin());
    auto const str = "☀💧🌴"_utf8;
    EXPECT_EQ(str.find(sun), str.cbegin());
    EXPECT_EQ(str.find(water), str.cbegin() + 1);
    EXPECT_EQ(str.find(tree), str.cbegin() + 2);
    EXPECT_EQ(str.find('x'), str.cend());
}

TEST(Utf8StringTests, Erase) {
    auto str = "This is a little test string, hello 🌍! And hello, 🦀!"_utf8;
    auto const original_length = str.calculate_char_count();
    auto it = str.find("little ");
    auto result = str.erase(it, it + 7);
    EXPECT_EQ(str, "This is a test string, hello 🌍! And hello, 🦀!");
    EXPECT_EQ(str.calculate_char_count(), original_length - 7);
    EXPECT_EQ(*result, 't');

    EXPECT_EQ(*str.erase(str.find('a')), ' ');
    EXPECT_EQ(str, "This is  test string, hello 🌍! And hello, 🦀!");
    EXPECT_EQ(str.calculate_char_count(), original_length - 8);

    it = str.find("🦀!");
    result = str.erase(it, it + 2);
    EXPECT_EQ(result, str.cend());
    EXPECT_EQ(str, "This is  test string, hello 🌍! And hello, ");
    EXPECT_EQ(str.calculate_char_count(), original_length - 10);

    result = str.erase(str.cbegin(), str.cend());
    EXPECT_EQ(result, str.cend());
    EXPECT_TRUE(str.is_empty());
    EXPECT_EQ(str.calculate_char_count(), 0);
}

TEST(Utf8StringTests, Reverse) {
    auto str = "Hello, 🌍!"_utf8;
    str.reverse();
    EXPECT_EQ(str, "!🌍 ,olleH");
    EXPECT_EQ(str.calculate_char_count(), 9);

    str.clear();
    str.reverse();
    EXPECT_TRUE(str.is_empty());
}

TEST(Utf8StringTests, ToUppercase) {
    EXPECT_EQ("Hello, 🌍!"_utf8.to_uppercase(), "HELLO, 🌍!");
    EXPECT_EQ("🦀🥵🌍"_utf8.to_uppercase(), "🦀🥵🌍");
    EXPECT_EQ(""_utf8.to_uppercase(), "");
    EXPECT_EQ("The symbol φ is often used in math."_utf8.to_uppercase(), "THE SYMBOL Φ IS OFTEN USED IN MATH.");
}

TEST(Utf8StringTests, ToLowercase) {
    EXPECT_EQ("Hello, 🌍!"_utf8.to_lowercase(), "hello, 🌍!");
    EXPECT_EQ("🦀🥵🌍"_utf8.to_lowercase(), "🦀🥵🌍");
    EXPECT_EQ(""_utf8.to_lowercase(), "");
    EXPECT_EQ("THE SYMBOL Φ IS OFTEN USED IN MATH."_utf8.to_lowercase(), "the symbol φ is often used in math.");
}

TEST(Utf8StringTests, OutputOperator) {
    auto stream = std::ostringstream{};
    stream << "Hello, 🌍!"_utf8;
    EXPECT_EQ(stream.str(), "Hello, 🌍!");
}
