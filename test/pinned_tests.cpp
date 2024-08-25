#include <concepts>
#include <gtest/gtest.h>
#include <lib2k/pinned.hpp>

TEST(PinnedTests, PinnedTests) {
    using PinnedString = c2k::Pinned<std::string>;

    static_assert(std::is_object_v<PinnedString>);

    static_assert(not std::move_constructible<PinnedString>);
    static_assert(not std::assignable_from<PinnedString&, PinnedString>);
    static_assert(not std::swappable<PinnedString>);

    static_assert(not std::copy_constructible<PinnedString>);
    static_assert(not std::movable<PinnedString>);
    static_assert(not std::assignable_from<PinnedString&, PinnedString&>);
    static_assert(not std::assignable_from<PinnedString&, PinnedString const&>);
    static_assert(not std::assignable_from<PinnedString&, PinnedString const>);
}
