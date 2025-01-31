#include <gtest/gtest.h>
#include <lib2k/overloaded.hpp>
#include <string>
#include <variant>

TEST(OverloadedTests, Overloaded) {
    constexpr auto variant = std::variant<int, double, std::string>{ 42 };
    auto const overloaded = c2k::Overloaded{
        [](int const value) { return value; },
        [](double const value) { return static_cast<int>(value); },
        [](std::string const& value) { return static_cast<int>(value.size()); },
    };
    auto const result = std::visit(overloaded, variant);
    ASSERT_EQ(result, 42);
}
