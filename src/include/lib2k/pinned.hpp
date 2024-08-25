#pragma once

namespace c2k {
    template<typename T>
    struct Pinned : T {
        using T::T;

        Pinned(Pinned const&) = delete;
        Pinned(Pinned&&) = delete;
        Pinned& operator=(Pinned const&) = delete;
        Pinned& operator=(Pinned&&) = delete;
        ~Pinned() = default;
    };
} // namespace c2k
