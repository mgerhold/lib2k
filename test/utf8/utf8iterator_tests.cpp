#include <gtest/gtest.h>
#include <lib2k/utf8.hpp>

using namespace c2k::Utf8Literals;

TEST(Utf8ConstIteratorTests, IncrementAndDecrement) {
    using c2k::Utf8StringView;
    auto const string = "Hello, C++ and its mascot 🐀!"_utf8view;
    auto const num_chars = static_cast<Utf8StringView::ConstIterator::difference_type>(string.calculate_char_count());
    auto it = string.cbegin();
    EXPECT_EQ(*it, 'H'_utf8);
    it = it + 1;
    EXPECT_EQ(*it, 'e'_utf8);
    it += 1;
    EXPECT_EQ(*it, 'l'_utf8);
    it += 3;
    EXPECT_EQ(*it, ','_utf8);
    it += -5;
    EXPECT_EQ(*it, 'H'_utf8);
    it += num_chars - 1;
    EXPECT_EQ(*it, '!'_utf8);
    it -= 1;
    EXPECT_EQ(*it, *"🐀"_utf8view.begin());
    it -= -1;
    EXPECT_EQ(*it, '!'_utf8);
    it -= num_chars - 1;
    EXPECT_EQ(*it, 'H'_utf8);
    it = 1 + it;
    EXPECT_EQ(*it, 'e'_utf8);
    it = it - 1;
    EXPECT_EQ(*it, 'H'_utf8);
}

TEST(Utf8ConstIteratorTests, Equality) {
    using c2k::Utf8StringView;
    using Difference = Utf8StringView::ConstIterator::difference_type;
    auto const string = "Hello, C++ and its mascot 🐀!"_utf8view;
    EXPECT_EQ(string.cbegin(), string.cbegin());
    EXPECT_NE(string.cbegin(), string.cbegin() + 1);
    EXPECT_EQ(string.cbegin() + static_cast<Difference>(string.calculate_char_count()), string.cend());
    EXPECT_NE(string.cbegin(), string.cend());
}

TEST(Utf8ConstIteratorTests, DifferenceCalculation) {
    using c2k::Utf8StringView;
    auto string = "test"_utf8view;
    EXPECT_EQ(string.cend() - string.cbegin(), 4);
    string = "Hello, 🐀!";
    EXPECT_EQ(string.cend() - string.cbegin(), 9);
    string = "";
    EXPECT_EQ(string.cend() - string.cbegin(), 0);
}

TEST(Utf8ConstReverseIteratorTests, IncrementAndDecrement) {
    using c2k::Utf8StringView;
    auto const string = "Hello, C++ and its mascot 🐀!"_utf8view;
    auto const num_chars = static_cast<Utf8StringView::ConstIterator::difference_type>(string.calculate_char_count());
    auto it = string.crbegin();
    EXPECT_EQ(*it, '!'_utf8);
    it = it + 1;
    EXPECT_EQ(*it, *"🐀"_utf8view.begin());
    it += 1;
    EXPECT_EQ(*it, ' '_utf8);
    it += 3;
    EXPECT_EQ(*it, 'c'_utf8);
    it += -5;
    EXPECT_EQ(*it, '!'_utf8);
    it += num_chars - 1;
    EXPECT_EQ(*it, 'H'_utf8);
    it -= 1;
    EXPECT_EQ(*it, 'e'_utf8);
    it -= -1;
    EXPECT_EQ(*it, 'H'_utf8);
    it -= num_chars - 1;
    EXPECT_EQ(*it, '!'_utf8);
    it = 1 + it;
    EXPECT_EQ(*it, *"🐀"_utf8view.begin());
    it = it - 1;
    EXPECT_EQ(*it, '!'_utf8);
}

TEST(Utf8ConstReverseIteratorTests, Equality) {
    using c2k::Utf8StringView;
    using Difference = Utf8StringView::ReverseIterator::difference_type;
    auto const string = "Hello, C++ and its mascot 🐀!"_utf8view;
    EXPECT_EQ(string.crbegin(), string.crbegin());
    EXPECT_NE(string.crbegin(), string.crbegin() + 1);
    EXPECT_EQ(string.crbegin() + static_cast<Difference>(string.calculate_char_count()), string.crend());
    EXPECT_NE(string.crbegin(), string.crend());
}

TEST(Utf8ConstReverseIteratorTests, DifferenceCalculation) {
    using c2k::Utf8StringView;
    auto string = "test"_utf8view;
    EXPECT_EQ(string.crend() - string.crbegin(), 4);
    string = "Hello, 🐀!";
    EXPECT_EQ(string.crend() - string.crbegin(), 9);
    string = "";
    EXPECT_EQ(string.crend() - string.crbegin(), 0);
}
