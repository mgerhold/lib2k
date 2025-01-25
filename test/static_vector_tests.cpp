#include <algorithm>
#include <gtest/gtest.h>
#include <lib2k/static_vector.hpp>
#include <numeric>
#include <ranges>
#include <string>

using c2k::StaticVector;

struct Pod final {
    int n;
    double d;
    float f;

    [[nodiscard]] constexpr auto operator<=>(Pod const&) const = default;
};

struct NotDefaultConstructible final {
    constexpr NotDefaultConstructible() = delete;
    explicit constexpr NotDefaultConstructible(int) { }

    [[nodiscard]] constexpr auto operator<=>(NotDefaultConstructible const&) const = default;
};

struct CopyableButNotMovable {
    constexpr CopyableButNotMovable() = default;
    constexpr CopyableButNotMovable(CopyableButNotMovable const&) = default;
    constexpr CopyableButNotMovable& operator=(CopyableButNotMovable const&) = default;
    CopyableButNotMovable(CopyableButNotMovable&&) = delete;
    CopyableButNotMovable& operator=(CopyableButNotMovable&&) = delete;
};

class IntHolder final {
private:
    int* m_value;

public:
    [[nodiscard]] explicit IntHolder(int const value) : m_value{ new int{ value } } { }

    IntHolder(IntHolder const& other) = delete;

    [[nodiscard]] IntHolder(IntHolder&& other) noexcept : m_value{ std::exchange(other.m_value, nullptr) } { }

    IntHolder& operator=(IntHolder const& other) = delete;

    IntHolder& operator=(IntHolder&& other) noexcept {
        if (this == std::addressof(other)) {
            return *this;
        }
        delete m_value;
        m_value = std::exchange(other.m_value, nullptr);
        return *this;
    }

    ~IntHolder() noexcept {
        delete m_value;
    }

    [[nodiscard]] int value() const {
        return *m_value;
    }
};

TEST(StaticVectorTests, DefaultConstruction) {
    static constexpr auto vector = StaticVector<int, 4>{};
    static_assert(vector.capacity() == 4); // NOLINT (access to static member through instance)
    static_assert(decltype(vector)::capacity() == 4);
    static_assert(vector.size() == 0); // NOLINT (comparing size with zero instead of calling empty)
    static_assert(vector.empty());

    static constexpr auto vector2 = StaticVector<Pod, 4>{};
    static_assert(vector2.capacity() == 4); // NOLINT (access to static member through instance)
    static_assert(decltype(vector2)::capacity() == 4);
    static_assert(vector2.size() == 0); // NOLINT (comparing size with zero instead of calling empty)
    static_assert(vector2.empty());

    static constexpr auto vector3 = StaticVector<NotDefaultConstructible, 4>{};
    static_assert(vector3.capacity() == 4); // NOLINT (access to static member through instance)
    static_assert(decltype(vector3)::capacity() == 4);
    static_assert(vector3.size() == 0); // NOLINT (comparing size with zero instead of calling empty)
    static_assert(vector3.empty());

    static constexpr auto vector4 = StaticVector<std::array<NotDefaultConstructible, 2>, 4>{};
    static_assert(vector4.capacity() == 4); // NOLINT (access to static member through instance)
    static_assert(decltype(vector4)::capacity() == 4);
    static_assert(vector4.size() == 0); // NOLINT (comparing size with zero instead of calling empty)
    static_assert(vector4.empty());

    static constexpr auto vector5 = StaticVector<CopyableButNotMovable, 4>{};
    static_assert(vector5.capacity() == 4); // NOLINT (access to static member through instance)
    static_assert(decltype(vector5)::capacity() == 4);
    static_assert(vector5.size() == 0); // NOLINT (comparing size with zero instead of calling empty)
    static_assert(vector5.empty());

    static auto constexpr vector6 = StaticVector<std::unique_ptr<int>, 4>{};
    EXPECT_EQ(vector6.capacity(), 4); // NOLINT (access to static member through instance)
    EXPECT_EQ(decltype(vector6)::capacity(), 4);
    EXPECT_EQ(vector6.size(), 0); // NOLINT (comparing size with zero instead of calling empty)
    EXPECT_TRUE(vector6.empty());
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

    static constexpr auto c = StaticVector<Pod, 3>{
        Pod{ 1, 2.0, 3.0f },
        Pod{ 4, 5.0, 6.0f },
    };
    static_assert(c.capacity() == 3); // NOLINT (access to static member through instance)
    static_assert(decltype(c)::capacity() == 3);
    static_assert(c.size() == 2);
    static_assert(not c.empty());

    // Non-default constructible types don't allow the StaticVector to be constexpr.
    auto const d = StaticVector<NotDefaultConstructible, 4>{
        NotDefaultConstructible{ 1 },
        NotDefaultConstructible{ 2 },
        NotDefaultConstructible{ 3 },
    };
    EXPECT_EQ(d.capacity(), 4); // NOLINT (access to static member through instance)
    EXPECT_EQ(decltype(d)::capacity(), 4);
    EXPECT_EQ(d.size(), 3);
    EXPECT_FALSE(d.empty());

    static constexpr auto e = StaticVector<CopyableButNotMovable, 4>{
        CopyableButNotMovable{},
        CopyableButNotMovable{},
    };
}

TEST(StaticVectorTests, Copying) {
    static constexpr auto a = StaticVector<int, 4>{ 1, 2, 3, 4 };
    static constexpr auto b = a;
    static_assert(a == b);
    auto c = decltype(a){};
    c = a;
    EXPECT_EQ(a, c);

    static constexpr auto d = StaticVector<Pod, 3>{
        Pod{ 1, 2.0, 3.0f },
        Pod{ 4, 5.0, 6.0f },
    };
    static constexpr auto e = d;
    static_assert(d == e);
    auto f = decltype(d){};
    f = d;
    EXPECT_EQ(d, f);

    auto const g = StaticVector<NotDefaultConstructible, 4>{
        NotDefaultConstructible{ 1 },
        NotDefaultConstructible{ 2 },
        NotDefaultConstructible{ 3 },
    };
    auto h = g;
    EXPECT_EQ(g, h);

    static constexpr auto i = StaticVector<CopyableButNotMovable, 4>{
        CopyableButNotMovable{},
        CopyableButNotMovable{},
    };
    static constexpr auto j = i;
    auto k = decltype(j){};
    k = j;
}

TEST(StaticVectorTests, Moving) {
    static constexpr auto lambda = [] {
        auto a = StaticVector<std::unique_ptr<int>, 4>{};
        a.push_back(std::make_unique<int>(1));
        a.push_back(std::make_unique<int>(2));
        a.push_back(std::make_unique<int>(3));
        a.push_back(std::make_unique<int>(4));
        auto b = std::move(a); // Move constructor.
        auto range = b | std::views::transform([](auto const& ptr) { return *ptr; });
        return std::accumulate(range.begin(), range.end(), 0);
    };
    static constexpr auto a = lambda();
    static_assert(a == 10);

    static constexpr auto lambda2 = [] {
        auto a2 = StaticVector<std::unique_ptr<int>, 4>{};
        a2.push_back(std::make_unique<int>(1));
        a2.push_back(std::make_unique<int>(2));
        a2.push_back(std::make_unique<int>(3));
        a2.push_back(std::make_unique<int>(4));
        auto b2 = decltype(a2){};
        b2 = std::move(a2); // Move assignment.
        auto range = b2 | std::views::transform([](auto const& ptr) { return *ptr; });
        return std::accumulate(range.begin(), range.end(), 0);
    };
    static constexpr auto b = lambda2();
    static_assert(b == 10);

    auto c = StaticVector<IntHolder, 4>{};
    c.push_back(IntHolder{ 1 });
    c.push_back(IntHolder{ 2 });
    c.push_back(IntHolder{ 3 });
    c.push_back(IntHolder{ 4 });
    auto d = std::move(c);
    EXPECT_TRUE(c.empty());
    EXPECT_EQ(d.size(), 4);
    auto const sum = std::accumulate(d.cbegin(), d.cend(), 0, [](auto const acc, auto const& holder) {
        return acc + holder.value();
    });
    EXPECT_EQ(sum, 10);
    auto e = decltype(c){};
    e = std::move(d);
    EXPECT_TRUE(d.empty());
    EXPECT_EQ(e.size(), 4);
    auto const sum2 = std::accumulate(e.cbegin(), e.cend(), 0, [](auto const acc, auto const& holder) {
        return acc + holder.value();
    });
    EXPECT_EQ(sum2, 10);
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

    static constexpr auto lambda = [] {
        auto a = StaticVector<std::unique_ptr<int>, 4>{};
        a.push_back(std::make_unique<int>(1));
        a.push_back(std::make_unique<int>(2));
        a.push_back(std::make_unique<int>(3));
        a.push_back(std::make_unique<int>(4));
        auto range = a | std::views::transform([](auto const& ptr) { return *ptr; });
        return std::accumulate(range.begin(), range.end(), 0);
    };
    static constexpr auto a = lambda();
    static_assert(a == 10);

    auto b = StaticVector<IntHolder, 10>{};
    b.emplace_back(1);
    b.emplace_back(2);
    b.emplace_back(3);
    auto const sum = std::accumulate(b.cbegin(), b.cend(), 0, [](auto const acc, auto const& holder) {
        return acc + holder.value();
    });
    EXPECT_EQ(sum, 6);
}

TEST(StaticVectorTests, PopBack) {
    auto vector = generate_vector<20>();
    EXPECT_EQ(vector.size(), 20);
    EXPECT_EQ(vector.back(), 20);
    EXPECT_EQ(vector.pop_back(), 20);
    EXPECT_EQ(vector.size(), 19);
    EXPECT_EQ(vector.back(), 19);

    auto vector2 = StaticVector<int, 1>{ 42 };
    EXPECT_EQ(vector2.size(), 1);
    EXPECT_EQ(vector2.pop_back(), 42);
    EXPECT_TRUE(vector2.empty());
    EXPECT_THROW(vector2.pop_back(), std::out_of_range);
}

TEST(StaticVectorTests, Clear) {
    auto vector = StaticVector<int, 4>{ 1, 2, 3, 4 };
    vector.clear();
    EXPECT_EQ(vector.size(), 0);
    EXPECT_TRUE(vector.empty());
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

TEST(StaticVectorTests, At) {
    static constexpr auto lambda = [] {
        auto a = StaticVector<std::unique_ptr<int>, 4>{};
        a.push_back(std::make_unique<int>(42));
        return *a.at(0);
    };
    static_assert(lambda() == 42);

    auto a = StaticVector<IntHolder, 4>{};
    a.emplace_back(42);
    EXPECT_EQ(a.at(0).value(), 42);
}

TEST(StaticVectorTests, Comparisons) {
    static constexpr auto a = StaticVector<int, 4>{ 1, 2, 3, 4 };
    static constexpr auto b = StaticVector<int, 4>{ 1, 2, 3, 4 };
    static_assert(a == b);
    static_assert(not(a != b)); // NOLINT (expression can be simplified)
    static constexpr auto c = StaticVector<int, 4>{ 1, 2, 3, 2 };
    static_assert(a != c);
    static_assert(a > c);
    static_assert(a >= c);
    static_assert(c < a);
    static_assert(c <= a);

    struct Test {
        int n;

        bool operator==(Test const& other) const = default;
    };

    static constexpr auto d = StaticVector<Test, 4>{ Test{ 1 }, Test{ 2 }, Test{ 3 }, Test{ 4 } };
    static constexpr auto e = StaticVector<Test, 4>{ Test{ 1 }, Test{ 2 }, Test{ 3 }, Test{ 4 } };
    static_assert(d == e);
}
