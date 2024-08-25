#pragma once

#include <optional>
#include <utility>

namespace c2k {
    template<typename Function>
    class Defer final {
    private:
        std::optional<Function> m_function;

    public:
        [[nodiscard]] explicit Defer(Function function) : m_function{ std::move(function) } { }

        Defer(Defer const& other) = delete;
        Defer(Defer&& other) noexcept = delete;
        Defer& operator=(Defer const& other) = delete;
        Defer& operator=(Defer&& other) noexcept = delete;

        ~Defer() noexcept {
            if (m_function.has_value()) {
                (*m_function)();
            }
        }
    };
} // namespace c2k
