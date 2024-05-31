#include <array>
#include <gtest/gtest.h>
#include <lib2k/random.hpp>
#include <span>
#include <vector>

TEST(RandomTests, SameSeedProducesSameResults) {
    using c2k::Random;
    static constexpr auto num_values = 10000;
    static constexpr auto num_iterations = 100;
    auto const seed = Random{}.next_integral<Random::Seed>();

    auto const original = [&] {
        auto random = Random{ seed };
        auto values = std::vector<int>{};
        values.reserve(num_values);
        for (auto i = 0; i < num_values; ++i) {
            values.push_back(random.next_integral<int>());
        }
        return values;
    }();

    for (auto i = 0; i < num_iterations; ++i) {
        auto random = Random{ seed };
        for (auto const value : original) {
            EXPECT_EQ(random.next_integral<int>(), value);
        }
    }
}

TEST(RandomTests, CopiedRandomInstanceProducesSameResults) {
    using c2k::Random;
    static constexpr auto num_values = 10000;

    auto original = Random{};
    auto copy = original;
    for (auto i = 0; i < num_values; ++i) {
        EXPECT_EQ(original.next_integral<int>(), copy.next_integral<int>());
    }
}

TEST(RandomTests, GetAndSetState) {
    using c2k::Random;
    static constexpr auto num_values = 10000;
    auto random = Random{};
    auto const state = random.state();

    auto const original = [&] {
        auto values = std::vector<int>{};
        values.reserve(num_values);
        for (auto i = 0; i < num_values; ++i) {
            values.push_back(random.next_integral<int>());
        }
        return values;
    }();

    random.state(state);
    for (auto const value : original) {
        EXPECT_EQ(random.next_integral<int>(), value);
    }
}

TEST(RandomTest, RandomIntegers) {
    using c2k::Random;
    auto random = Random{};
    for (auto i = 0; i < 1000000; ++i) {
        std::ignore = random.next_integral<std::uint16_t>();
    }
    for (auto i = 0; i < 1000000; ++i) {
        std::ignore = random.next_integral<std::uint32_t>();
    }
    for (auto i = 0; i < 1000000; ++i) {
        std::ignore = random.next_integral<std::uint64_t>();
    }
    for (auto i = 0; i < 1000000; ++i) {
        std::ignore = random.next_integral<std::int16_t>();
    }
    for (auto i = 0; i < 1000000; ++i) {
        std::ignore = random.next_integral<std::int32_t>();
    }
    for (auto i = 0; i < 1000000; ++i) {
        std::ignore = random.next_integral<std::int64_t>();
    }

    for (auto i = 0; i < 1000000; ++i) {
        auto const number = random.next_integral<std::uint16_t>(42);
        ASSERT_LT(number, 42);
    }
    for (auto i = 0; i < 1000000; ++i) {
        auto const number = random.next_integral<std::uint32_t>(42);
        ASSERT_LT(number, 42);
    }
    for (auto i = 0; i < 1000000; ++i) {
        auto const number = random.next_integral<std::uint64_t>(42);
        ASSERT_LT(number, 42);
    }
    for (auto i = 0; i < 1000000; ++i) {
        auto const number = random.next_integral<std::int16_t>(42);
        ASSERT_GE(number, 0);
        ASSERT_LT(number, 42);
    }
    for (auto i = 0; i < 1000000; ++i) {
        auto const number = random.next_integral<std::int32_t>(42);
        ASSERT_GE(number, 0);
        ASSERT_LT(number, 42);
    }
    for (auto i = 0; i < 1000000; ++i) {
        auto const number = random.next_integral<std::int64_t>(42);
        ASSERT_GE(number, 0);
        ASSERT_LT(number, 42);
    }

    for (auto i = 0; i < 1000000; ++i) {
        auto const number = random.next_integral<std::uint16_t>(11, 42);
        ASSERT_GE(number, 11);
        ASSERT_LT(number, 42);
    }
    for (auto i = 0; i < 1000000; ++i) {
        auto const number = random.next_integral<std::uint32_t>(11, 42);
        ASSERT_GE(number, 11);
        ASSERT_LT(number, 42);
    }
    for (auto i = 0; i < 1000000; ++i) {
        auto const number = random.next_integral<std::uint64_t>(11, 42);
        ASSERT_GE(number, 11);
        ASSERT_LT(number, 42);
    }
    for (auto i = 0; i < 1000000; ++i) {
        auto const number = random.next_integral<std::int16_t>(11, 42);
        ASSERT_GE(number, 11);
        ASSERT_LT(number, 42);
    }
    for (auto i = 0; i < 1000000; ++i) {
        auto const number = random.next_integral<std::int32_t>(11, 42);
        ASSERT_GE(number, 11);
        ASSERT_LT(number, 42);
    }
    for (auto i = 0; i < 1000000; ++i) {
        auto const number = random.next_integral<std::int64_t>(11, 42);
        ASSERT_GE(number, 11);
        ASSERT_LT(number, 42);
    }

    for (auto i = 0; i < 1000000; ++i) {
        auto const number = random.next_integral(-10, -5);
        ASSERT_GE(number, -10);
        ASSERT_LT(number, -5);
    }
}

TEST(RandomTests, Byte) {
    using c2k::Random;
    auto random = Random{};
    for (auto i = 0; i < 1000000; ++i) {
        [[maybe_unused]] auto const value = random.next_byte();
        static_assert(std::same_as<std::remove_cvref_t<decltype(value)>, std::byte>);
    }
    while (true) {
        auto const value = random.next_byte();
        if (static_cast<int>(value) == 0) {
            break;
        }
    }
    while (true) {
        auto const value = random.next_byte();
        if (static_cast<int>(value) == 255) {
            break;
        }
    }
    while (true) {
        auto const value = random.next_byte();
        if (static_cast<int>(value) == 42) {
            break;
        }
    }
}

TEST(RandomTests, Unsigned8) {
    using c2k::Random;
    auto random = Random{};
    for (auto i = 0; i < 1000000; ++i) {
        [[maybe_unused]] auto const value = random.next_integral<std::uint8_t>();
        static_assert(std::same_as<std::remove_cvref_t<decltype(value)>, std::uint8_t>);
    }
    while (true) {
        auto const value = random.next_byte();
        if (static_cast<int>(value) == 0) {
            break;
        }
    }
    while (true) {
        auto const value = random.next_byte();
        if (static_cast<int>(value) == 255) {
            break;
        }
    }
    while (true) {
        auto const value = random.next_byte();
        if (static_cast<int>(value) == 42) {
            break;
        }
    }
}

TEST(RandomTests, Char) {
    using c2k::Random;
    auto random = Random{};
    for (auto i = 0; i < 1000000; ++i) {
        [[maybe_unused]] auto const value = random.next_char();
        static_assert(std::same_as<std::remove_cvref_t<decltype(value)>, char>);
    }
    while (true) {
        auto const value = random.next_char();
        if (value == std::numeric_limits<char>::max()) {
            break;
        }
    }
    while (true) {
        auto const value = random.next_char();
        if (value == std::numeric_limits<char>::min()) {
            break;
        }
    }
    while (true) {
        auto const value = random.next_char();
        if (value == 'a') {
            break;
        }
    }
    while (true) {
        auto const value = random.next_char();
        if (value == '\n') {
            break;
        }
    }
}

TEST(RandomTests, Bool) {
    using c2k::Random;
    auto random = Random{};
    for (auto i = 0; i < 1000000; ++i) {
        [[maybe_unused]] auto const value = random.next_bool();
        static_assert(std::same_as<std::remove_cvref_t<decltype(value)>, bool>);
    }
    while (true) {
        auto const value = random.next_bool();
        if (value) {
            break;
        }
    }
    while (true) {
        auto const value = random.next_bool();
        if (not value) {
            break;
        }
    }
}

TEST(RandomTests, Float) {
    using c2k::Random;
    auto random = Random{};
    for (auto i = 0; i < 1000000; ++i) {
        auto const value = random.next_float();
        static_assert(std::same_as<std::remove_cvref_t<decltype(value)>, float>);
        EXPECT_GE(value, 0.0f);
        EXPECT_LE(value, 1.0f);
    }
    while (true) {
        auto const value = random.next_float();
        if (value >= 0.5f and value <= 0.6f) {
            break;
        }
    }
}

TEST(RandomTests, Double) {
    using c2k::Random;
    auto random = Random{};
    for (auto i = 0; i < 1000000; ++i) {
        auto const value = random.next_double();
        static_assert(std::same_as<std::remove_cvref_t<decltype(value)>, double>);
        EXPECT_GE(value, 0.0);
        EXPECT_LE(value, 1.0);
    }
    while (true) {
        auto const value = random.next_double();
        if (value >= 0.5 and value <= 0.6) {
            break;
        }
    }
}

TEST(RandomTest, InvalidArguments) {
    using c2k::Random;
    auto random = Random{};
    EXPECT_THROW({ std::ignore = random.next_integral(0); }, std::invalid_argument);
    EXPECT_THROW({ std::ignore = random.next_integral(-1); }, std::invalid_argument);
    EXPECT_THROW({ std::ignore = random.next_integral(-10); }, std::invalid_argument);

    EXPECT_THROW({ std::ignore = random.next_integral(0, 0); }, std::invalid_argument);
    EXPECT_THROW({ std::ignore = random.next_integral(10, 1); }, std::invalid_argument);
    EXPECT_THROW({ std::ignore = random.next_integral(10, 10); }, std::invalid_argument);
    EXPECT_THROW({ std::ignore = random.next_integral(-5, -5); }, std::invalid_argument);
    EXPECT_THROW({ std::ignore = random.next_integral(-5, -10); }, std::invalid_argument);
}

TEST(RandomTest, RandomRange) {
    using c2k::Random;
    auto random = Random{};
    for (auto i = 0; i < 1000000; ++i) {
        auto const number = random.next_integral(42, 100);
        EXPECT_GE(number, 42);
        EXPECT_LT(number, 100);
    }
}

TEST(RandomTest, BytesArray) {
    using c2k::Random;
    auto random = Random{};

    {
        auto const bytes = random.bytes_array<0>();
        static_assert(bytes.empty());
    }

    {
        auto bytes = random.bytes_array<1>();
        static_assert(bytes.size() == 1);
        while (bytes.at(0) != std::byte{ 42 }) {
            bytes = random.bytes_array<1>();
        }
    }

    auto const perform_no_zero_test = [&random]<std::size_t num_bytes>() {
        auto bytes = random.bytes_array<num_bytes>();
        static_assert(bytes.size() == num_bytes);
        while (true) {
            auto no_zero = true;
            for (auto const byte : bytes) {
                if (byte == std::byte{ 0 }) {
                    no_zero = false;
                    break;
                }
            }
            if (no_zero) {
                break;
            }
            bytes = random.bytes_array<num_bytes>();
        }
    };
    perform_no_zero_test.operator()<2>();
    perform_no_zero_test.operator()<3>();
    perform_no_zero_test.operator()<4>();
    perform_no_zero_test.operator()<5>();
    perform_no_zero_test.operator()<6>();
    perform_no_zero_test.operator()<7>();
    perform_no_zero_test.operator()<8>();
    perform_no_zero_test.operator()<9>();
    perform_no_zero_test.operator()<1024>();
}

TEST(RandomTest, BytesVector) {
    using c2k::Random;
    auto random = Random{};

    {
        auto const bytes = random.bytes_vector(0);
        EXPECT_TRUE(bytes.empty());
    }

    {
        auto bytes = random.bytes_vector(1);
        EXPECT_EQ(bytes.size(), 1);
        while (bytes.at(0) != std::byte{ 42 }) {
            bytes = random.bytes_vector(1);
        }
    }

    auto const perform_no_zero_test = [&random](std::size_t const num_bytes) {
        auto bytes = random.bytes_vector(num_bytes);
        EXPECT_EQ(bytes.size(), num_bytes);
        while (true) {
            auto no_zero = true;
            for (auto const byte : bytes) {
                if (byte == std::byte{ 0 }) {
                    no_zero = false;
                    break;
                }
            }
            if (no_zero) {
                break;
            }
            bytes = random.bytes_vector(num_bytes);
        }
    };
    perform_no_zero_test(2);
    perform_no_zero_test(3);
    perform_no_zero_test(4);
    perform_no_zero_test(5);
    perform_no_zero_test(6);
    perform_no_zero_test(7);
    perform_no_zero_test(8);
    perform_no_zero_test(9);
    perform_no_zero_test(1024);
}

TEST(RandomTest, Shuffle) {
    using c2k::Random;
    auto random = Random{};
    auto vec = std::vector{ 42, 13, 8, -2, 12, -17 };
    while (true) {
        random.shuffle(vec);
        if (vec == std::vector{ 12, -2, 42, -17, 8, 13 }) {
            break;
        }
    }

    auto array = std::array{ 42, 13, 8, -2, 12, -17 };
    while (true) {
        random.shuffle(array);
        if (array == std::array{ 12, -2, 42, -17, 8, 13 }) {
            break;
        }
    }

    int c_array[] = { 42, 13, 8, -2, 12, -17 };
    while (true) {
        random.shuffle(c_array);
        if (std::vector(c_array, c_array + std::size(c_array)) == std::vector{ 12, -2, 42, -17, 8, 13 }) {
            break;
        }
    }
}

TEST(RandomTest, Choice) {
    using c2k::Random;
    auto random = Random{};

    auto const vec = std::vector{ 42, 13, 8, -2, 12, -17 };
    for (auto const value : vec) {
        while (true) {
            if (random.choice(vec) == value) {
                break;
            }
        }
    }

    auto const array = std::array{ 42, 13, 8, -2, 12, -17 };
    for (auto const value : vec) {
        while (true) {
            if (random.choice(array) == value) {
                break;
            }
        }
    }

    auto const c_array = std::array{ 42, 13, 8, -2, 12, -17 };
    for (auto const value : vec) {
        while (true) {
            if (random.choice(c_array) == value) {
                break;
            }
        }
    }
}
