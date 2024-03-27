#include <gtest/gtest.h>
#include <lib2k/string_utils.hpp>

TEST(StringUtilsTests, RegularLeftTrim) {
    using c2k::left_trim;
    using namespace std::string_literals;

    auto s = "  simple test  "s;
    left_trim(s);
    EXPECT_EQ(s, "simple test  ");

    s = "\t\t\v hello";
    left_trim(s);
    EXPECT_EQ(s, "hello");

    s = " ";
    left_trim(s);
    EXPECT_EQ(s, "");

    s = "test  ";
    left_trim(s);
    EXPECT_EQ(s, "test  ");
}

TEST(StringUtilsTests, LeftTrimWithCustomChars) {
    using c2k::left_trim;
    using namespace std::string_literals;

    static constexpr auto to_erase = "!#.";

    auto s = "  simple test  "s;
    left_trim(s, to_erase);
    EXPECT_EQ(s, "  simple test  ");

    s = "!test";
    left_trim(s, to_erase);
    EXPECT_EQ(s, "test");

    s = "...test";
    left_trim(s, to_erase);
    EXPECT_EQ(s, "test");

    s = ".#!!#..test";
    left_trim(s, to_erase);
    EXPECT_EQ(s, "test");

    s = "...";
    left_trim(s, to_erase);
    EXPECT_EQ(s, "");

    s = " ";
    left_trim(s, to_erase);
    EXPECT_EQ(s, " ");
}

TEST(StringUtilsTests, LeftTrimWithPredicate) {
    using c2k::left_trim;
    using namespace std::string_literals;

    static constexpr auto predicate = [](char const c) {
        return static_cast<bool>(std::isdigit(static_cast<unsigned char>(c)));
    };

    auto s = "  simple test  "s;
    left_trim(s, predicate);
    EXPECT_EQ(s, "  simple test  ");

    s = "123test123";
    left_trim(s, predicate);
    EXPECT_EQ(s, "test123");

    s = "123";
    left_trim(s, predicate);
    EXPECT_EQ(s, "");

    s = " 123";
    left_trim(s, predicate);
    EXPECT_EQ(s, " 123");

    s = " ";
    left_trim(s, predicate);
    EXPECT_EQ(s, " ");
}

TEST(StringUtilsTests, RegularLeftTrimmed) {
    using c2k::left_trimmed;
    using namespace std::string_literals;

    EXPECT_EQ(left_trimmed("  test"), "test");

    auto s = "  test"s;
    EXPECT_EQ(left_trimmed(std::move(s)), "test");

    EXPECT_EQ(left_trimmed("  test  "), "test  ");
    EXPECT_EQ(left_trimmed(""), "");
    EXPECT_EQ(left_trimmed(" "), "");
    EXPECT_EQ(left_trimmed("test test"), "test test");
}

TEST(StringUtilsTests, LeftTrimmedWithCustomChars) {
    using c2k::left_trimmed;
    using namespace std::string_literals;

    static constexpr auto to_erase = "!#.";

    EXPECT_EQ(left_trimmed("  simple test  "s, to_erase), "  simple test  ");
    EXPECT_EQ(left_trimmed("!test"s, to_erase), "test");
    EXPECT_EQ(left_trimmed("...test"s, to_erase), "test");
    EXPECT_EQ(left_trimmed(".#!!#..test"s, to_erase), "test");
    EXPECT_EQ(left_trimmed("..."s, to_erase), "");
    EXPECT_EQ(left_trimmed(" "s, to_erase), " ");
}

TEST(StringUtilsTests, LeftTrimmedWithPredicate) {
    using c2k::left_trimmed;
    using namespace std::string_literals;

    static constexpr auto predicate = [](char const c) {
        return static_cast<bool>(std::isdigit(static_cast<unsigned char>(c)));
    };

    EXPECT_EQ(left_trimmed("  simple test  "s, predicate), "  simple test  ");
    EXPECT_EQ(left_trimmed("123test123"s, predicate), "test123");
    EXPECT_EQ(left_trimmed("123"s, predicate), "");
    EXPECT_EQ(left_trimmed(" 123"s, predicate), " 123");
    EXPECT_EQ(left_trimmed(" "s, predicate), " ");
}

TEST(StringUtilsTests, RegularRightTrim) {
    using c2k::right_trim;
    using namespace std::string_literals;

    auto s = "  simple test  "s;
    right_trim(s);
    EXPECT_EQ(s, "  simple test");

    s = "hello\t\t\v";
    right_trim(s);
    EXPECT_EQ(s, "hello");

    s = " ";
    right_trim(s);
    EXPECT_EQ(s, "");

    s = "  test";
    right_trim(s);
    EXPECT_EQ(s, "  test");
}

TEST(StringUtilsTests, RightTrimWithCustomChars) {
    using c2k::right_trim;
    using namespace std::string_literals;

    static constexpr auto to_erase = "!#.";

    auto s = "  simple test  "s;
    right_trim(s, to_erase);
    EXPECT_EQ(s, "  simple test  ");

    s = "test!";
    right_trim(s, to_erase);
    EXPECT_EQ(s, "test");

    s = "test...";
    right_trim(s, to_erase);
    EXPECT_EQ(s, "test");

    s = "test.#!!#..";
    right_trim(s, to_erase);
    EXPECT_EQ(s, "test");

    s = "...";
    right_trim(s, to_erase);
    EXPECT_EQ(s, "");

    s = " ";
    right_trim(s, to_erase);
    EXPECT_EQ(s, " ");
}

TEST(StringUtilsTests, RightTrimWithPredicate) {
    using c2k::right_trim;
    using namespace std::string_literals;

    static constexpr auto predicate = [](char const c) {
        return static_cast<bool>(std::isdigit(static_cast<unsigned char>(c)));
    };

    auto s = "  simple test  "s;
    right_trim(s, predicate);
    EXPECT_EQ(s, "  simple test  ");

    s = "123test123";
    right_trim(s, predicate);
    EXPECT_EQ(s, "123test");

    s = "123";
    right_trim(s, predicate);
    EXPECT_EQ(s, "");

    s = " 123";
    right_trim(s, predicate);
    EXPECT_EQ(s, " ");

    s = " ";
    right_trim(s, predicate);
    EXPECT_EQ(s, " ");
}

TEST(StringUtilsTests, RegularRightTrimmed) {
    using c2k::right_trimmed;
    using namespace std::string_literals;

    EXPECT_EQ(right_trimmed("test  "), "test");

    auto s = "test  "s;
    EXPECT_EQ(right_trimmed(std::move(s)), "test");

    EXPECT_EQ(right_trimmed("  test  "), "  test");
    EXPECT_EQ(right_trimmed(""), "");
    EXPECT_EQ(right_trimmed(" "), "");
    EXPECT_EQ(right_trimmed("test test"), "test test");
}

TEST(StringUtilsTests, RightTrimmedWithCustomChars) {
    using c2k::right_trimmed;
    using namespace std::string_literals;

    static constexpr auto to_erase = "!#.";

    EXPECT_EQ(right_trimmed("  simple test  "s, to_erase), "  simple test  ");
    EXPECT_EQ(right_trimmed("test!"s, to_erase), "test");
    EXPECT_EQ(right_trimmed("test..."s, to_erase), "test");
    EXPECT_EQ(right_trimmed("test.#!!#.."s, to_erase), "test");
    EXPECT_EQ(right_trimmed("..."s, to_erase), "");
    EXPECT_EQ(right_trimmed(" "s, to_erase), " ");
}

TEST(StringUtilsTests, RightTrimmedWithPredicate) {
    using c2k::right_trimmed;
    using namespace std::string_literals;

    static constexpr auto predicate = [](char const c) {
        return static_cast<bool>(std::isdigit(static_cast<unsigned char>(c)));
    };

    EXPECT_EQ(right_trimmed("  simple test  "s, predicate), "  simple test  ");
    EXPECT_EQ(right_trimmed("123test123"s, predicate), "123test");
    EXPECT_EQ(right_trimmed("123"s, predicate), "");
    EXPECT_EQ(right_trimmed(" 123"s, predicate), " ");
    EXPECT_EQ(right_trimmed(" "s, predicate), " ");
}

TEST(StringUtilsTests, RegularTrim) {
    using c2k::trim;
    using namespace std::string_literals;

    auto s = "  simple test  "s;
    trim(s);
    EXPECT_EQ(s, "simple test");

    s = "\t\t\v hello \v\t\t";
    trim(s);
    EXPECT_EQ(s, "hello");

    s = " ";
    trim(s);
    EXPECT_EQ(s, "");

    s = "test";
    trim(s);
    EXPECT_EQ(s, "test");
}

TEST(StringUtilsTests, TrimWithCustomChars) {
    using c2k::trim;
    using namespace std::string_literals;

    static constexpr auto to_erase = "!#.";

    auto s = "  simple test  "s;
    trim(s, to_erase);
    EXPECT_EQ(s, "  simple test  ");

    s = "!test!";
    trim(s, to_erase);
    EXPECT_EQ(s, "test");

    s = "...test...";
    trim(s, to_erase);
    EXPECT_EQ(s, "test");

    s = ".#!!#..test.#!!#..";
    trim(s, to_erase);
    EXPECT_EQ(s, "test");

    s = "...";
    trim(s, to_erase);
    EXPECT_EQ(s, "");

    s = " ";
    trim(s, to_erase);
    EXPECT_EQ(s, " ");
}

TEST(StringUtilsTests, TrimWithPredicate) {
    using c2k::trim;
    using namespace std::string_literals;

    static constexpr auto predicate = [](char const c) {
        return static_cast<bool>(std::isdigit(static_cast<unsigned char>(c)));
    };

    auto s = "  simple test  "s;
    trim(s, predicate);
    EXPECT_EQ(s, "  simple test  ");

    s = "123test123";
    trim(s, predicate);
    EXPECT_EQ(s, "test");

    s = "123";
    trim(s, predicate);
    EXPECT_EQ(s, "");

    s = " 123 ";
    trim(s, predicate);
    EXPECT_EQ(s, " 123 ");

    s = " ";
    trim(s, predicate);
    EXPECT_EQ(s, " ");
}

TEST(StringUtilsTests, RegularTrimmed) {
    using c2k::trimmed;
    using namespace std::string_literals;

    EXPECT_EQ(trimmed("  test  "), "test");

    auto s = "  test  "s;
    EXPECT_EQ(trimmed(std::move(s)), "test");

    EXPECT_EQ(trimmed("test test"), "test test");
}

TEST(StringUtilsTests, TrimmedWithCustomChars) {
    using c2k::trimmed;
    using namespace std::string_literals;

    static constexpr auto to_erase = "!#.";

    EXPECT_EQ(trimmed("  simple test  "s, to_erase), "  simple test  ");
    EXPECT_EQ(trimmed("!test!"s, to_erase), "test");
    EXPECT_EQ(trimmed("...test..."s, to_erase), "test");
    EXPECT_EQ(trimmed(".#!!#..test.#!!#.."s, to_erase), "test");
    EXPECT_EQ(trimmed("..."s, to_erase), "");
    EXPECT_EQ(trimmed(" "s, to_erase), " ");
}

TEST(StringUtilsTests, TrimmedWithPredicate) {
    using c2k::trimmed;
    using namespace std::string_literals;

    static constexpr auto predicate = [](char const c) {
        return static_cast<bool>(std::isdigit(static_cast<unsigned char>(c)));
    };

    EXPECT_EQ(trimmed("  simple test  "s, predicate), "  simple test  ");
    EXPECT_EQ(trimmed("123test123"s, predicate), "test");
    EXPECT_EQ(trimmed("123"s, predicate), "");
    EXPECT_EQ(trimmed(" 123 "s, predicate), " 123 ");
    EXPECT_EQ(trimmed(" "s, predicate), " ");
}

TEST(StringUtilsTests, Split) {
    using c2k::split;
    using namespace std::string_literals;

    EXPECT_EQ(split("one;two;three", ";"), (std::vector{ "one"s, "two"s, "three"s }));
    EXPECT_EQ(split("one|two|three|four", "|"), (std::vector{ "one"s, "two"s, "three"s, "four"s }));
    EXPECT_EQ(split("apple", "|"), std::vector{ "apple"s });
    EXPECT_EQ(split("", ";"), std::vector{ ""s });
    EXPECT_EQ(split(",,,", ","), (std::vector{ ""s, ""s, ""s, ""s }));
    EXPECT_EQ(split("a,a,a", ","), (std::vector{ "a"s, "a"s, "a"s }));
    EXPECT_EQ(
            split("banana;apple;cherry;blueberry;raspberry", ";"),
            (std::vector{ "banana"s, "apple"s, "cherry"s, "blueberry"s, "raspberry"s })
    );
    EXPECT_EQ(
            split("data, more data, even more data", ","),
            (std::vector{ "data"s, " more data"s, " even more data"s })
    );
    EXPECT_EQ(split("this.is.a.test", "."), (std::vector{ "this"s, "is"s, "a"s, "test"s }));
    EXPECT_EQ(split("no_delimiters", "#"), (std::vector{ "no_delimiters"s }));
    EXPECT_EQ(split("hello\nworld\n", "\n"), (std::vector{ "hello"s, "world"s, ""s }));
    EXPECT_EQ(split(";semi;colons;everywhere;", ";"), (std::vector{ ""s, "semi"s, "colons"s, "everywhere"s, ""s }));
}

TEST(StringUtilsTests, SplitWithEmptyDelimiterFails) {
    EXPECT_THROW({ std::ignore = c2k::split("1, 2, 3", ""); }, std::invalid_argument);
}

TEST(StringUtilsTests, Join) {
    using c2k::join;
    using namespace std::string_literals;

    EXPECT_EQ(join(std::vector{ "this", "is", "a", "test" }, ", "), "this, is, a, test");

    char const* words[] = { "this", "is", "a", "test" };
    EXPECT_EQ(join(words, ", "), "this, is, a, test");

    EXPECT_EQ(join(std::vector<std::string>{}, ", "), "");

    EXPECT_EQ(join(std::vector{ "single" }, ", "), "single");

    EXPECT_EQ(join(std::vector{ "1", "2", "3", "4" }, ", "), "1, 2, 3, 4");

    char const* nullStrings[] = { nullptr, nullptr };
    EXPECT_THROW({ std::ignore = join(nullStrings, ", "); }, std::invalid_argument);
}

TEST(StringUtilsTests, VariadicJoin) {
    using c2k::join;
    EXPECT_EQ(join(", ", "this", "is", "a", "test"), "this, is, a, test");
    EXPECT_EQ(join("ab", "ab"), "ab");
    EXPECT_EQ(join("", ""), "");
    EXPECT_EQ(join("", "a"), "a");
    EXPECT_EQ(join("", "a", "b", "c"), "abc");
}

TEST(StringUtilsTests, Repeated) {
    using c2k::repeated;
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    EXPECT_EQ(repeated("abc", 3), "abcabcabc");
    EXPECT_EQ(repeated("abc", 0), "");
    EXPECT_EQ(repeated("", 10), "");
    EXPECT_EQ(repeated("*", 5), "*****");

    static constexpr auto a = "test";
    auto const b = "test"s;
    static constexpr auto c = "test"sv;
    EXPECT_EQ(repeated(a, 3), "testtesttest");
    EXPECT_EQ(repeated(b, 3), "testtesttest");
    EXPECT_EQ(repeated(c, 3), "testtesttest");
    EXPECT_NO_THROW(std::ignore = repeated("abc", static_cast<std::size_t>(1e6)));
    EXPECT_EQ(repeated("你好", 3), "你好你好你好");
}
