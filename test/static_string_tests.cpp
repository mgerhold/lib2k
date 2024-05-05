#include <gtest/gtest.h>
#include <lib2k/static_string.hpp>
#include <sstream>

TEST(StaticStringTests, Instantiating) {
    [[maybe_unused]] static constexpr auto s1 = StaticString{ "test" };
    [[maybe_unused]] static constexpr auto s2 = StaticString{ "" };
    [[maybe_unused]] static constexpr auto s3 = StaticString{ "Ferris: 🦀" };
    [[maybe_unused]] static constexpr auto s4 = "Hello, world!"_static;
    [[maybe_unused]] static constexpr auto s5 = ""_static;
    [[maybe_unused]] static constexpr auto s6 = "Ferris: 🦀"_static;
}

TEST(StaticStringTests, CString) {
    static constexpr auto s1 = ""_static;
    EXPECT_STREQ(s1.c_str(), "");
    static constexpr auto s2 = "test"_static;
    EXPECT_STREQ(s2.c_str(), "test");
}

TEST(StaticStringTests, StringView) {
    using namespace std::string_view_literals;

    static_assert(""_static.view() == ""sv);
    static_assert("test"_static.view() == "test"sv);
}

TEST(StaticStringTests, String) {
    using namespace std::string_literals;

    static_assert(""_static.str() == ""s);
    static_assert("test"_static.str() == "test"s);
}

TEST(StaticStringTests, OutputStreamOperator) {
    static constexpr auto s = "test"_static;
    auto stream = std::ostringstream{};
    stream << s;
    EXPECT_EQ(stream.str(), "test");
}

TEST(StaticStringTests, Equality) {
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    static_assert("test"_static == "test");
    static_assert("test"_static == "test"s);
    static_assert("test"_static == "test"sv);
    static_assert("test"_static == "test"_static);

    static_assert("test"_static != "tset");
    static_assert("test"_static != "tset"s);
    static_assert("test"_static != "tset"sv);
    static_assert("test"_static != "tset"_static);
    static_assert("test"_static != "");
    static_assert("test"_static != ""s);
    static_assert("test"_static != ""sv);
    static_assert("test"_static != ""_static);
}

TEST(StaticStringTests, Concatenation) {
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    static_assert("Hello"_static + ", world!" == "Hello, world!");
    static_assert("Hello"_static + ", world!"_static == "Hello, world!");
    static_assert("Hello" + ", world!"_static == "Hello, world!");
}

TEST(StaticStringTests, Join) {
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    static_assert(", "_static.join("1"_static, "2"_static, "3"_static) == "1, 2, 3");
    static_assert(", "_static.join("1", "2", "3") == "1, 2, 3");
    static_assert(", "_static.join("1") == "1");
}
