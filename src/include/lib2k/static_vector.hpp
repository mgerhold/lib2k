#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <format>
#include <initializer_list>
#include <memory>
#include <new>
#include <stdexcept>
#include <utility>

namespace c2k {

    namespace detail {
        template<typename T>
        concept NotDefaultInitializable = not std::default_initializable<T>;
    } // namespace detail

    class InsufficientCapacity final : public std::runtime_error {
    public:
        explicit InsufficientCapacity(std::size_t const capacity)
            : std::runtime_error{ std::format("insufficient capacity '{}'", capacity) } { }
    };

    template<typename T, std::size_t max_capacity>
    class StaticVector final {
    private:
        // Should not fail for pure standard C++ types, but could fail for types using #pragma pack.
        static_assert(sizeof(T) % alignof(T) == 0);

        struct TypeErasedStorage final {
            alignas(T) std::array<char, max_capacity * sizeof(T)> data{};
        };

        struct TypedStorage final {
            std::array<T, max_capacity> elements{};

            constexpr ~TypedStorage() noexcept = default;
        };

        using Storage = std::conditional_t<std::default_initializable<T>, TypedStorage, TypeErasedStorage>;

        Storage m_storage{};
        std::size_t m_size{ 0 };

    public:
        [[nodiscard]] constexpr StaticVector() = default;

        [[nodiscard]] constexpr StaticVector(std::initializer_list<T> const values) {
            if (values.size() > capacity()) {
                throw InsufficientCapacity{ capacity() };
            }
            for (auto const& value : values) {
                push_back(value);
            }
        }

        [[nodiscard]] constexpr StaticVector(StaticVector const& other) {
            for (auto const& value : other) {
                push_back(value);
            }
        }

        [[nodiscard]] constexpr StaticVector(StaticVector&& other) noexcept
            requires std::movable<T>
        {
            for (auto& value : other) {
                push_back(std::move(value));
            }
            other.clear();
        }

        constexpr StaticVector& operator=(StaticVector const& other) {
            if (this == std::addressof(other)) {
                return *this;
            }
            clear();
            for (auto& value : other) {
                push_back(value);
            }
            return *this;
        }

        constexpr StaticVector& operator=(StaticVector&& other) noexcept
            requires std::movable<T>
        {
            if (this == std::addressof(other)) {
                return *this;
            }
            clear();
            for (auto& value : other) {
                push_back(std::move(value));
            }
            other.clear();
            return *this;
        }

        constexpr ~StaticVector() noexcept {
            clear();
        }

        [[nodiscard]] constexpr bool empty() const {
            return size() == 0;
        }

        [[nodiscard]] constexpr std::size_t size() const {
            return m_size;
        }

        [[nodiscard]] static constexpr std::size_t capacity() {
            return max_capacity;
        }

        [[nodiscard]] constexpr T const* data() const
            requires std::default_initializable<T>
        {
            return m_storage.elements.data();
        }

        [[nodiscard]] T const* data() const {
            return std::launder(reinterpret_cast<T const*>(m_storage.data.data()));
        }

        [[nodiscard]] constexpr T* data()
            requires std::default_initializable<T>
        {
            return m_storage.elements.data();
        }

        [[nodiscard]] T* data() {
            return std::launder(reinterpret_cast<T*>(m_storage.data.data()));
        }

        [[nodiscard]] constexpr auto begin() {
            return data();
        }

        [[nodiscard]] constexpr auto begin() const {
            return data();
        }

        [[nodiscard]] constexpr auto cbegin() const {
            return begin();
        }

        [[nodiscard]] constexpr auto end() {
            return begin() + size();
        }

        [[nodiscard]] constexpr auto end() const {
            return begin() + size();
        }

        [[nodiscard]] constexpr auto cend() const {
            return end();
        }

        [[nodiscard]] constexpr auto rbegin() {
            return std::reverse_iterator<T*>{ data() + size() };
        }

        [[nodiscard]] constexpr auto rbegin() const {
            return std::reverse_iterator<T const*>{ data() + size() };
        }

        [[nodiscard]] constexpr auto crbegin() const {
            return rbegin();
        }

        [[nodiscard]] constexpr auto rend() {
            return rbegin() + size();
        }

        [[nodiscard]] constexpr auto rend() const {
            return rbegin() + size();
        }

        [[nodiscard]] constexpr auto crend() const {
            return rend();
        }

        [[nodiscard]] constexpr T const& at(std::size_t const index) const {
            if (index >= size()) {
                throw std::out_of_range{ "index out of range" };
            }

            return data()[index];
        }

        [[nodiscard]] constexpr T& at(std::size_t const index) {
            if (index >= size()) {
                throw std::out_of_range{ "index out of range" };
            }

            return data()[index];
        }

        [[nodiscard]] constexpr T const& operator[](std::size_t const index) const {
            return at(index);
        }

        [[nodiscard]] constexpr T& operator[](std::size_t const index) {
            return at(index);
        }

        [[nodiscard]] constexpr T const& front() const {
            return at(0);
        }

        [[nodiscard]] constexpr T& front() {
            return at(0);
        }

        [[nodiscard]] constexpr T const& back() const {
            return at(size() - 1);
        }

        [[nodiscard]] constexpr T& back() {
            return at(size() - 1);
        }

        constexpr void push_back(T const& value) {
            if (m_size >= capacity()) {
                throw InsufficientCapacity{ capacity() };
            }
            std::construct_at(&data()[m_size], value);
            ++m_size;
        }

        constexpr void push_back(T&& value) {
            if (m_size >= capacity()) {
                throw InsufficientCapacity{ capacity() };
            }
            std::construct_at(&data()[m_size], std::move(value));
            ++m_size;
        }

        constexpr void emplace_back(auto&&... args) {
            if (m_size >= capacity()) {
                throw InsufficientCapacity{ capacity() };
            }
            std::construct_at(&data()[m_size], std::forward<decltype(args)>(args)...);
            ++m_size;
        }

        constexpr T pop_back() {
            if (empty()) {
                throw std::out_of_range{ "pop_back on empty vector" };
            }
            auto result = std::move(back());
            if constexpr (not std::default_initializable<T>) {
                std::destroy_at(&data()[m_size - 1]);
            }
            --m_size;
            return result;
        }

        constexpr void clear() {
            if constexpr (std::default_initializable<T>) {
                // Overwrite existing objects with default-constructed objects.
                // This may be reasonable for types like std::string so that their
                // memory gets deallocated here instead of in the destructor.
                if constexpr (std::movable<T>) {
                    for (auto& value : *this) {
                        value = T{};
                    }
                } else {
                    static constexpr auto default_value = T{};
                    for (auto& value : *this) {
                        value = default_value;
                    }
                }
                m_size = 0;
            } else {
                for (auto i = std::size_t{ 0 }; i < m_size; ++i) {
                    std::destroy_at(&data()[i]);
                }
                m_size = 0;
            }
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
