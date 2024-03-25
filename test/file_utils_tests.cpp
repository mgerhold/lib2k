#include <array>
#include <gtest/gtest.h>
#include <lib2k/file_utils.hpp>
#include <lib2k/string_utils.hpp>

TEST(FileUtilsTests, OpeningNonExistentFileFails) {
    EXPECT_FALSE(c2k::read_file("does_not_exist.dat").has_value());
}

TEST(FileUtilsTests, WritingToInvalidPathFails) {
    auto const path = std::filesystem::path{ "/not/a/valid/path.dat" };
    EXPECT_FALSE(c2k::write_file(path, "some content").has_value());
    EXPECT_FALSE(exists(path));
}

TEST(FileUtilsTests, WriteAndRead) {
    static constexpr auto contents = "these are just some test contents for the file";
    auto const path = std::filesystem::path{ "temp.txt" };
    ASSERT_TRUE(c2k::write_file(path, contents).has_value());
    auto const read = c2k::read_file(path);
    ASSERT_TRUE(read.has_value());
    EXPECT_EQ(read.value(), contents);
    EXPECT_TRUE(std::filesystem::remove(path));
}

TEST(FileUtilsTests, WriteAppendAndRead) {
    static constexpr auto first_part = "this is the first part\n";
    static constexpr auto parts = std::array{
        "these are just",
        "some random C-strings",
        "that we want to write to a file",
        "some can even contain\nline breaks,\nbut they don't have to",
    };
    auto const path = std::filesystem::path{ "temp.txt" };
    ASSERT_TRUE(c2k::write_file(path, first_part).has_value());
    for (auto const part : parts) {
        ASSERT_TRUE(c2k::write_file(path, part, c2k::WriteFileBehavior::Append).has_value());
    }

    auto const read = c2k::read_file(path);
    ASSERT_TRUE(read.has_value());
    EXPECT_EQ(read.value(), first_part + c2k::join(parts, ""));
    EXPECT_TRUE(std::filesystem::remove(path));
}
