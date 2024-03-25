#pragma once

#include <filesystem>
#include <string>
#include <tl/expected.hpp>

namespace c2k {
    enum class WriteFileBehavior {
        Overwrite,
        Append,
    };

    enum class ReadFileError {
        UnableToOpenFileForReading,
        UnableToReadFromFile,
    };

    enum class WriteFileError {
        UnableToOpenFileForWriting,
        UnableToWriteToFile,
    };

    [[nodiscard]] tl::expected<std::string, ReadFileError> read_file(std::filesystem::path const& path);

    [[nodiscard]] tl::expected<void, WriteFileError> write_file(
            std::filesystem::path const& path,
            std::string_view contents,
            WriteFileBehavior behavior = WriteFileBehavior::Overwrite
    );
} // namespace c2k
