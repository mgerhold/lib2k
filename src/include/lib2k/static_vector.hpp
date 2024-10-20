#pragma once

#include <algorithm>
#include <array>
#include <format>
#include <stdexcept>

namespace c2k {
    class InsufficientCapacity final : public std::runtime_error {
    public:
        explicit InsufficientCapacity(std::size_t const capacity)
            : std::runtime_error{ std::format("insufficient capacity '{}'", capacity) } { }
    };

    template<typename T, std::size_t max_capacity>
    class StaticVector final {
    private:
        std::array<T, max_capacity> m_data{};
        std::size_t m_size{ 0 };

    public:
        constexpr StaticVector() = default;

        constexpr StaticVector(std::initializer_list<T> const elements) {
            if (elements.size() > max_capacity) {
                throw InsufficientCapacity{ max_capacity };
            }
            std::copy(elements.begin(), elements.end(), m_data.begin());
            m_size = elements.size();
        }

        [[nodiscard]] constexpr bool empty() const {
            return m_size == 0;
        }

        [[nodiscard]] constexpr std::size_t size() const {
            return m_size;
        }

        [[nodiscard]] static constexpr std::size_t capacity() {
            return max_capacity;
        }

        [[nodiscard]] constexpr T const* data() const {
            return m_data.data();
        }

        [[nodiscard]] constexpr T* data() {
            return m_data.data();
        }

        [[nodiscard]] constexpr auto begin() {
            return m_data.begin();
        }

        [[nodiscard]] constexpr auto begin() const {
            return m_data.begin();
        }

        [[nodiscard]] constexpr auto cbegin() const {
            return begin();
        }

        [[nodiscard]] constexpr auto end() {
            return m_data.begin() + size();
        }

        [[nodiscard]] constexpr auto end() const {
            return m_data.begin() + size();
        }

        [[nodiscard]] constexpr auto cend() const {
            return end();
        }

        [[nodiscard]] constexpr auto rbegin() {
            return m_data.rbegin();
        }

        [[nodiscard]] constexpr auto rbegin() const {
            return m_data.rbegin();
        }

        [[nodiscard]] constexpr auto crbegin() const {
            return rbegin();
        }

        [[nodiscard]] constexpr auto rend() {
            return m_data.rend();
        }

        [[nodiscard]] constexpr auto rend() const {
            return m_data.rend();
        }

        [[nodiscard]] constexpr auto crend() const {
            return rend();
        }

        [[nodiscard]] T const& at(std::size_t const index) const {
            return m_data.at(index);
        }

        [[nodiscard]] T& at(std::size_t const index) {
            return m_data.at(index);
        }

        [[nodiscard]] T const& operator[](std::size_t const index) const {
            return at(index);
        }

        [[nodiscard]] T& operator[](std::size_t const index) {
            return at(index);
        }

        [[nodiscard]] T const& front() const {
            return at(0);
        }

        [[nodiscard]] T& front() {
            return at(0);
        }

        [[nodiscard]] T const& back() const {
            return at(size() - 1);
        }

        [[nodiscard]] T& back() {
            return at(size() - 1);
        }

        constexpr void push_back(T value) {
            if (m_size >= capacity()) {
                throw InsufficientCapacity{ capacity() };
            }
            m_data[m_size] = std::move(value);
            ++m_size;
        }

        constexpr void emplace_back(auto&&... args) {
            if (m_size >= capacity()) {
                throw InsufficientCapacity{ capacity() };
            }
            m_data[m_size] = T{ std::forward<decltype(args)>(args)... };
            ++m_size;
        }

        [[nodiscard]] constexpr bool operator==(StaticVector const& other) const {
            if (size() != other.size()) {
                return false;
            }
            for (auto i = std::size_t{ 0 }; i < size(); ++i) {
                if (at(i) != other.at(i)) {
                    return false;
                }
            }
            return true;
        }
    };
} // namespace c2k
