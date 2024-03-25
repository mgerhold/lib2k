#include <fstream>
#include <lib2k/file_utils.hpp>
#include <sstream>

namespace c2k {

    [[nodiscard]] tl::expected<std::string, ReadFileError> read_file(std::filesystem::path const& path) {
        auto file = std::ifstream{ path };
        if (not file) {
            return tl::unexpected{ ReadFileError::UnableToOpenFileForReading };
        }
        auto stream = std::ostringstream{};
        stream << file.rdbuf();
        if (not stream) {
            return tl::unexpected{ ReadFileError::UnableToReadFromFile };
        }
        return std::move(stream).str();
    }


    // clang-format off
    [[nodiscard]] tl::expected<void, WriteFileError> write_file(
        std::filesystem::path const& path,
        std::string_view const contents,
        WriteFileBehavior const behavior
    ) { // clang-format on
        auto const mode = static_cast<std::ios_base::openmode>(
                std::ios_base::out | (behavior == WriteFileBehavior::Append ? std::ios_base::app : 0)
        );
        auto file = std::ofstream{ path, mode };
        if (not file) {
            return tl::unexpected{ WriteFileError::UnableToOpenFileForWriting };
        }
        file << contents;
        if (not file) {
            return tl::unexpected{ WriteFileError::UnableToWriteToFile };
        }
        return {};
    }
} // namespace c2k
