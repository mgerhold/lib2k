#include <gtest/gtest.h>
#include <lib2k/static_vector.hpp>
#include <numeric>
#include <string>

using c2k::StaticVector;

TEST(StaticVectorTests, DefaultConstruction) {
    static constexpr auto vector = StaticVector<int, 4>{};
    static_assert(vector.capacity() == 4); // NOLINT (access to static member through instance)
    static_assert(decltype(vector)::capacity() == 4);
    static_assert(vector.size() == 0); // NOLINT (comparing size with zero instead of calling empty)
    static_assert(vector.empty());
}

TEST(StaticVectorTests, ConstructionWithInitializerList) {
    static constexpr auto a = StaticVector<int, 4>{ 1, 2, 3, 4 };
    static_assert(a.capacity() == 4); // NOLINT (access to static member through instance)
    static_assert(decltype(a)::capacity() == 4);
    static_assert(a.size() == 4);
    static_assert(not a.empty());

    static constexpr auto b = StaticVector<int, 4>{ 1, 2 };
    static_assert(b.capacity() == 4); // NOLINT (access to static member through instance)
    static_assert(decltype(b)::capacity() == 4);
    static_assert(b.size() == 2);
    static_assert(not b.empty());

    EXPECT_THROW(std::ignore = (StaticVector<int, 4>{ 1, 2, 3, 4, 5 }), c2k::InsufficientCapacity);
}

TEST(StaticVectorTests, Iterating) {
    static constexpr auto vector = StaticVector<int, 4>{ 2, 5 };
    static constexpr auto expected = 10;
    auto actual = 1;
    for (auto const number : vector) {
        actual *= number;
    }
    EXPECT_EQ(expected, actual);

    actual = 1;
    for (auto it = std::cbegin(vector); it != std::cend(vector); ++it) { // NOLINT (not using range-based for loop)
        actual *= *it;
    }
    EXPECT_EQ(expected, actual);

    actual = 1;
    for (auto i = std::size_t{ 0 }; i < vector.size(); ++i) {
        actual *= vector[i];
    }
    EXPECT_EQ(expected, actual);

    actual = 1;
    for (auto i = std::size_t{ 0 }; i < vector.size(); ++i) {
        actual *= vector.at(i);
    }
    EXPECT_EQ(expected, actual);
}

template<std::size_t capacity>
[[nodiscard]] static constexpr StaticVector<int, capacity> generate_vector() {
    auto result = StaticVector<int, capacity>{};
    for (auto i = std::size_t{ 0 }; i < capacity; ++i) {
        result.push_back(static_cast<int>(i + 1));
    }
    return result;
}

TEST(StaticVectorTests, FrontAndBack) {
    auto vector = StaticVector<int, 4>{ 1, 2 };
    EXPECT_EQ(vector.front(), 1);
    EXPECT_EQ(vector.back(), 2);
    vector.front() += 41;
    vector.back() *= 5;
    EXPECT_EQ(vector.front(), 42);
    EXPECT_EQ(vector.back(), 10);
}

TEST(StaticVectorTests, PushBack) {
    static constexpr auto vector1 = generate_vector<100>();
    EXPECT_EQ(std::accumulate(vector1.cbegin(), vector1.cend(), 0), 5050);

    auto vector2 = generate_vector<100>();
    EXPECT_THROW(vector2.push_back(101), c2k::InsufficientCapacity);

    auto vector3 = StaticVector<int, 4>{};
    vector3.push_back(5);
    vector3.push_back(10);
    EXPECT_EQ(vector3.at(0), 5);
    EXPECT_EQ(vector3.at(1), 10);
    EXPECT_EQ(vector3.capacity(), 4);
    EXPECT_EQ(vector3.size(), 2);
    EXPECT_FALSE(vector3.empty());
}

struct Person {
    std::string name;
    int age;
};

TEST(StaticVectorTests, EmplaceBack) {
    auto vector = StaticVector<Person, 4>{};
    vector.emplace_back("Bjarne", 73);
    EXPECT_EQ(vector.at(0).name, "Bjarne");
    EXPECT_EQ(vector.at(0).age, 73);
    EXPECT_EQ(vector.size(), 1);
    EXPECT_FALSE(vector.empty());
}
