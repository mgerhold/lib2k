#pragma once

#include <cassert>
#include <concepts>
#include <memory>
#include <utility>

namespace c2k {

    template<std::default_initializable T>
    class NonNullOwner;

    namespace detail {
        struct Clang16Satisfier final {
            /* This weird wrapper struct is only needed because clang-16 doesn't understand
             * the friend declaration of the variadic template. clang-17 seems to accept it.
             * But to lower the requirements, we use this. */
            template<std::default_initializable T, typename... Args>
            [[nodiscard]] static NonNullOwner<T> make_non_null_owner(Args&&... args);
        };
    } // namespace detail

    /**
     * @brief A class that holds a non-null ownership of an object allocated on the free store.
     *
     * This class represents a non-null ownership of an object. It wraps around a `std::unique_ptr`
     * to ensure that the owned object is never null. It provides safe access to the owned object
     * through overloaded dereference and arrow operators. A moved-from object of this type wraps
     * a default-constructed value, therefore `T` must conform the `std::default_initializable`
     * concept.
     *
     * @tparam T The type of the object being owned.
     */
    template<std::default_initializable T>
    class NonNullOwner final {
        friend struct detail::Clang16Satisfier;

    private:
        std::unique_ptr<T> m_owned;

        explicit NonNullOwner(std::unique_ptr<T> owned) : m_owned{ std::move(owned) } {
            assert(m_owned != nullptr);
        }

    public:
        NonNullOwner(NonNullOwner const& other) = delete;
        NonNullOwner& operator=(NonNullOwner const& other) = delete;

        NonNullOwner(NonNullOwner&& other) noexcept : m_owned{ std::exchange(other.m_owned, std::make_unique<T>()) } { }

        NonNullOwner& operator=(NonNullOwner&& other) noexcept {
            if (this == std::addressof(other)) {
                return *this;
            }
            m_owned = std::exchange(other.m_owned, std::make_unique<T>());
            return *this;
        }

        [[nodiscard]] T const& operator*() const {
            return *m_owned;
        }

        [[nodiscard]] T& operator*() {
            return *m_owned;
        }

        [[nodiscard]] T const* operator->() const {
            return m_owned.get();
        }

        [[nodiscard]] T* operator->() {
            return m_owned.get();
        }
    };

    template<std::default_initializable T, typename... Args>
    NonNullOwner<T> detail::Clang16Satisfier::make_non_null_owner(Args&&... args) {
        static_assert(sizeof...(args) > 0);
        return NonNullOwner{ std::make_unique<T>(std::forward<Args>(args)...) };
    }

    /**
     * @brief Creates a `NonNullOwner` object.
     *
     * This function creates and returns a `NonNullOwner` object, which is used to manage ownership of a dynamically
     * allocated object that cannot be `nullptr`.
     *
     * @tparam T The type of object to be owned.
     * @tparam Args The types of arguments to be passed to the object's constructor.
     * @param args The arguments to be forwarded to the object's constructor.
     * @return A `NonNullOwner` object that owns the newly created object of type `T`.
     *
     * @see NonNullOwner
     */
    template<std::default_initializable T, typename... Args>
    [[nodiscard]] NonNullOwner<T> make_non_null_owner(Args&&... args) {
        return detail::Clang16Satisfier::make_non_null_owner<T>(std::forward<Args>(args)...);
    }
} // namespace c2k
