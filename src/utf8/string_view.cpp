#include <lib2k/utf8/string.hpp>
#include <lib2k/utf8/string_view.hpp>
#include <utf8proc.h>

namespace c2k {

    Utf8StringView::Utf8StringView(Utf8String const& string) : m_view{ string.m_data } { }

    Utf8StringView::Utf8StringView(std::string const& string) {
        if (not Utf8String::is_valid_utf8(string)) {
            throw InvalidUtf8String{};
        }
        *this = from_string_view_unchecked(string);
    }

    Utf8StringView::Utf8StringView(detail::Utf8ConstIterator const& begin, detail::Utf8ConstIterator const& end)
        : m_view{ reinterpret_cast<char const*>(begin.m_next_char_start),
                  reinterpret_cast<char const*>(end.m_next_char_start) } { }

    Utf8StringView::Utf8StringView(std::string_view const view) {
        if (not Utf8String::is_valid_utf8(view)) {
            throw InvalidUtf8String{};
        }
        *this = from_string_view_unchecked(view);
    }

    Utf8StringView::Utf8StringView(char const* const chars) : Utf8StringView{ std::string_view{ chars } } {
        if (chars == nullptr) {
            throw std::invalid_argument{ "cannot create Utf8String from nullptr" };
        }
    }

    [[nodiscard]] Utf8StringView Utf8StringView::from_string_view_unchecked(std::string_view const view) {
        auto result = Utf8StringView{};
        result.m_view = view;
        return result;
    }

    [[nodiscard]] std::size_t Utf8StringView::calculate_char_width() const {
        auto width = std::size_t{ 0 };
        auto current = reinterpret_cast<utf8proc_uint8_t const*>(m_view.data());

        while (current < reinterpret_cast<utf8proc_uint8_t const*>(m_view.data() + m_view.length())) {
            auto codepoint = utf8proc_int32_t{};
            auto const bytes_read = utf8proc_iterate(current, -1, &codepoint);
            // clang-format off
            assert(
                codepoint != -1
                and utf8proc_codepoint_valid(codepoint)
                and "Utf8String objects must always contain valid UTF8"
            );
            // clang-format on
            width += static_cast<std::size_t>(utf8proc_charwidth(codepoint));
            current += bytes_read;
        }
        // clang-format off
        assert(
            current == reinterpret_cast<utf8proc_uint8_t const*>(m_view.data() + m_view.length())
            and "Utf8String objects must always contain valid UTF8"
        );
        // clang-format on
        return width;
    }

    [[nodiscard]] Utf8StringView Utf8StringView::substring(ConstIterator const begin, ConstIterator const end) const {
        return Utf8StringView{ begin, end };
    }

    [[nodiscard]] Utf8StringView Utf8StringView::substring(ConstIterator const begin) const {
        return substring(begin, this->cend());
    }

    // clang-format off
    [[nodiscard]] Utf8StringView Utf8StringView::substring(
        ConstIterator const begin,
        std::size_t const num_chars
    ) const { // clang-format on
        return substring(begin, begin + static_cast<ConstIterator::difference_type>(num_chars));
    }

    [[nodiscard]] Utf8StringView Utf8StringView::substring(std::size_t const start, std::size_t const num_chars) const {
        auto const begin = this->cbegin() + static_cast<ConstIterator::difference_type>(start);
        auto const end = begin + static_cast<ConstIterator::difference_type>(num_chars);
        return substring(begin, end);
    }

    [[nodiscard]] Utf8StringView Utf8StringView::substring(std::size_t const start) const {
        auto const begin = this->cbegin() + static_cast<ConstIterator::difference_type>(start);
        return substring(begin, this->cend());
    }

    [[nodiscard]] Utf8Char Utf8StringView::front() const {
        if (is_empty()) {
            throw std::out_of_range{ "cannot call front() on empty string view" };
        }
        return *cbegin();
    }

    [[nodiscard]] Utf8Char Utf8StringView::back() const {
        if (is_empty()) {
            throw std::out_of_range{ "cannot call back() on empty string view" };
        }
        return *(cend() - 1);
    }

    [[nodiscard]] Utf8StringView::ConstIterator Utf8StringView::find(Utf8Char const needle) const {
        return find(needle, cbegin());
    }

    // clang-format off
    [[nodiscard]] Utf8StringView::ConstIterator Utf8StringView::find(
        Utf8Char const needle,
        ConstIterator const& start
    ) const { // clang-format on
        auto const start_offset = reinterpret_cast<char const*>(start.m_next_char_start) - m_view.data();
        auto const substring_data = m_view.substr(start_offset);
        assert(not needle.m_codepoint.empty());
        auto const needle_substring = std::string_view{
            reinterpret_cast<char const*>(needle.m_codepoint.data()),
            reinterpret_cast<char const*>(needle.m_codepoint.data() + needle.m_codepoint.size()),
        };
        auto const position = substring_data.find(needle_substring);
        if (position == std::string_view::npos) {
            return cend();
        }
        return ConstIterator{
            reinterpret_cast<std::byte const*>(
                    &*(substring_data.begin() + static_cast<std::string_view::difference_type>(position))
            ),
        };
    }

    // clang-format off
    [[nodiscard]] Utf8StringView::ConstIterator Utf8StringView::find(
        Utf8Char const needle,
        ConstIterator::difference_type const start_position
    ) const { // clang-format on
        return find(needle, cbegin() + start_position);
    }

    [[nodiscard]] Utf8StringView::ConstIterator Utf8StringView::find(Utf8StringView const needle) const {
        return find(needle, cbegin());
    }

    // clang-format off
    [[nodiscard]] Utf8StringView::ConstIterator Utf8StringView::find(
        Utf8StringView const needle,
        ConstIterator const& start
    ) const { // clang-format on
        auto const start_offset = reinterpret_cast<char const*>(start.m_next_char_start) - m_view.data();
        auto const substring_data = m_view.substr(start_offset);
        auto const needle_string = std::string_view{ needle.m_view };
        auto const position = substring_data.find(needle_string);
        if (position == std::string_view::npos) {
            return cend();
        }
        return ConstIterator{
            reinterpret_cast<std::byte const*>(
                    &*(substring_data.begin() + static_cast<std::string_view::difference_type>(position))
            ),
        };
    }

    // clang-format off
    Utf8StringView::ConstIterator Utf8StringView::find(
        Utf8StringView const needle,
        ConstIterator::difference_type const start_position
    ) const { // clang-format on
        return find(needle, cbegin() + start_position);
    }

    [[nodiscard]] std::vector<Utf8StringView> Utf8StringView::split(Utf8StringView const delimiter) {
        if (delimiter.is_empty()) {
            throw std::invalid_argument{ "cannot split string with empty delimiter" };
        }

        auto find_iterator = find(delimiter);
        if (find_iterator == cend()) {
            return { *this };
        }

        auto const delimiter_char_count = delimiter.calculate_char_count();

        auto result = std::vector<Utf8StringView>{};
        decltype(find_iterator) last_iterator = cbegin();
        while (find_iterator != cend()) {
            result.push_back(substring(last_iterator, find_iterator));
            last_iterator = find_iterator + static_cast<ConstIterator::difference_type>(delimiter_char_count);
            find_iterator = find(delimiter, last_iterator);
        }
        result.push_back(substring(last_iterator));
        return result;
    }

    [[nodiscard]] Utf8String Utf8StringView::replace(
            Utf8StringView const to_replace,
            Utf8StringView const replacement,
            ConstIterator const& start,
            MaxReplacementCount const max_num_replacements
    ) const {
        if (std::to_underlying(max_num_replacements) == 0) {
            return *this;
        }

        if (start == cend()) {
            if (to_replace.is_empty()) {
                return replacement;
            }
            return *this;
        }

        if (to_replace.is_empty()) {
            auto result = Utf8String{};
            result += replacement;
            for (auto const c : *this) {
                result += c;
                result += replacement;
            }
            return result;
        }

        auto current = find(to_replace, start);
        if (current == cend()) {
            return *this;
        }
        auto result = Utf8String{};
        auto num_replacements = std::underlying_type_t<decltype(max_num_replacements)>{ 0 };
        auto previous = cbegin();
        while (current != cend()) {
            if (current != previous) {
                result += Utf8StringView{ previous, current };
            }
            result += replacement;
            ++num_replacements;
            current += static_cast<ConstIterator::difference_type>(to_replace.calculate_char_count());
            previous = current;
            if (current == cend()) {
                break;
            }
            if (num_replacements >= std::to_underlying(max_num_replacements)) {
                result += Utf8StringView{ current, cend() };
                break;
            }
            current = find(to_replace, current);
            if (current == cend()) {
                result += Utf8StringView{ previous, cend() };
                break;
            }
        }
        if (current != cend() and num_replacements < std::to_underlying(max_num_replacements)) {
            result += Utf8StringView{ current, cend() };
        }

        return result;
    }
} // namespace c2k
