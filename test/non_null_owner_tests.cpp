#include <format>
#include <gtest/gtest.h>
#include <lib2k/non_null_owner.hpp>

using c2k::make_non_null_owner;

TEST(NonNullOwnerTests, TestConstructionAndAssignment) {
    auto owner = make_non_null_owner<int>(123);
    EXPECT_EQ(*owner, 123);

    owner = make_non_null_owner<int>(456);
    EXPECT_EQ(*owner, 456);
}

TEST(NonNullOwnerTests, TestMoveConstruction) {
    auto original_owner = make_non_null_owner<int>(101112);
    auto moved_owner = std::move(original_owner);
    EXPECT_EQ(*moved_owner, 101112);
    EXPECT_EQ(*original_owner, int{}); // Verifies it's in a default-constructed state
}

TEST(NonNullOwnerTests, TestDestruction) {
    { [[maybe_unused]] auto owner = make_non_null_owner<int>(131415); }
}

TEST(NonNullOwnerTests, TestConstructionAssignment) {
    auto owner = make_non_null_owner<int>(123);
    EXPECT_EQ(*owner, 123);

    owner = make_non_null_owner<int>(456);
    EXPECT_EQ(*owner, 456);
}


struct Person final {
    std::string first_name;
    std::string last_name;
    int age{};

    Person() = default;

    Person(std::string first_name, std::string last_name, int const age)
        : first_name{ std::move(first_name) },
          last_name{ std::move(last_name) },
          age{ age } { }

    [[nodiscard]] std::string get_name() const {
        return std::format("{} {}", first_name, last_name);
    }

    [[nodiscard]] int get_age() const {
        return age;
    }
};
TEST(NonNullOwnerTests, TestConstructionAndAssignmentWithString) {
    auto owner = make_non_null_owner<std::string>("Hello, world!");
    EXPECT_EQ(*owner, "Hello, world!");

    owner = make_non_null_owner<std::string>("New string");
    EXPECT_EQ(*owner, "New string");
}

TEST(NonNullOwnerTests, TestMoveConstructionWithString) {
    auto original_owner = make_non_null_owner<std::string>("Original string");
    auto moved_owner = std::move(original_owner);
    EXPECT_EQ(*moved_owner, "Original string");
    EXPECT_TRUE(original_owner->empty()); // Verifies it's in a moved-from state
}

TEST(NonNullOwnerTests, TestConstructionAndAssignmentWithPerson) {
    auto owner = make_non_null_owner<Person>("John", "Doe", 30);
    EXPECT_EQ(owner->get_name(), "John Doe");
    EXPECT_EQ(owner->get_age(), 30);

    owner = make_non_null_owner<Person>("Jane", "Doe", 28);
    EXPECT_EQ(owner->get_name(), "Jane Doe");
    EXPECT_EQ(owner->get_age(), 28);
}

TEST(NonNullOwnerTests, TestMoveConstructionWithPerson) {
    auto original_owner = make_non_null_owner<Person>("John", "Doe", 30);
    auto moved_owner = std::move(original_owner);

    EXPECT_EQ(moved_owner->get_name(), "John Doe");
    EXPECT_EQ(moved_owner->get_age(), 30);

    EXPECT_EQ(original_owner->get_name(), " ");
    EXPECT_EQ(original_owner->get_age(), 0);
}
