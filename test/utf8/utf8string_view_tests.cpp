#include <algorithm>
#include <gtest/gtest.h>
#include <lib2k/utf8.hpp>
#include <vector>

using c2k::Utf8Char;
using c2k::Utf8String;
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

TEST(Utf8StringViewTests, NumChars) {
    EXPECT_EQ("abc"_utf8view.calculate_char_count(), 3);
    EXPECT_EQ("a🦀c"_utf8view.calculate_char_count(), 3);
    EXPECT_EQ(""_utf8view.calculate_char_count(), 0);
    EXPECT_EQ("🦀🌍😊"_utf8view.calculate_char_count(), 3);
    EXPECT_EQ("Hello, 🌍!"_utf8view.calculate_char_count(), 9);
    EXPECT_EQ("C++ Programming 🚀"_utf8view.calculate_char_count(), 17);
}

TEST(Utf8StringViewTests, CharWidth) {
    EXPECT_EQ("abc"_utf8view.calculate_char_width(), 3);
    EXPECT_EQ(""_utf8view.calculate_char_width(), 0);
    EXPECT_EQ("a🦀c"_utf8view.calculate_char_width(), 4);
    EXPECT_EQ("🦀🌍😊"_utf8view.calculate_char_width(), 6);
    EXPECT_EQ("C++ Programming 🚀"_utf8view.calculate_char_width(), 18);
}

TEST(Utf8StringViewTests, Iterating) {
    auto const utf8_string_view = "Hello, 🌍!"_utf8view;
    auto iterator = utf8_string_view.begin();
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
    EXPECT_EQ(utf8_string_view.calculate_char_count(), 9);

    auto stream = std::ostringstream{};
    for (auto const c : utf8_string_view) {
        stream << c << '\n';
    }
    EXPECT_EQ(stream.str(), "H\ne\nl\nl\no\n,\n \n🌍\n!\n");

    auto const empty = ""_utf8view;
    auto counter = std::size_t{ 0 };
    for ([[maybe_unused]] auto const& c : empty) {
        ++counter;
    }
    EXPECT_EQ(counter, 0);
}

TEST(Utf8StringViewTests, IteratingBackwards) {
    auto const utf8_string_view = "Hello, 🌍!"_utf8view;
    auto iterator = utf8_string_view.end();
    --iterator;
    EXPECT_EQ(*iterator, '!'_utf8);
    --iterator;
    EXPECT_EQ(
            *iterator,
            Utf8Char::from_bytes(std::array{
                    std::byte{ static_cast<unsigned char>('\xf0') },
                    std::byte{ static_cast<unsigned char>('\x9f') },
                    std::byte{ static_cast<unsigned char>('\x8c') },
                    std::byte{ static_cast<unsigned char>('\x8d') },
            }) // == 🌍
    );
    --iterator;
    EXPECT_EQ(*iterator, ' '_utf8);
    --iterator;
    EXPECT_EQ(*iterator, ','_utf8);
    --iterator;
    EXPECT_EQ(*iterator, 'o'_utf8);
    --iterator;
    EXPECT_EQ(*iterator, 'l'_utf8);
    --iterator;
    EXPECT_EQ(*iterator, 'l'_utf8);
    --iterator;
    EXPECT_EQ(*iterator, 'e'_utf8);
    --iterator;
    EXPECT_EQ(*iterator, 'H'_utf8);
    EXPECT_EQ(iterator, utf8_string_view.cbegin());

    auto reverse_iterator = utf8_string_view.crbegin();
    EXPECT_EQ(*reverse_iterator, *(--utf8_string_view.end()));
    EXPECT_NE(reverse_iterator, utf8_string_view.crend());
    ++reverse_iterator;
    EXPECT_EQ(
            *reverse_iterator,
            Utf8Char::from_bytes(std::array{
                    std::byte{ static_cast<unsigned char>('\xf0') },
                    std::byte{ static_cast<unsigned char>('\x9f') },
                    std::byte{ static_cast<unsigned char>('\x8c') },
                    std::byte{ static_cast<unsigned char>('\x8d') },
            }) // == 🌍
    );
    EXPECT_NE(reverse_iterator, utf8_string_view.crend());
    ++reverse_iterator;
    EXPECT_EQ(*reverse_iterator, ' '_utf8);
    EXPECT_NE(reverse_iterator, utf8_string_view.crend());
    ++reverse_iterator;
    EXPECT_EQ(*reverse_iterator, ','_utf8);
    EXPECT_NE(reverse_iterator, utf8_string_view.crend());
    ++reverse_iterator;
    EXPECT_EQ(*reverse_iterator, 'o'_utf8);
    EXPECT_NE(reverse_iterator, utf8_string_view.crend());
    ++reverse_iterator;
    EXPECT_EQ(*reverse_iterator, 'l'_utf8);
    EXPECT_NE(reverse_iterator, utf8_string_view.crend());
    ++reverse_iterator;
    EXPECT_EQ(*reverse_iterator, 'l'_utf8);
    EXPECT_NE(reverse_iterator, utf8_string_view.crend());
    ++reverse_iterator;
    EXPECT_EQ(*reverse_iterator, 'e'_utf8);
    EXPECT_NE(reverse_iterator, utf8_string_view.crend());
    ++reverse_iterator;
    EXPECT_EQ(*reverse_iterator, 'H'_utf8);
    EXPECT_NE(reverse_iterator, utf8_string_view.crend());
    ++reverse_iterator;
    EXPECT_EQ(reverse_iterator, utf8_string_view.crend());

    auto reversed = Utf8String{};
    for (auto it = utf8_string_view.rbegin(); it != utf8_string_view.rend(); ++it) {
        reversed += *it;
    }
    EXPECT_EQ(reversed, "!🌍 ,olleH");

    auto back_and_forth = Utf8String{};
    auto it = utf8_string_view.crbegin();
    for (; it != utf8_string_view.crend(); ++it) {
        back_and_forth += *it;
    }
    --it;
    while (true) {
        back_and_forth += *it;
        if (it == utf8_string_view.crbegin()) {
            break;
        }
        --it;
    }
    EXPECT_EQ(back_and_forth, "!🌍 ,olleHHello, 🌍!");
}

TEST(Utf8StringViewTests, IsEmpty) {
    EXPECT_TRUE(""_utf8view.is_empty());
    EXPECT_FALSE("!"_utf8view.is_empty());
}

TEST(Utf8StringViewTests, Substring) {
    auto const string = "The quick brown 🦊 jumps over the lazy 🐶."_utf8view;
    auto sub = string.substring(
            std::find(string.cbegin(), string.cend(), *"🦊"_utf8view.begin()),
            std::find(string.cbegin(), string.cend(), *"🐶"_utf8view.begin())
    );
    EXPECT_EQ(sub, "🦊 jumps over the lazy "_utf8view);

    auto reconstructed = Utf8String{};
    for (auto const& c : sub) {
        reconstructed += c;
    }
    EXPECT_EQ(reconstructed, "🦊 jumps over the lazy ");

    sub = string.substring(std::find(string.cbegin(), string.cend(), *"🐶"_utf8view.begin()));
    EXPECT_EQ(sub, "🐶."_utf8view);

    sub = string.substring(string.cbegin() + 4, 5);
    EXPECT_EQ(sub, "quick");

    sub = string.substring(10, 7);
    EXPECT_EQ(sub, "brown 🦊"_utf8view);

    sub = string.substring(10);
    EXPECT_EQ(sub, "brown 🦊 jumps over the lazy 🐶."_utf8view);
}

TEST(Utf8StringViewTests, FrontAndBack) {
    auto string = ""_utf8view;
    EXPECT_THROW(std::ignore = string.front(), std::out_of_range);
    EXPECT_THROW(std::ignore = string.back(), std::out_of_range);
    string = "!";
    EXPECT_EQ(string.front(), '!');
    EXPECT_EQ(string.back(), '!');
    string = "C++ 🐀";
    EXPECT_EQ(string.front(), 'C');
    EXPECT_EQ(string.back(), *"🐀"_utf8view.cbegin());
}

TEST(Utf8StringViewTests, NonNullTerminatedStringView) {
    auto const string = "The quick brown 🦊 jumps over the lazy 🐶."_utf8;
    auto const sub_view = Utf8StringView{ string }.substring(10, 7);
    EXPECT_EQ(sub_view, "brown 🦊"_utf8view);
    auto count = std::size_t{ 0 };
    for ([[maybe_unused]] auto const& c : sub_view) {
        ++count;
    }
    EXPECT_EQ(count, 7);
    EXPECT_NE(*sub_view.cend(), '\0'); // this string view is not null-terminated
}

TEST(Utf8StringViewTests, Join) {
    using namespace std::string_literals;
    EXPECT_EQ(", "_utf8view.join(std::vector{ "this", "is", "a", "test" }), "this, is, a, test");

    char const* words[] = { "this", "is", "a", "test" };
    EXPECT_EQ(", "_utf8view.join(words), "this, is, a, test");

    EXPECT_EQ(", "_utf8view.join(std::vector<std::string>{}), "");

    EXPECT_EQ(", "_utf8view.join(std::vector{ "single" }), "single");

    EXPECT_EQ(", "_utf8view.join(std::vector{ "1", "2", "3", "4" }), "1, 2, 3, 4");

    char const* nullStrings[] = { nullptr, nullptr };
    EXPECT_THROW({ std::ignore = ", "_utf8view.join(nullStrings); }, std::invalid_argument);
}
