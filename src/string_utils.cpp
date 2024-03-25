#include <lib2k/string_utils.hpp>

namespace c2k {
    [[nodiscard]] std::vector<std::string> split(std::string const& s, std::string_view const delimiter) {
        if (delimiter.empty()) {
            throw std::invalid_argument{ "cannot split string with empty delimiter" };
        }

        auto pos = s.find(delimiter);
        if (pos == std::string::npos) {
            return { s };
        }

        auto result = std::vector<std::string>{};
        decltype(pos) last_pos = 0;
        while (pos != std::string::npos) {
            result.push_back(s.substr(last_pos, pos - last_pos));
            last_pos = pos + delimiter.length();
            pos = s.find(delimiter, pos + delimiter.length());
        }

        if (last_pos <= s.length()) {
            result.push_back(s.substr(last_pos));
        }

        return result;
    }
}
