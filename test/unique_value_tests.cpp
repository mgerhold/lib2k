#include <concepts>
#include <gtest/gtest.h>
#include <lib2k/unique_value.hpp>
#include <unordered_set>
#include <memory>

using c2k::UniqueValue;

static inline auto created_handles = std::unordered_set<int>{};
static inline auto next_destroy_handle = 1;

[[nodiscard]] static int create() {
    auto const handle = next_destroy_handle++;
    auto const [_, inserted] = created_handles.insert(handle);
    assert(inserted);
    return handle;
}

static void destroy(int const handle) {
    if (handle == 0) {
        return;
    }
    auto const num_removed = created_handles.erase(handle);
    if (num_removed == 0) {
        throw std::logic_error{ "trying to destroy non-existing handle" };
    }
    assert(num_removed == 1);
}

[[nodiscard]] bool all_destroyed() {
    return created_handles.empty();
}

TEST(UniqueValueTests, CreateUniqueValue) {
    using Deleter = void (*)(int);
    {
        auto const handle = UniqueValue<int, Deleter>{ create(), destroy };
        EXPECT_GT(*handle, 0);
    }
    EXPECT_TRUE(all_destroyed());
}

static_assert(not std::copy_constructible<UniqueValue<int>>);

TEST(UniqueValueTests, MoveConstructible) {
    using Deleter = void (*)(int);
    {
        auto original_handle = UniqueValue<int, Deleter>{ create(), destroy };
        EXPECT_GT(*original_handle, 0);
        auto const underlying_handle = *original_handle;
        auto new_handle = std::move(original_handle);
        EXPECT_FALSE(original_handle.has_value());
        EXPECT_EQ(underlying_handle, *new_handle);
    }
    EXPECT_TRUE(all_destroyed());
}

TEST(UniqueValueTests, VectorOfUniqueValues) {
    using Deleter = void (*)(int);
    {
        auto handles = std::vector<UniqueValue<int, Deleter>>{};
        for (auto i = 0; i < 1000; ++i) {
            handles.emplace_back(create(), destroy);
        }
    }
    EXPECT_TRUE(all_destroyed());
}

TEST(UniqueValueTests, UniqueValueWithNonCopyableInside) {
    auto original_handle = UniqueValue{ std::make_unique<int>(42) };
    auto const new_handle = std::move(original_handle);
}
