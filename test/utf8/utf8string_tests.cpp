#include <algorithm>
#include <gtest/gtest.h>
#include <lib2k/utf8.hpp>

using c2k::Utf8Char;
using c2k::Utf8Error;
using c2k::Utf8String;
using namespace c2k::Utf8Literals;

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

    auto const empty = ""_utf8;
    auto counter = std::size_t{ 0 };
    for ([[maybe_unused]] auto const& c : empty) {
        ++counter;
    }
    EXPECT_EQ(counter, 0);
}

TEST(Utf8StringTests, IteratingBackwards) {
    auto const utf8_string = "Hello, 🌍!"_utf8;
    auto iterator = utf8_string.end();
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
    EXPECT_EQ(iterator, utf8_string.cbegin());

    auto reverse_iterator = utf8_string.crbegin();
    EXPECT_EQ(*reverse_iterator, *(--utf8_string.end()));
    EXPECT_NE(reverse_iterator, utf8_string.crend());
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
    EXPECT_NE(reverse_iterator, utf8_string.crend());
    ++reverse_iterator;
    EXPECT_EQ(*reverse_iterator, ' '_utf8);
    EXPECT_NE(reverse_iterator, utf8_string.crend());
    ++reverse_iterator;
    EXPECT_EQ(*reverse_iterator, ','_utf8);
    EXPECT_NE(reverse_iterator, utf8_string.crend());
    ++reverse_iterator;
    EXPECT_EQ(*reverse_iterator, 'o'_utf8);
    EXPECT_NE(reverse_iterator, utf8_string.crend());
    ++reverse_iterator;
    EXPECT_EQ(*reverse_iterator, 'l'_utf8);
    EXPECT_NE(reverse_iterator, utf8_string.crend());
    ++reverse_iterator;
    EXPECT_EQ(*reverse_iterator, 'l'_utf8);
    EXPECT_NE(reverse_iterator, utf8_string.crend());
    ++reverse_iterator;
    EXPECT_EQ(*reverse_iterator, 'e'_utf8);
    EXPECT_NE(reverse_iterator, utf8_string.crend());
    ++reverse_iterator;
    EXPECT_EQ(*reverse_iterator, 'H'_utf8);
    EXPECT_NE(reverse_iterator, utf8_string.crend());
    ++reverse_iterator;
    EXPECT_EQ(reverse_iterator, utf8_string.crend());

    auto reversed = Utf8String{};
    for (auto it = utf8_string.rbegin(); it != utf8_string.rend(); ++it) {
        reversed += *it;
    }
    EXPECT_EQ(reversed, "!🌍 ,olleH");

    auto back_and_forth = Utf8String{};
    auto it = utf8_string.crbegin();
    for (; it != utf8_string.crend(); ++it) {
        back_and_forth += *it;
    }
    --it;
    while (true) {
        back_and_forth += *it;
        if (it == utf8_string.crbegin()) {
            break;
        }
        --it;
    }
    EXPECT_EQ(back_and_forth, "!🌍 ,olleHHello, 🌍!");
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

TEST(Utf8StringTests, Substring) {
    auto const string = "The quick brown 🦊 jumps over the lazy 🐶."_utf8;
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

TEST(Utf8StringTests, FrontAndBack) {
    auto string = ""_utf8;
    EXPECT_THROW(std::ignore = string.front(), std::out_of_range);
    EXPECT_THROW(std::ignore = string.back(), std::out_of_range);
    string = "!";
    EXPECT_EQ(string.front(), '!');
    EXPECT_EQ(string.back(), '!');
    string = "C++ 🐀";
    EXPECT_EQ(string.front(), 'C');
    EXPECT_EQ(string.back(), *"🐀"_utf8view.cbegin());
}

TEST(Utf8StringTests, Join) {
    using namespace std::string_literals;
    EXPECT_EQ(", "_utf8.join(std::vector{ "this", "is", "a", "test" }), "this, is, a, test");

    char const* words[] = { "this", "is", "a", "test" };
    EXPECT_EQ(", "_utf8.join(words), "this, is, a, test");

    EXPECT_EQ(", "_utf8.join(std::vector<std::string>{}), "");

    EXPECT_EQ(", "_utf8.join(std::vector{ "single" }), "single");

    EXPECT_EQ(", "_utf8.join(std::vector{ "1", "2", "3", "4" }), "1, 2, 3, 4");

    char const* nullStrings[] = { nullptr, nullptr };
    EXPECT_THROW({ std::ignore = ", "_utf8.join(nullStrings); }, std::invalid_argument);
}

TEST(Utf8StringTests, Split) {
    using namespace std::string_literals;

    EXPECT_EQ("one;two;three"_utf8.split(";"), (std::vector{ "one"_utf8, "two"_utf8, "three"_utf8 }));
    EXPECT_EQ("one|two|three|four"_utf8.split("|"), (std::vector{ "one"_utf8, "two"_utf8, "three"_utf8, "four"_utf8 }));
    EXPECT_EQ("apple"_utf8.split("|"), std::vector{ "apple"_utf8 });
    EXPECT_EQ(""_utf8.split(";"), std::vector{ ""_utf8 });
    EXPECT_EQ(",,,"_utf8.split(","), (std::vector{ ""_utf8, ""_utf8, ""_utf8, ""_utf8 }));
    EXPECT_EQ("a,a,a"_utf8.split(","), (std::vector{ "a"_utf8, "a"_utf8, "a"_utf8 }));
    EXPECT_EQ(
            "banana;apple;cherry;blueberry;raspberry"_utf8.split(";"),
            (std::vector{ "banana"_utf8, "apple"_utf8, "cherry"_utf8, "blueberry"_utf8, "raspberry"_utf8 })
    );
    EXPECT_EQ(
            "data, more data, even more data"_utf8.split(","),
            (std::vector{ "data"_utf8, " more data"_utf8, " even more data"_utf8 })
    );
    EXPECT_EQ("this.is.a.test"_utf8.split("."), (std::vector{ "this"_utf8, "is"_utf8, "a"_utf8, "test"_utf8 }));
    EXPECT_EQ("no_delimiters"_utf8.split("#"), std::vector{ "no_delimiters"_utf8 });
    EXPECT_EQ("hello\nworld\n"_utf8.split("\n"), (std::vector{ "hello"_utf8, "world"_utf8, ""_utf8 }));
    EXPECT_EQ(
            ";semi;colons;everywhere;"_utf8.split(";"),
            (std::vector{ ""_utf8, "semi"_utf8, "colons"_utf8, "everywhere"_utf8, ""_utf8 })
    );
}

TEST(Utf8StringTests, Replace) {
    using namespace c2k::Utf8Literals;
    using c2k::MaxReplacementCount;
    EXPECT_EQ("abc"_utf8.replace("abc", "abc"), "abc");
    EXPECT_EQ("abc"_utf8.replace("b", "abc"), "aabcc");
    EXPECT_EQ("abc"_utf8.replace("d", ""), "abc");
    EXPECT_EQ(""_utf8.replace("", ""), "");
    EXPECT_EQ(""_utf8.replace("", "test"), "test");
    EXPECT_EQ("ǺBC"_utf8.replace("Ǻ", "A"), "ABC");
    EXPECT_EQ("Hello, 🌍!"_utf8.replace("🌍", "World"), "Hello, World!");
    EXPECT_EQ("abababab"_utf8.replace("aba", "<>"), "<>b<>b");
    EXPECT_EQ("abcabc"_utf8.replace("a", "ab"), "abbcabbc");
    EXPECT_EQ("abc"_utf8.replace("b", "bcd"), "abcdc");
    EXPECT_EQ("aaaa"_utf8.replace("a", "aa"), "aaaaaaaa");
    EXPECT_EQ("312312312312"_utf8.replace("23", "!"), "31!1!1!12");
    EXPECT_EQ("312312312312"_utf8.replace("23", "!!"), "31!!1!!1!!12");
    EXPECT_EQ("312312312312"_utf8.replace("23", "test"), "31test1test1test12");
    EXPECT_EQ("testtesttest"_utf8.replace("test", "a"), "aaa");
    EXPECT_EQ("Hello, Bjarne!"_utf8.replace("Bjarne", "Herb"), "Hello, Herb!");
    EXPECT_EQ("1, 2, 3"_utf8.replace(", ", "\n"), "1\n2\n3");
    EXPECT_EQ("to"_utf8.replace("too long", "something"), "to");
    EXPECT_EQ("abc"_utf8.replace("", "test"), "testatestbtestctest");
    EXPECT_EQ("Bjarne is cool, Scott is cool"_utf8.replace(" is cool", ""), "Bjarne, Scott");

    // with start iterator
    auto view = "aaaa"_utf8;
    EXPECT_EQ(view.replace("a", "!", view.cbegin() + 2), "aa!!");
    view = "NaNaNaNaNa";
    EXPECT_EQ(view.replace("Na", "Batman!", view.cbegin() + 8), "NaNaNaNaBatman!");
    view = "abc";
    EXPECT_EQ(view.replace("abc", "abc", view.cbegin()), "abc");
    view = "Bjarne is cool, Scott is cool";
    EXPECT_EQ(view.replace(" is cool", "", view.cbegin()), "Bjarne, Scott");
    view = "aaaa";
    EXPECT_EQ(view.replace("a", "!", view.cbegin() + 4), "aaaa");

    // with max replacement count
    EXPECT_EQ("aaaa"_utf8.replace("a", "!", MaxReplacementCount{ 2 }), "!!aa");
    EXPECT_EQ("aaaa"_utf8.replace("a", "!", MaxReplacementCount{ 0 }), "aaaa");
    EXPECT_EQ("aaaa"_utf8.replace("a", "!", MaxReplacementCount{ 1 }), "!aaa");
    EXPECT_EQ("aaaa"_utf8.replace("a", "!", MaxReplacementCount{ 4 }), "!!!!");
    EXPECT_EQ("aaaa"_utf8.replace("a", "!", MaxReplacementCount{ 5 }), "!!!!");

    // with start position and max replacement count
    view = "aaaaaaa";
    EXPECT_EQ(view.replace("aa", "!", view.cbegin() + 1, MaxReplacementCount{ 2 }), "a!!aa");
    view = "1, 2, 3, 4, 5";
    EXPECT_EQ(view.replace(", ", "\n", view.cbegin() + 2, MaxReplacementCount{ 2 }), "1, 2\n3\n4, 5");
}
