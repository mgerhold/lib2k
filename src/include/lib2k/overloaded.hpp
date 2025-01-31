#pragma once

namespace c2k {
    template<typename... Ts>
    struct Overloaded : Ts... {
        using Ts::operator()...;
    };
} // namespace c2k
