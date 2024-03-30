#include <concepts>
#include <gtest/gtest.h>
#include <lib2k/unique_value.hpp>
#include <memory>
#include <unordered_set>

class UniqueValueFixture : public testing::Test {
protected:
    std::unordered_set<int> created_handles{};
    int next_free_handle = 1; // 0 means "invalid/null handle"

    [[nodiscard]] int create() {
        auto const handle = next_free_handle++;
        [[maybe_unused]] auto const [_, inserted] = created_handles.insert(handle);
        assert(inserted);
        return handle;
    }

    void destroy(int const handle) {
        if (handle == 0) {
            return;
        }
        auto const num_removed = created_handles.erase(handle);
        if (num_removed == 0) {
            throw std::logic_error{ "trying to destroy non-existing handle" };
        }
        assert(num_removed == 1);
    }

    [[nodiscard]] bool all_destroyed() const {
        return created_handles.empty();
    }
};

using c2k::UniqueValue;

TEST_F(UniqueValueFixture, CreateUniqueValue) {
    {
        auto const handle = UniqueValue{ create(), [this](auto const n) {
            destroy(n);
        } };
        EXPECT_GT(*handle, 0);
    }
    EXPECT_TRUE(all_destroyed());
}

static_assert(not std::copy_constructible<UniqueValue<int>>);

TEST_F(UniqueValueFixture, MoveConstructible) {
    {
        auto original_handle = UniqueValue{ create(), [this](auto const handle) {
            destroy(handle);
        } };
        EXPECT_GT(*original_handle, 0);
        auto const underlying_handle = *original_handle;
        auto new_handle = std::move(original_handle);
        EXPECT_FALSE(original_handle.has_value());
        EXPECT_EQ(underlying_handle, *new_handle);
    }
    EXPECT_TRUE(all_destroyed());
}

TEST_F(UniqueValueFixture, VectorOfUniqueValues) {
    auto const deleter = [this](auto const handle) {
        destroy(handle);
    };
    using Deleter = decltype(deleter);
    {
        auto handles = std::vector<UniqueValue<int, Deleter>>{};
        for (auto i = 0; i < 1000; ++i) {
            handles.emplace_back(create(), deleter);
        }
    }
    EXPECT_TRUE(all_destroyed());
}

TEST(UniqueValueTests, UniqueValueWithNonCopyableInside) {
    auto original_handle = UniqueValue{ std::make_unique<int>(42) };
    auto const new_handle = std::move(original_handle);
}

TEST_F(UniqueValueFixture, DestroyIsCalledOnInvalidHandle) {
#ifdef _WIN32
    EXPECT_DEATH(
        {
        std::ignore = (UniqueValue{ 42, [this](auto const handle) { destroy(handle); } });
        },
        // MSVC doesn't show the exception message
        ""
    );
#else
    EXPECT_DEATH(
            {
                std::ignore = (UniqueValue{ 42, [this](auto const handle) { destroy(handle); } });
            },
            "trying to destroy non-existing handle"
    );
#endif
}

TEST(UniqueValueTests, MemberAccessOperator) {
    auto value = UniqueValue{ std::string{ "test" } };
    EXPECT_EQ(value->length(), 4);
    auto const value2 = std::move(value);
    EXPECT_THROW(std::ignore = value->length(), std::bad_optional_access);
    EXPECT_EQ(value2->length(), 4);
}
