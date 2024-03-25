#pragma once

#include <array>
#include <cstring>
#include <format>
#include <lib2k/concepts.hpp>
#include <random>
#include <vector>

namespace c2k {
    // clang-format off
    template<typename T>
    concept UniformIntDistributionType = IsOneOf<
        T,
        short,
        int,
        long,
        long long,
        unsigned short,
        unsigned int,
        unsigned long,
        unsigned long long>;
    // clang-format on

    class Random final {
    public:
        struct State final {
            using NativeRepresentation = std::string;

            NativeRepresentation value;
        };

    private:
        std::mt19937_64 m_generator;

    public:
        using Seed = decltype(m_generator)::result_type;

        Random();
        explicit Random(Seed seed);

        template<UniformIntDistributionType T>
        [[nodiscard]] auto next_integral() {
            return std::uniform_int_distribution<T>{}(m_generator);
        }

        template<UniformIntDistributionType T>
        [[nodiscard]] auto next_integral(T const max_exclusive) {
            if (max_exclusive <= T{ 0 }) {
                throw std::invalid_argument{
                    std::format("'{}' is not a valid upper bound (must be greater than 0)", max_exclusive)
                };
            }
            return next_integral(T{ 0 }, max_exclusive);
        }

        template<UniformIntDistributionType T>
        [[nodiscard]] auto next_integral(T const min_inclusive, T const max_exclusive) {
            if (max_exclusive <= min_inclusive) {
                throw std::invalid_argument{
                    std::format("upper bound {} must be greater than lower bound {}", max_exclusive, min_inclusive)
                };
            }
            return std::uniform_int_distribution<T>{ min_inclusive,
                                                     static_cast<T>(max_exclusive - T{ 1 }) }(m_generator);
        }

        [[nodiscard]] auto next_bool() {
            return static_cast<bool>(std::uniform_int_distribution{ 0, 1 }(m_generator));
        }

        [[nodiscard]] auto next_byte() {
            return static_cast<std::byte>(std::uniform_int_distribution{
                    0,
                    static_cast<int>(std::numeric_limits<std::byte>::max()),
            }(m_generator));
        }

        [[nodiscard]] auto next_char() {
            return static_cast<char>(std::uniform_int_distribution{
                    0,
                    static_cast<int>(std::numeric_limits<char>::max()),
            }(m_generator));
        }

        [[nodiscard]] auto next_float() {
            return std::uniform_real_distribution{ 0.0f, 1.0f }(m_generator);
        }

        [[nodiscard]] auto next_double() {
            return std::uniform_real_distribution{ 0.0, 1.0 }(m_generator);
        }

        template<std::size_t size>
        [[nodiscard]] auto bytes_array() {
            using DistributionType = std::uint64_t;
            static constexpr auto chunk_size = sizeof(DistributionType);
            static_assert(UniformIntDistributionType<DistributionType>);
            auto distribution = std::uniform_int_distribution<DistributionType>{};

            auto result = std::array<std::byte, size>{};
            for (std::size_t i = 0; i < result.size(); i += chunk_size) {
                auto value = distribution(m_generator);
                auto const current_chunk_size = std::min(result.size() - i, chunk_size);
                std::memcpy(result.data() + i, reinterpret_cast<char const*>(&value), current_chunk_size);
            }
            return result;
        }

        [[nodiscard]] std::vector<std::byte> bytes_vector(std::size_t size);

        [[nodiscard]] State state() const;

        void state(State const& new_state);
    };
} // namespace c2k
