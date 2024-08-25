#include <gtest/gtest.h>
#include <lib2k/defer.hpp>

TEST(Defer, DeferTests) {
    auto i = 41;
    {
        auto const _ = c2k::Defer{ [&i] { ++i; } };
    }
    EXPECT_EQ(42, i);
}
