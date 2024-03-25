#include <lib2k/random.hpp>
#include <sstream>

namespace c2k {
    Random::Random() : m_generator{ std::random_device{}() } { }

    Random::Random(Seed const seed) : m_generator{ seed } { }

    [[nodiscard]] std::vector<std::byte> Random::bytes_vector(std::size_t const size) {
        using DistributionType = std::uint64_t;
        static constexpr auto chunk_size = sizeof(DistributionType);
        static_assert(UniformIntDistributionType<DistributionType>);
        auto distribution = std::uniform_int_distribution<DistributionType>{};

        auto result = std::vector<std::byte>{};
        result.resize(size, std::byte{ 0 });
        for (std::size_t i = 0; i < result.size(); i += chunk_size) {
            auto value = distribution(m_generator);
            auto const current_chunk_size = std::min(result.size() - i, chunk_size);
            std::memcpy(result.data() + i, reinterpret_cast<char const*>(&value), current_chunk_size);
        }
        return result;
    }

    [[nodiscard]] Random::State Random::state() const {
        auto stream = std::ostringstream{};
        stream << m_generator;
        return State{ std::move(stream).str() };
    }

    void Random::state(State const& new_state) {
        auto stream = std::istringstream{ new_state.value };
        stream >> m_generator;
    }
} // namespace c2k
