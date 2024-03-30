#pragma once

#include <functional>
#include <optional>
#include <utility>

#ifdef _MSC_VER
#if _MSC_VER >= 1929
#define LIB2K_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#else
#error "MSVC compiler not capable of applying [[msvc::no_unique_address]], upgrade your compiler"
#endif
#else
#define LIB2K_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

namespace c2k {

    namespace detail {
        template<typename T>
        struct UniqueValueDefaultDeleter final {
            constexpr void operator()(T const&) { }
        };
    } // namespace detail

    /**
     * @class UniqueValue
     * @brief Represents a unique value with a customizable deleter.
     *
     * This class stores a single value of type T, which can be accessed through
     * member functions. The value is owned uniquely by this class and will be
     * deleted when the object is destroyed or when a new value is assigned to it.
     * Additionally, the user can provide a callable object (deleter) to be called
     * when the value needs to be deleted.
     * This is similar to `std::unique_ptr<T>`, except that it doesn't manage the
     * object via a pointer. Use-cases for this can be non-pointer resource handles,
     * for example file descriptors (files, sockets, ...) or GPU buffers.
     */
    template<typename T, std::invocable<T> Deleter = detail::UniqueValueDefaultDeleter<T>>
    class UniqueValue final {
    private:
        std::optional<T> m_value;
        LIB2K_NO_UNIQUE_ADDRESS Deleter m_deleter;

    public:
        explicit UniqueValue(T value) : UniqueValue{ std::move(value), Deleter{} } { }
        UniqueValue(T value, Deleter deleter) : m_value{ std::move(value) }, m_deleter{ std::move(deleter) } { }

        UniqueValue(UniqueValue const& other) = delete;
        UniqueValue& operator=(UniqueValue const& other) = delete;

        UniqueValue(UniqueValue&& other) noexcept
            : m_value{ std::exchange(other.m_value, std::nullopt) },
              m_deleter{ std::move(other.m_deleter) } { }

        UniqueValue& operator=(UniqueValue&& other) noexcept {
            if (this == std::addressof(other)) {
                return *this;
            }
            using std::swap;
            swap(m_value, other.m_value);
            swap(m_deleter, other.m_deleter);
            return *this;
        }

        ~UniqueValue() noexcept {
            // this static_assert is inside the destructor since the destructor is (almost?) guaranteed
            // to be instantiated when this class template is instantiated
            // clang-format off
            static_assert(
                sizeof(UniqueValue) == sizeof(std::optional<T>)
                or not std::is_empty_v<Deleter>,
                "compiler did not successfully apply [[no_unique_address]] attribute"
            );
            // clang-format on

            if (m_value.has_value()) {
                m_deleter(m_value.value());
            }
        }

        [[nodiscard]] explicit operator T const&() const {
            return value();
        }

        [[nodiscard]] explicit operator T&() {
            return value();
        }

        [[nodiscard]] bool has_value() const {
            return m_value.has_value();
        }

        [[nodiscard]] T const& value() const {
            return m_value.value();
        }

        [[nodiscard]] T& value() {
            return m_value.value();
        }

        [[nodiscard]] T const& operator*() const {
            return value();
        }

        [[nodiscard]] T& operator*() {
            return value();
        }
    };
} // namespace c2k
