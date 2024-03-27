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

    [[nodiscard]] std::string replace(
            std::string original,
            std::string_view const to_replace,
            std::string_view const replacement,
            StartPosition start_pos,
            MaxReplacementCount const max_num_replacements
    ) { // clang-format on
        if (std::to_underlying(max_num_replacements) == 0) {
            return original;
        }

        if (to_replace.empty()) {
            using Difference = decltype(original)::difference_type;
            auto const num_insertions = original.length() + 1;

            auto const result_length = num_insertions * replacement.size() + original.size();
            auto result = std::string{};
            result.resize(result_length);

            for (auto i = decltype(num_insertions){ 0 }; i < num_insertions; ++i) {
                std::copy(
                        replacement.cbegin(),
                        replacement.cend(),
                        result.begin() + static_cast<Difference>(i * (replacement.size() + 1))
                );
                if (i < num_insertions - 1) {
                    result.at((i + 1) * replacement.size() + i) = original.at(i);
                }
            }
            return result;
        }

        static constexpr auto npos = decltype(original)::npos;
        start_pos = StartPosition{ original.find(to_replace, std::to_underlying(start_pos)) };
        auto num_replacements = std::underlying_type_t<decltype(max_num_replacements)>{ 0 };
        while (std::to_underlying(start_pos) != npos) {
            original.replace(std::to_underlying(start_pos), to_replace.length(), replacement);
            ++num_replacements;
            if (num_replacements >= std::to_underlying(max_num_replacements)) {
                break;
            }
            start_pos = StartPosition{ std::to_underlying(start_pos) + replacement.length() };
            if (std::to_underlying(start_pos) == original.length()) {
                break;
            }
            start_pos = StartPosition{ original.find(to_replace, std::to_underlying(start_pos)) };
        }
        return original;
    }
} // namespace c2k
